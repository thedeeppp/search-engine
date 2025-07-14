// tfidf_engine.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

struct Document {
    std::string path;
    std::unordered_map<std::string, int> termFreq;
    int totalTerms = 0;
};

std::vector<Document> documents;
std::unordered_map<std::string, int> documentFreq;

// Tokenizes lowercase words only
std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string word;
    for (char c : text) {
        if (std::isalnum(c)) {
            word += std::tolower(c);
        } else if (!word.empty()) {
            tokens.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) tokens.push_back(word);
    return tokens;
}

void index_document(const std::string& path) {
    std::ifstream file(path);
    if (!file) return;

    Document doc;
    doc.path = path;
    std::unordered_set<std::string> words_in_doc;

    std::string line;
    while (getline(file, line)) {
        auto words = tokenize(line);
        for (const auto& word : words) {
            doc.termFreq[word]++;
            doc.totalTerms++;
            words_in_doc.insert(word);
        }
    }
    for (const auto& word : words_in_doc) {
        documentFreq[word]++;
    }
    documents.push_back(doc);
}

void index_directory(const std::string& dirPath) {
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            index_document(entry.path().string());
        }
    }
}

double compute_tfidf(const std::string& term, const Document& doc, int totalDocs) {
    if (doc.termFreq.find(term) == doc.termFreq.end()) return 0.0;
    double tf = (double) doc.termFreq.at(term) / doc.totalTerms;
    double idf = std::log((double) totalDocs / (1 + documentFreq[term]));
    return tf * idf;
}

void search(const std::string& query) {
    auto queryTerms = tokenize(query);
    std::vector<std::pair<std::string, double>> scoredDocs;

    for (const auto& doc : documents) {
        double score = 0.0;
        for (const auto& term : queryTerms) {
            score += compute_tfidf(term, doc, documents.size());
        }
        if (score > 0) {
            scoredDocs.push_back({doc.path, score});
        }
    }

    std::sort(scoredDocs.begin(), scoredDocs.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    std::cout << "\nTop Relevant Documents:\n";
    for (const auto& [path, score] : scoredDocs) {
        std::cout << path << " (score: " << score << ")\n";
    }
    if (scoredDocs.empty()) std::cout << "No matching documents found.\n";
}

int main() {
    std::string directory = "./test/files"; // Change to your actual folder path
    index_directory(directory);

    std::string input;
    std::cout << "Enter your search query: ";
    std::getline(std::cin, input);
    search(input);
    return 0;
}
