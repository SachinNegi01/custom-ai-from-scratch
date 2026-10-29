#include "httplib.h"
#include "src/vector_engine.hpp"
#include "src/rag_engine.hpp"

void cors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

// Seed vectors across the four 4D category regions in the 16D demo space.
void loadDemo(VectorDB& db) {
    const auto dist = getDistFn("cosine");
    db.insert("Linked List: nodes connected by pointers", "cs", {0.90f,0.85f,0.72f,0.68f,0.12f,0.08f,0.15f,0.10f,0.05f,0.08f,0.06f,0.09f,0.07f,0.11f,0.08f,0.06f}, dist);
    db.insert("Binary Search Tree: O(log n) search and insert", "cs", {0.88f,0.82f,0.78f,0.74f,0.15f,0.10f,0.08f,0.12f,0.06f,0.07f,0.08f,0.05f,0.09f,0.06f,0.07f,0.10f}, dist);
    db.insert("Dynamic Programming: memoization overlapping subproblems", "cs", {0.82f,0.76f,0.88f,0.80f,0.20f,0.18f,0.12f,0.09f,0.07f,0.06f,0.08f,0.07f,0.08f,0.09f,0.06f,0.07f}, dist);
    db.insert("Graph BFS and DFS: breadth and depth first traversal", "cs", {0.85f,0.80f,0.75f,0.82f,0.18f,0.14f,0.10f,0.08f,0.06f,0.09f,0.07f,0.06f,0.10f,0.08f,0.09f,0.07f}, dist);
    db.insert("Hash Table: O(1) lookup with collision chaining", "cs", {0.87f,0.78f,0.70f,0.76f,0.13f,0.11f,0.09f,0.14f,0.08f,0.07f,0.06f,0.08f,0.07f,0.10f,0.08f,0.09f}, dist);
    db.insert("Calculus: derivatives integrals and limits", "math", {0.12f,0.15f,0.18f,0.10f,0.91f,0.86f,0.78f,0.72f,0.08f,0.06f,0.07f,0.09f,0.07f,0.08f,0.06f,0.10f}, dist);
    db.insert("Linear Algebra: matrices eigenvalues eigenvectors", "math", {0.20f,0.18f,0.15f,0.12f,0.88f,0.90f,0.82f,0.76f,0.09f,0.07f,0.08f,0.06f,0.10f,0.07f,0.08f,0.09f}, dist);
    db.insert("Probability: distributions random variables Bayes theorem", "math", {0.15f,0.12f,0.20f,0.18f,0.84f,0.80f,0.88f,0.82f,0.07f,0.08f,0.06f,0.10f,0.09f,0.06f,0.09f,0.08f}, dist);
    db.insert("Number Theory: primes modular arithmetic RSA cryptography", "math", {0.22f,0.16f,0.14f,0.20f,0.80f,0.85f,0.76f,0.90f,0.08f,0.09f,0.07f,0.06f,0.08f,0.10f,0.07f,0.06f}, dist);
    db.insert("Combinatorics: permutations combinations generating functions", "math", {0.18f,0.20f,0.16f,0.14f,0.86f,0.78f,0.84f,0.80f,0.06f,0.07f,0.09f,0.08f,0.06f,0.09f,0.10f,0.07f}, dist);
    db.insert("Neapolitan Pizza: wood-fired dough San Marzano tomatoes", "food", {0.08f,0.06f,0.09f,0.07f,0.07f,0.08f,0.06f,0.09f,0.90f,0.86f,0.78f,0.72f,0.08f,0.06f,0.09f,0.07f}, dist);
    db.insert("Sushi: vinegared rice raw fish and nori rolls", "food", {0.06f,0.08f,0.07f,0.09f,0.09f,0.06f,0.08f,0.07f,0.86f,0.90f,0.82f,0.76f,0.07f,0.09f,0.06f,0.08f}, dist);
    db.insert("Ramen: noodle soup with chashu pork and soft-boiled eggs", "food", {0.09f,0.07f,0.06f,0.08f,0.08f,0.09f,0.07f,0.06f,0.82f,0.78f,0.90f,0.84f,0.09f,0.07f,0.08f,0.06f}, dist);
    db.insert("Tacos: corn tortillas with carnitas salsa and cilantro", "food", {0.07f,0.09f,0.08f,0.06f,0.06f,0.07f,0.09f,0.08f,0.78f,0.82f,0.86f,0.90f,0.06f,0.08f,0.07f,0.09f}, dist);
    db.insert("Croissant: laminated pastry with buttery flaky layers", "food", {0.06f,0.07f,0.10f,0.09f,0.10f,0.06f,0.07f,0.10f,0.85f,0.80f,0.76f,0.82f,0.09f,0.07f,0.10f,0.06f}, dist);
    db.insert("Basketball: fast-paced shooting dribbling slam dunks", "sports", {0.09f,0.07f,0.08f,0.10f,0.08f,0.09f,0.07f,0.06f,0.08f,0.07f,0.09f,0.06f,0.91f,0.85f,0.78f,0.72f}, dist);
    db.insert("Football: tackles touchdowns field goals and strategy", "sports", {0.07f,0.09f,0.06f,0.08f,0.09f,0.07f,0.10f,0.08f,0.07f,0.09f,0.08f,0.07f,0.87f,0.89f,0.82f,0.76f}, dist);
    db.insert("Tennis: racket volleys groundstrokes and Wimbledon serves", "sports", {0.08f,0.06f,0.09f,0.07f,0.07f,0.08f,0.06f,0.09f,0.09f,0.06f,0.07f,0.08f,0.83f,0.80f,0.88f,0.82f}, dist);
    db.insert("Chess: openings endgames tactics strategic board game", "sports", {0.25f,0.20f,0.22f,0.18f,0.22f,0.18f,0.20f,0.15f,0.06f,0.08f,0.07f,0.09f,0.80f,0.84f,0.78f,0.90f}, dist);
    db.insert("Swimming: butterfly freestyle backstroke Olympic competition", "sports", {0.06f,0.08f,0.07f,0.09f,0.08f,0.06f,0.09f,0.07f,0.10f,0.08f,0.06f,0.07f,0.85f,0.82f,0.86f,0.80f}, dist);
}

