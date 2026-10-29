# Custom AI From Scratch

## Introduction

Custom AI From Scratch is an educational project for exploring how the pieces of an AI application fit together. It builds a small vector search engine and connects it to a local language model so users can see how text is embedded, searched, and used to answer questions.

The project does not train a foundation model. Instead, it implements vector search concepts in C++ and uses Ollama models for document embeddings and text generation. A browser interface makes the search algorithms, results, and document retrieval visible.

## Tech Stack

| # | Technology | Role in the project |
|---:|---|---|
| **01** | ![C++17 logo badge](https://img.shields.io/badge/C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white) | Implements the vector database, search algorithms, and HTTP API. |
| **02** | ![cpp-httplib badge with C++ logo](https://img.shields.io/badge/cpp--httplib-394B59?style=for-the-badge&logo=cplusplus&logoColor=white) | Provides the single-header HTTP server and client. cpp-httplib has no separate brand logo; the badge uses its C++ ecosystem logo. |
| **03** | ![HTML5 logo badge](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white) ![CSS3 logo badge](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white) ![JavaScript logo badge](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black) | Powers the browser interface, search controls, and vector-space visualization. |
| **04** | ![CMake logo badge](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white) | Configures and builds the C++ application. |
| **05** | ![Ollama logo badge](https://img.shields.io/badge/Ollama-111111?style=for-the-badge&logo=ollama&logoColor=white) | Runs local embeddings with `nomic-embed-text` and answer generation with `llama3.2`. |

### Run locally on Windows

Install CMake 3.16 or newer and a C++17 compiler. From PowerShell in the project directory, configure and build:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Run the resulting executable. Use the command that matches your CMake generator: Visual Studio generators place it in `build\Release`, while single-configuration generators such as MinGW generally place it in `build`.

```powershell
.\build\Release\custom_ai.exe
```

```powershell
.\build\custom_ai.exe
```

The server listens on port 8080. Open [http://localhost:8080](http://localhost:8080) in a browser and leave the server running while you use the interface.

Vector search works without Ollama. To enable document embedding and Ask AI, install and start Ollama, then download the configured models:

```powershell
ollama pull nomic-embed-text
ollama pull llama3.2
```

More build details are in [BUILDING.md](BUILDING.md).

## Project Structure

```text
custom-ai-from-scratch/
├── main.cpp                 # HTTP server, demo data, and API routes
├── index.html               # Search, visualization, document, and Ask AI interface
├── httplib.h                # Bundled cpp-httplib dependency
├── CMakeLists.txt           # C++17 build configuration
├── BUILDING.md              # Build and run guide
└── src/
    ├── vector_engine.hpp   # Distance metrics, indexes, and vector database
    └── rag_engine.hpp      # Text chunking, Ollama client, and document database
```

## Architecture

The application has two related search paths. Demo vectors use hand-authored 16-dimensional values and a lightweight keyword-based text-to-vector function in the browser. Documents use real embeddings produced by Ollama; their dimension is taken from the returned embedding.

```text
Demo query
   → browser creates a 16D demo vector
   → /search API
   → Brute Force, KD-Tree, or HNSW in VectorDB
   → ranked matches, latency, and 2D PCA visualization

Document text
   → /doc/insert
   → overlapping word chunks
   → Ollama /api/embed
   → DocumentDB indexes each chunk with HNSW and keeps a Brute Force fallback for small collections

Question
   → Ollama embeds the question
   → DocumentDB retrieves the closest chunks
   → Ollama /api/generate answers with the retrieved text
   → answer and source chunks return to the interface
```

The demo vector database supports **cosine**, **Euclidean**, and **Manhattan** distance. Brute Force provides a direct baseline, KD-Tree partitions the vector space, and HNSW searches a layered neighbor graph. The interface can compare their measured search times and display HNSW layer counts.

The HTTP API includes:

- `GET /search`, `GET /items`, `POST /insert`, and `DELETE /delete/{id}` for demo vectors.
- `GET /benchmark` and `GET /hnsw-info` for algorithm timing and graph details.
- `POST /doc/insert`, `GET /doc/list`, and `DELETE /doc/delete/{id}` for document chunks.
- `POST /doc/search` and `POST /doc/ask` for retrieval and retrieval-augmented answers.
- `GET /status` for Ollama and database status.

## Results

The project provides a browser-based lab for searching the seeded 16D examples, changing the algorithm and distance metric, inserting and deleting vectors, reviewing search latency, comparing algorithms, and viewing a PCA projection of the vectors.

When Ollama is available, users can add text documents, inspect their chunks, ask questions, and expand the retrieved passages used to generate an answer. Document matches are also highlighted in the vector-space display through companion demo vectors.

Vector and document records currently live in memory. They are cleared when the server stops. Search timings depend on the machine and current dataset; the small demo collection is intended for learning and comparison, not as a production performance benchmark.

## Future Scope

- Persist vectors and documents between server runs.
- Replace the lightweight JSON field helpers with a full JSON library.
- Add automated tests for distance metrics, index behavior, API validation, and document retrieval.
- Make model names, chunk size, overlap, and retrieval limits configurable.
- Evaluate retrieval quality and index performance with larger datasets.
- Add document import from common file formats and richer source tracking.
