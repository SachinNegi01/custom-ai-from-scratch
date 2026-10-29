#pragma once

#include "../httplib.h"
#include "vector_engine.hpp"
#include <utility>

// Split long documents into overlapping word windows for retrieval.
inline std::vector<std::string> chunkText(const std::string& text,
                                          int chunkWords = 250,
                                          int overlapWords = 30) {
    std::istringstream input(text);
    std::vector<std::string> words;
    std::string word;
    while (input >> word) words.push_back(word);
    if (words.empty()) return {};
    if (chunkWords <= 0) chunkWords = 250;
    overlapWords = std::max(0, std::min(overlapWords, chunkWords - 1));
    if (static_cast<int>(words.size()) <= chunkWords) return {text};

    const int step = chunkWords - overlapWords;
    std::vector<std::string> chunks;
    for (int start = 0; start < static_cast<int>(words.size()); start += step) {
        const int end = std::min(start + chunkWords, static_cast<int>(words.size()));
        std::string chunk;
        for (int i = start; i < end; ++i) {
            if (i > start) chunk += ' ';
            chunk += words[i];
        }
        chunks.push_back(std::move(chunk));
        if (end == static_cast<int>(words.size())) break;
    }
    return chunks;
}

// Client for Ollama's local REST API. Embeddings use the current /api/embed
// request/response shape; generation uses /api/generate.
class OllamaClient {
    std::string host;
    int port;

    static std::vector<float> parseEmbedding(const std::string& body) {
        size_t pos = body.find("\"embeddings\"");
        if (pos == std::string::npos) return {};
        pos = body.find('[', pos);
        if (pos == std::string::npos) return {};
        pos = body.find('[', pos + 1);
        if (pos == std::string::npos) return {};
        const size_t end = body.find(']', pos + 1);
        if (end == std::string::npos) return {};
        return parseVec(body.substr(pos + 1, end - pos - 1));
    }

public:
    std::string embedModel = "nomic-embed-text";
    std::string genModel = "llama3.2";

    explicit OllamaClient(const std::string& h = "127.0.0.1", int p = 11434)
        : host(h), port(p) {}

    bool isAvailable() const {
        httplib::Client client(host, port);
        client.set_connection_timeout(2, 0);
        const auto response = client.Get("/api/tags");
        return response && response->status == 200;
    }

    std::vector<float> embed(const std::string& text) const {
        httplib::Client client(host, port);
        client.set_connection_timeout(3, 0);
        client.set_read_timeout(60, 0);
        const std::string body = "{\"model\":" + jS(embedModel)
            + ",\"input\":" + jS(text) + ",\"truncate\":true}";
        const auto response = client.Post("/api/embed", body, "application/json");
        if (!response || response->status != 200) return {};
        return parseEmbedding(response->body);
    }

    std::string generate(const std::string& prompt) const {
        httplib::Client client(host, port);
        client.set_connection_timeout(3, 0);
        client.set_read_timeout(180, 0);
        const std::string body = "{\"model\":" + jS(genModel)
            + ",\"prompt\":" + jS(prompt) + ",\"stream\":false}";
        const auto response = client.Post("/api/generate", body, "application/json");
        if (!response || response->status != 200)
            return "ERROR: Ollama unavailable. Run: ollama serve";
        return extractStr(response->body, "response");
    }
};

struct DocItem {
    int id;
    std::string title;
    std::string text;
    std::vector<float> emb;
};

// Document chunks use the same HNSW and Brute Force search implementations
// as the demo vectors, while keeping their real embedding dimensions.
class DocumentDB {
    std::unordered_map<int, DocItem> store;
    HNSW hnsw;
    BruteForce bruteForce;
    mutable std::mutex mutex;
    int nextId = 1;
    int dims = 0;

public:
    DocumentDB() : hnsw(16, 200) {}

    int insert(const std::string& title, const std::string& text,
               const std::vector<float>& embedding) {
        if (embedding.empty()) return -1;
        std::lock_guard<std::mutex> lock(mutex);
        if (dims != 0 && static_cast<int>(embedding.size()) != dims) return -1;
        if (dims == 0) dims = static_cast<int>(embedding.size());
        const int id = nextId++;
        DocItem item{id, title, text, embedding};
        store.emplace(id, item);
        VectorItem vector{id, title, "doc", embedding};
        hnsw.insert(vector, cosine);
        bruteForce.insert(vector);
        return id;
    }

    std::vector<std::pair<float, DocItem>> search(
        const std::vector<float>& query, int k, float maxDistance = 0.7f) {
        std::lock_guard<std::mutex> lock(mutex);
        if (store.empty() || query.empty() || static_cast<int>(query.size()) != dims || k <= 0)
            return {};
        const auto raw = store.size() < 10
            ? bruteForce.knn(query, k, cosine)
            : hnsw.knn(query, k, 50, cosine);
        std::vector<std::pair<float, DocItem>> hits;
        for (const auto& [distance, id] : raw) {
            const auto found = store.find(id);
            if (found != store.end() && distance <= maxDistance)
                hits.emplace_back(distance, found->second);
        }
        return hits;
    }

    bool remove(int id) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!store.erase(id)) return false;
        hnsw.remove(id);
        bruteForce.remove(id);
        return true;
    }

    std::vector<DocItem> all() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<DocItem> items;
        items.reserve(store.size());
        for (const auto& [id, item] : store) items.push_back(item);
        std::sort(items.begin(), items.end(), [](const DocItem& a, const DocItem& b) {
            return a.id < b.id;
        });
        return items;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return store.size();
    }

    int getDims() const {
        std::lock_guard<std::mutex> lock(mutex);
        return dims;
    }
};
