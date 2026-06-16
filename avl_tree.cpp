// ============================================================
//  avl_tree.cpp — DestinationDatabase (AVL Tree) implementation
// ============================================================
#include "../include/avl_tree.h"
#include <iostream>
#include <iomanip>

namespace Tourism {

// -------------------------------------------------------
//  DestinationRecord
// -------------------------------------------------------
void DestinationRecord::print() const {
    std::cout << "  ┌─────────────────────────────────────\n";
    std::cout << "  │  ID    : " << id << "\n";
    std::cout << "  │  Name  : " << name << "\n";
    std::cout << "  │  City  : " << city << "\n";
    std::cout << "  │  Price : $" << std::fixed
              << std::setprecision(2) << price << "\n";
    std::cout << "  └─────────────────────────────────────\n";
}

// -------------------------------------------------------
//  AVL Utility helpers
// -------------------------------------------------------
int DestinationDatabase::height(AVLNode* n) const {
    return n ? n->height : 0;
}

int DestinationDatabase::balanceFactor(AVLNode* n) const {
    return n ? height(n->left) - height(n->right) : 0;
}

static void updateHeight(AVLNode* n) {
    if (n)
        n->height = 1 + std::max(
            n->left  ? n->left->height  : 0,
            n->right ? n->right->height : 0);
}

// -------------------------------------------------------
//  Rotations
// -------------------------------------------------------

//       y                    x
//      / \     Right        / \
//     x   T3  ──────>     T1   y
//    / \                       / \
//   T1  T2                   T2  T3
AVLNode* DestinationDatabase::rotateRight(AVLNode* y) {
    AVLNode* x  = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left  = T2;

    updateHeight(y);
    updateHeight(x);
    return x;
}

//     x                       y
//    / \      Left           / \
//   T1   y   ──────>        x   T3
//       / \                / \
//      T2  T3            T1  T2
AVLNode* DestinationDatabase::rotateLeft(AVLNode* x) {
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;

    y->left  = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);
    return y;
}

// Balance a node after insertion / deletion
AVLNode* DestinationDatabase::balance(AVLNode* n) {
    updateHeight(n);
    int bf = balanceFactor(n);

    // Left-Heavy
    if (bf > 1) {
        if (balanceFactor(n->left) < 0)          // Left-Right case
            n->left = rotateLeft(n->left);
        return rotateRight(n);                    // Left-Left case
    }

    // Right-Heavy
    if (bf < -1) {
        if (balanceFactor(n->right) > 0)          // Right-Left case
            n->right = rotateRight(n->right);
        return rotateLeft(n);                     // Right-Right case
    }

    return n;   // already balanced
}

// -------------------------------------------------------
//  Insert
// -------------------------------------------------------
AVLNode* DestinationDatabase::insert(AVLNode* node,
                                      const DestinationRecord& rec) {
    if (!node) return new AVLNode{ rec };

    if (rec.id < node->data.id)
        node->left  = insert(node->left,  rec);
    else if (rec.id > node->data.id)
        node->right = insert(node->right, rec);
    else {
        // Duplicate ID → update record
        node->data = rec;
        return node;
    }

    return balance(node);
}

void DestinationDatabase::insert(int id, const std::string& name,
                                  const std::string& city, double price) {
    DestinationRecord rec{ id, name, city, price };
    root_ = insert(root_, rec);
    std::cout << "[AVL] Inserted: [" << id << "] " << name
              << " — $" << std::fixed << std::setprecision(2) << price << "\n";
}

// -------------------------------------------------------
//  Min-value node (used in deletion)
// -------------------------------------------------------
AVLNode* DestinationDatabase::minNode(AVLNode* node) const {
    AVLNode* curr = node;
    while (curr->left) curr = curr->left;
    return curr;
}

// -------------------------------------------------------
//  Remove
// -------------------------------------------------------
AVLNode* DestinationDatabase::remove(AVLNode* node, int id, bool& ok) {
    if (!node) { ok = false; return nullptr; }

    if (id < node->data.id)
        node->left  = remove(node->left,  id, ok);
    else if (id > node->data.id)
        node->right = remove(node->right, id, ok);
    else {
        ok = true;
        // Node with only one child or no child
        if (!node->left || !node->right) {
            AVLNode* tmp = node->left ? node->left : node->right;
            delete node;
            return tmp;
        }
        // Node with two children: get in-order successor (smallest in right)
        AVLNode* successor = minNode(node->right);
        node->data  = successor->data;
        bool dummy  = false;
        node->right = remove(node->right, successor->data.id, dummy);
    }

    return balance(node);
}