int main() {
    VectorDB db(DIMS);
    DocumentDB docDB;
    OllamaClient ollama;
    loadDemo(db);

    httplib::Server server;
    server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        cors(res);
        res.status = 204;
    });

    server.Get("/search", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        const auto query = parseVec(req.get_param_value("v"));
        if (static_cast<int>(query.size()) != DIMS) {
            res.status = 400;
            res.set_content("{\"error\":\"need " + std::to_string(DIMS) + "D vector\"}", "application/json");
            return;
        }
        int k = 5;
        try { if (req.has_param("k")) k = std::stoi(req.get_param_value("k")); } catch (...) {}
        k = std::max(k, 1);
        auto metric = req.get_param_value("metric");
        if (metric.empty()) metric = "cosine";
        auto algo = req.get_param_value("algo");
        if (algo.empty()) algo = "hnsw";
        if (algo != "hnsw" && algo != "kdtree" && algo != "bruteforce") {
            res.status = 400;
            res.set_content("{\"error\":\"unknown algorithm\"}", "application/json");
            return;
        }

        const auto out = db.search(query, k, metric, algo);
        std::ostringstream json;
        json << "{\"results\":[";
        for (size_t i = 0; i < out.hits.size(); ++i) {
            if (i) json << ',';
            const auto& hit = out.hits[i];
            json << "{\"id\":" << hit.id << ",\"metadata\":" << jS(hit.meta)
                 << ",\"category\":" << jS(hit.cat) << ",\"distance\":"
                 << std::fixed << std::setprecision(6) << hit.dist
                 << ",\"embedding\":" << jVec(hit.emb) << '}';
        }
        json << "],\"latencyUs\":" << out.us << ",\"algo\":" << jS(out.algo)
             << ",\"metric\":" << jS(out.metric) << '}';
        res.set_content(json.str(), "application/json");
    });

    server.Post("/insert", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        std::string metadata, category;
        std::vector<float> embedding;
        if (!parseBody(req.body, metadata, category, embedding) || static_cast<int>(embedding.size()) != DIMS) {
            res.status = 400;
            res.set_content("{\"error\":\"invalid body; expected metadata, category, and a 16D embedding\"}", "application/json");
            return;
        }
        const int id = db.insert(metadata, category, embedding, getDistFn("cosine"));
        res.set_content("{\"id\":" + std::to_string(id) + "}", "application/json");
    });

    server.Delete(R"(/delete/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        const int id = std::stoi(req.matches[1]);
        res.set_content("{\"ok\":" + std::string(db.remove(id) ? "true" : "false") + "}", "application/json");
    });

    server.Get("/items", [&](const httplib::Request&, httplib::Response& res) {
        cors(res);
        const auto items = db.all();
        std::ostringstream json;
        json << '[';
        for (size_t i = 0; i < items.size(); ++i) {
            if (i) json << ',';
            const auto& item = items[i];
            json << "{\"id\":" << item.id << ",\"metadata\":" << jS(item.metadata)
                 << ",\"category\":" << jS(item.category) << ",\"embedding\":" << jVec(item.emb) << '}';
        }
        json << ']';
        res.set_content(json.str(), "application/json");
    });

    server.Get("/benchmark", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        const auto query = parseVec(req.get_param_value("v"));
        if (static_cast<int>(query.size()) != DIMS) {
            res.status = 400;
            res.set_content("{\"error\":\"need " + std::to_string(DIMS) + "D vector\"}", "application/json");
            return;
        }
        int k = 5;
        try { if (req.has_param("k")) k = std::stoi(req.get_param_value("k")); } catch (...) {}
        k = std::max(k, 1);
        auto metric = req.get_param_value("metric");
        if (metric.empty()) metric = "cosine";
        const auto result = db.benchmark(query, k, metric);
        res.set_content("{\"bruteforceUs\":" + std::to_string(result.bfUs)
            + ",\"kdtreeUs\":" + std::to_string(result.kdUs)
            + ",\"hnswUs\":" + std::to_string(result.hnswUs)
            + ",\"itemCount\":" + std::to_string(result.n) + '}', "application/json");
    });

    server.Get("/hnsw-info", [&](const httplib::Request&, httplib::Response& res) {
        cors(res);
        const auto info = db.hnswInfo();
        std::ostringstream json;
        json << "{\"topLayer\":" << info.topLayer << ",\"nodeCount\":" << info.nodeCount << ",\"nodesPerLayer\":[";
        for (size_t i = 0; i < info.nodesPerLayer.size(); ++i) { if (i) json << ','; json << info.nodesPerLayer[i]; }
        json << "],\"edgesPerLayer\":[";
        for (size_t i = 0; i < info.edgesPerLayer.size(); ++i) { if (i) json << ','; json << info.edgesPerLayer[i]; }
        json << "],\"nodes\":[";
        for (size_t i = 0; i < info.nodes.size(); ++i) {
            if (i) json << ',';
            const auto& node = info.nodes[i];
            json << "{\"id\":" << node.id << ",\"metadata\":" << jS(node.metadata)
                 << ",\"category\":" << jS(node.category) << ",\"maxLyr\":" << node.maxLyr << '}';
        }
        json << "],\"edges\":[";
        for (size_t i = 0; i < info.edges.size(); ++i) {
            if (i) json << ',';
            const auto& edge = info.edges[i];
            json << "{\"src\":" << edge.src << ",\"dst\":" << edge.dst << ",\"lyr\":" << edge.lyr << '}';
        }
        json << "]}";
        res.set_content(json.str(), "application/json");
    });

    server.Get("/stats", [&](const httplib::Request&, httplib::Response& res) {
        cors(res);
        res.set_content("{\"count\":" + std::to_string(db.size())
            + ",\"dims\":" + std::to_string(DIMS)
            + ",\"algorithms\":[\"bruteforce\",\"kdtree\",\"hnsw\"]"
            + ",\"metrics\":[\"euclidean\",\"cosine\",\"manhattan\"]}", "application/json");
    });

    server.Post("/doc/insert", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        const auto title = extractStr(req.body, "title");
        const auto text = extractStr(req.body, "text");
        if (title.empty() || text.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"need title and text\"}", "application/json");
            return;
        }
        const auto chunks = chunkText(text);
        if (chunks.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"document contains no words\"}", "application/json");
            return;
        }

        std::vector<int> ids;
        for (size_t i = 0; i < chunks.size(); ++i) {
            const auto embedding = ollama.embed(chunks[i]);
            if (embedding.empty()) {
                res.status = 503;
                res.set_content("{\"error\":\"Could not create an embedding. Check Ollama and the configured embedding model.\"}", "application/json");
                return;
            }
            const auto chunkTitle = chunks.size() > 1
                ? title + " [" + std::to_string(i + 1) + "/" + std::to_string(chunks.size()) + "]"
                : title;
            const int id = docDB.insert(chunkTitle, chunks[i], embedding);
            if (id < 0) {
                res.status = 409;
                res.set_content("{\"error\":\"Embedding dimensions changed; all document chunks must use the same model and dimensions.\"}", "application/json");
                return;
            }
            ids.push_back(id);
        }
        std::ostringstream json;
        json << "{\"ids\":[";
        for (size_t i = 0; i < ids.size(); ++i) { if (i) json << ','; json << ids[i]; }
        json << "],\"chunks\":" << chunks.size() << ",\"dims\":" << docDB.getDims() << '}';
        res.set_content(json.str(), "application/json");
    });

    server.Delete(R"(/doc/delete/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        const int id = std::stoi(req.matches[1]);
        res.set_content("{\"ok\":" + std::string(docDB.remove(id) ? "true" : "false") + "}", "application/json");
    });

    server.Get("/doc/list", [&](const httplib::Request&, httplib::Response& res) {
        cors(res);
        const auto docs = docDB.all();
        std::ostringstream json;
        json << '[';
        for (size_t i = 0; i < docs.size(); ++i) {
            if (i) json << ',';
            std::string preview = docs[i].text.substr(0, 120);
            if (docs[i].text.size() > 120) preview += "…";
            std::istringstream words(docs[i].text);
            int wordCount = 0;
            std::string word;
            while (words >> word) ++wordCount;
            json << "{\"id\":" << docs[i].id << ",\"title\":" << jS(docs[i].title)
                 << ",\"preview\":" << jS(preview) << ",\"words\":" << wordCount << '}';
        }
        json << ']';
        res.set_content(json.str(), "application/json");
    });

    server.Post("/doc/search", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        const auto question = extractStr(req.body, "question");
        const int k = std::max(1, std::min(extractInt(req.body, "k", 3), 20));
        if (question.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"need question\"}", "application/json");
            return;
        }
        const auto query = ollama.embed(question);
        if (query.empty()) {
            res.status = 503;
            res.set_content("{\"error\":\"Ollama embedding request failed\"}", "application/json");
            return;
        }
        const auto hits = docDB.search(query, k);
        std::ostringstream json;
        json << "{\"contexts\":[";
        for (size_t i = 0; i < hits.size(); ++i) {
            if (i) json << ',';
            json << "{\"id\":" << hits[i].second.id << ",\"title\":" << jS(hits[i].second.title)
                 << ",\"distance\":" << std::fixed << std::setprecision(4) << hits[i].first << '}';
        }
        json << "]}";
        res.set_content(json.str(), "application/json");
    });

    server.Post("/doc/ask", [&](const httplib::Request& req, httplib::Response& res) {
        cors(res);
        const auto question = extractStr(req.body, "question");
        const int k = std::max(1, std::min(extractInt(req.body, "k", 3), 20));
        if (question.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"need question\"}", "application/json");
            return;
        }
        const auto query = ollama.embed(question);
        if (query.empty()) {
            res.status = 503;
            res.set_content("{\"error\":\"Ollama embedding request failed\"}", "application/json");
            return;
        }
        const auto hits = docDB.search(query, k);
        std::ostringstream context;
        for (size_t i = 0; i < hits.size(); ++i)
            context << '[' << (i + 1) << "] " << hits[i].second.title << ":\n"
                    << hits[i].second.text << "\n\n";
        const std::string prompt =
            "You are a helpful assistant. Answer the user's question directly. "
            "Use the provided context if it contains relevant information. "
            "If it does not, use your general knowledge. Do not mention the context.\n\n"
            "Context:\n" + context.str() + "Question: " + question + "\n\nAnswer:";
        const auto answer = ollama.generate(prompt);
        if (answer.rfind("ERROR:", 0) == 0) {
            res.status = 503;
            res.set_content("{\"error\":" + jS(answer) + '}', "application/json");
            return;
        }
        std::ostringstream json;
        json << "{\"answer\":" << jS(answer) << ",\"model\":" << jS(ollama.genModel) << ",\"contexts\":[";
        for (size_t i = 0; i < hits.size(); ++i) {
            if (i) json << ',';
            json << "{\"id\":" << hits[i].second.id << ",\"title\":" << jS(hits[i].second.title)
                 << ",\"text\":" << jS(hits[i].second.text)
                 << ",\"distance\":" << std::fixed << std::setprecision(4) << hits[i].first << '}';
        }
        json << "],\"docCount\":" << docDB.size() << '}';
        res.set_content(json.str(), "application/json");
    });

    server.Get("/status", [&](const httplib::Request&, httplib::Response& res) {
        cors(res);
        const bool ollamaUp = ollama.isAvailable();
        res.set_content("{\"ollamaAvailable\":" + std::string(ollamaUp ? "true" : "false")
            + ",\"embedModel\":" + jS(ollama.embedModel)
            + ",\"genModel\":" + jS(ollama.genModel)
            + ",\"docCount\":" + std::to_string(docDB.size())
            + ",\"docDims\":" + std::to_string(docDB.getDims())
            + ",\"demoDims\":" + std::to_string(DIMS)
            + ",\"demoCount\":" + std::to_string(db.size()) + '}', "application/json");
    });

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
#ifdef CUSTOM_AI_SOURCE_DIR
        std::ifstream file(std::string(CUSTOM_AI_SOURCE_DIR) + "/index.html");
#else
        std::ifstream file("index.html");
#endif
        if (!file.is_open()) {
            res.status = 404;
            res.set_content("Frontend file index.html was not found.", "text/plain");
            return;
        }
        res.set_content(std::string(std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>()), "text/html");
    });

    std::cout << "Vector search API listening at http://localhost:8080\n"
              << db.size() << " demo vectors | " << DIMS << " dimensions\n";
    server.listen("0.0.0.0", 8080);
}
