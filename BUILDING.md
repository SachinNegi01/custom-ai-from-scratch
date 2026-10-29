# Build and run

## Requirements

- CMake 3.16 or newer
- A C++17 compiler
- Ollama for document embeddings and local answer generation

The vector search demo works without Ollama. The document and Ask AI features need Ollama running locally with the configured models:

```powershell
ollama pull nomic-embed-text
ollama pull llama3.2
```

## Build on Windows

From the project folder:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

For a Visual Studio generator, run the executable with:

```powershell
.\build\Release\custom_ai.exe
```

For a single-configuration generator such as MinGW Makefiles, run:

```powershell
.\build\custom_ai.exe
```

The server listens on port 8080. Open [http://localhost:8080](http://localhost:8080) in a browser. Keep the server running while using the application.

## Data lifetime

Demo vectors, inserted vectors, and document chunks are stored in memory. They are cleared when the server stops.