bool DestinationDatabase::remove(int id) {
    bool ok = false;
    root_ = remove(root_, id, ok);
    if (ok)
        std::cout << "[AVL] Removed destination with ID: " << id << "\n";
    else
        std::cout << "[AVL] ID " << id << " not found for removal.\n";
    return ok;
}

// -------------------------------------------------------
//  Search
// -------------------------------------------------------
DestinationRecord* DestinationDatabase::search(AVLNode* node, int id) const {
    if (!node) return nullptr;
    if (id == node->data.id) return &node->data;
    if (id <  node->data.id) return search(node->left,  id);
    return search(node->right, id);
}

DestinationRecord* DestinationDatabase::search(int id) const {
    std::cout << "[AVL] Searching for ID: " << id << " ... ";
    auto* result = search(root_, id);
    if (result) std::cout << "Found!\n";
    else        std::cout << "Not found.\n";
    return result;
}

// -------------------------------------------------------
//  In-order traversal (sorted by ID)
// -------------------------------------------------------
void DestinationDatabase::inorder(AVLNode* node,
                                   std::vector<DestinationRecord>& out) const {
    if (!node) return;
    inorder(node->left,  out);
    out.push_back(node->data);
    inorder(node->right, out);
}

std::vector<DestinationRecord> DestinationDatabase::inorderList() const {
    std::vector<DestinationRecord> result;
    inorder(root_, result);
    return result;
}

void DestinationDatabase::printInorder() const {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║    Destination Database (In-order)   ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "  " << std::setw(6) << "ID"
              << "  " << std::setw(28) << std::left << "Name"
              << "  " << std::setw(20) << "City"
              << "  " << "Price\n";
    std::cout << "  ──────────────────────────────────────────────────────\n";

    auto list = inorderList();
    for (const auto& rec : list) {
        std::cout << "  " << std::setw(6) << std::right << rec.id
                  << "  " << std::setw(28) << std::left << rec.name
                  << "  " << std::setw(20) << rec.city
                  << "  $" << std::fixed << std::setprecision(2)
                  << rec.price << "\n";
    }
    std::cout << std::right << "\n";
}

// -------------------------------------------------------
//  Range search [lo, hi] by ID
// -------------------------------------------------------
void DestinationDatabase::rangeSearch(AVLNode* node, int lo, int hi,
                                       std::vector<DestinationRecord>& out) const {
    if (!node) return;
    if (node->data.id > lo)  rangeSearch(node->left,  lo, hi, out);
    if (node->data.id >= lo && node->data.id <= hi) out.push_back(node->data);
    if (node->data.id < hi)  rangeSearch(node->right, lo, hi, out);
}

std::vector<DestinationRecord>
DestinationDatabase::rangeSearch(int lo, int hi) const {
    std::vector<DestinationRecord> result;
    rangeSearch(root_, lo, hi, result);
    return result;
}

// -------------------------------------------------------
//  ASCII tree visualizer
// -------------------------------------------------------
void DestinationDatabase::printTreeHelper(AVLNode* node,
                                           const std::string& prefix,
                                           bool isLeft) const {
    if (!node) return;
    std::cout << prefix;
    std::cout << (isLeft ? "├── " : "└── ");
    std::cout << "[" << node->data.id << "] " << node->data.name
              << " (h=" << node->height << ")\n";
    printTreeHelper(node->left,  prefix + (isLeft ? "│   " : "    "), true);
    printTreeHelper(node->right, prefix + (isLeft ? "│   " : "    "), false);
}

void DestinationDatabase::printTree() const {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║      AVL Tree Structure              ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    if (!root_) { std::cout << "  (empty)\n\n"; return; }
    printTreeHelper(root_, "", false);
    std::cout << "\n";
}

// -------------------------------------------------------
//  Destructor helper
// -------------------------------------------------------
void DestinationDatabase::destroyTree(AVLNode* node) {
    if (!node) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

} // namespace Tourism