// ============================================================
//  Smart Tourism System — AVL Tree Module (Header)
//  Self-balancing BST keyed on Destination ID.
//  Stores: ID, Name, City, Price
// ============================================================
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace Tourism {

// -------------------------------------------------------
//  DestinationRecord — payload stored in each tree node
// -------------------------------------------------------
struct DestinationRecord {
    int         id;
    std::string name;
    std::string city;
    double      price;   // base visit/entry price (USD)

    void print() const;
};

// -------------------------------------------------------
//  AVLNode — internal node of the AVL tree
// -------------------------------------------------------
struct AVLNode {
    DestinationRecord data;
    AVLNode* left   = nullptr;
    AVLNode* right  = nullptr;
    int               height = 1;
};

// -------------------------------------------------------
//  DestinationDatabase — AVL tree operations
// -------------------------------------------------------
class DestinationDatabase {
public:
    DestinationDatabase()  = default;
    ~DestinationDatabase() { destroyTree(root_); }

    // Disable copy (owns raw pointers)
    DestinationDatabase(const DestinationDatabase&)            = delete;
    DestinationDatabase& operator=(const DestinationDatabase&) = delete;

    // ---------- Core Operations ----------
    void insert(int id, const std::string& name,
                const std::string& city, double price);
    bool remove(int id);
    DestinationRecord* search(int id) const;

    // ---------- Range / Sorted Queries ----------
    std::vector<DestinationRecord> inorderList()  const;   // sorted by ID
    std::vector<DestinationRecord> rangeSearch(int lo, int hi) const;

    // ---------- Display ----------
    void printInorder()  const;
    void printTree()     const;   // visual ASCII art
    bool isEmpty()       const { return root_ == nullptr; }

private:
    AVLNode* root_ = nullptr;

    // ---- AVL helpers ----
    int      height(AVLNode* n)                       const;
    int      balanceFactor(AVLNode* n)                const;
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* balance(AVLNode* n);

    AVLNode* insert(AVLNode* node, const DestinationRecord& rec);
    AVLNode* minNode(AVLNode* node)                   const;
    AVLNode* remove(AVLNode* node, int id, bool& ok);

    DestinationRecord* search(AVLNode* node, int id)  const;
    void inorder(AVLNode* node, std::vector<DestinationRecord>& out) const;
    void rangeSearch(AVLNode* node, int lo, int hi,
                     std::vector<DestinationRecord>& out)             const;
    void printTreeHelper(AVLNode* node, const std::string& prefix,
                         bool isLeft)                                 const;
    void destroyTree(AVLNode* node);
};

} // namespace Tourism