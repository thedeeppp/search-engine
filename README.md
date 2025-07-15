# Mini Search Engine

## TL;DR

This is a C++ multithreaded keyword-based search engine that indexes words from text files and builds an in-memory inverted index using STL containers. Each document is tokenized and stored using a Storage class, while the Engine class manages indexing and querying using multiple threads. It supports configurable input via conf.json, and results are ranked based on word occurrences. Core data structures include unordered_map, vector, and mutex. Built with CMake and designed for fast, extensible local search.

## Overview

This document explains the architecture and data structures used in this project. It also provides a quick setup guide and conceptual understanding of how the engine performs indexing, tokenization, and searching.

---

## Project Purpose

This C++ project is a multithreaded keyword-based search engine that:

* Indexes words from text-based files
* Builds a searchable in-memory inverted index
* Processes search queries and returns documents with matching keywords

---

## Core Components

### 1. Storage Class

Represents a single document and stores its indexed content.

#### Data Structures:

* `unordered_map<string, vector<int>> words;`

  * Maps each word to a vector of positions where it occurs in the file
* `string fileName;`
* `std::mutex mutex;`

This is the inverted index for a single document.

---

### 2. Engine Class

Responsible for indexing all files and performing search queries.

#### Data Structures:

* `vector<Storage*> storages;`

  * List of all indexed documents
* `vector<std::thread> threads;`

  * Handles multithreading during indexing
* `mutex fileWork;`

  * Synchronizes access to shared state

---

### 3. Jsoner Class (Inherited)

Used to parse `conf.json` and `requests.json`, and serialize results.

#### Tools:

* Uses the [nlohmann/json](https://github.com/nlohmann/json) library to parse and write JSON.

---

## Indexing Process

1. Read files listed in `conf.json`
2. Multithreading: Each file is read in parallel using `thread`
3. Tokenization: Words are extracted using space/regex delimiters
4. Indexing: Each word’s position is recorded using:

```cpp
words[word].push_back(position);
```

5. Storage: Each document’s data is stored in a `Storage` instance and pushed into `storages`

---

## Querying Process

1. Read queries from `conf.json` or `requests.json`
2. Loop through all `Storage` objects
3. For each query word:

   * Lookup in `Storage::words[word]`
   * Count matches and build a relevance score
4. Return top N results (defined in `max_responses`)

---

## Tokenization Details

The tokenizer splits text into words based on whitespace or punctuation. It supports alphanumeric parsing and lowercasing for uniformity.

```cpp
if (isalpha(char)) word += tolower(char);
```

---

## Synchronization (Multithreading)

* Threads are used for parallel file indexing
* Access to shared data like the list of files and `storages` is synchronized with `std::mutex`

---

## Setup Instructions (WSL/Linux)

### Clone and Build

```bash
git clone https://github.com/thedeeppp/search-engine.git
cd search-engine
cmake -S . -B build
camke --build build 
```


### Run the Search Engine

```bash
cd build
./SearchEngine
```

## Summary of Key Data Structures

| Purpose           | Structure Used                                  |
| ----------------- | ----------------------------------------------- |
| Inverted Index    | `unordered_map<string, vector<int>>`            |
| All Documents     | `vector<Storage*>`                              |
| Thread Management | `vector<thread>`, `mutex`                       |
| Token/Query Words | `string`, `vector<string>`                      |
| Result Storage    | `vector<pair<string, int>>` (file, match count) |

---
