// ============================================================
//  main.cpp — Smart Tourism System Driver
//  Demonstrates all 4 modules with realistic tourism data.
// ============================================================
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "include/graph.h"
#include "include/avl_tree.h"
#include "include/booking_manager.h"
#include "include/search_sort.h"

// ── Utility helpers ─────────────────────────────────────────
static void printBanner(const std::string& title) {
    std::string border(60, '=');
    std::cout << "\n" << border << "\n";
    std::cout << "  " << title << "\n";
    std::cout << border << "\n";
}

static void pause() {
    std::cout << "\n  Press <Enter> to continue...\n";
    std::cin.ignore();
}

// ============================================================
//  STEP 1: Graph & Dijkstra Demo
// ============================================================
void demoGraph() {
    printBanner("STEP 1: Graph — BFS, DFS & Dijkstra's Algorithm");
    Tourism::TourismGraph graph;

    // Add famous Indian tourist destinations
    graph.addDestination(1, "Taj Mahal",         "Agra");
    graph.addDestination(2, "Jaipur City Palace","Jaipur");
    graph.addDestination(3, "Gateway of India",  "Mumbai");
    graph.addDestination(4, "Qutub Minar",       "Delhi");
    graph.addDestination(5, "Hampi Ruins",        "Karnataka");
    graph.addDestination(6, "Mysore Palace",      "Mysore");
    graph.addDestination(7, "Amber Fort",         "Jaipur");
    graph.addDestination(8, "Ellora Caves",       "Aurangabad");

    std::cout << "\n";

    // Add routes (distances in km — approximate road distances)
    graph.addRoute(1, 2, 240.0);   // Agra ↔ Jaipur
    graph.addRoute(1, 4, 200.0);   // Agra ↔ Delhi
    graph.addRoute(2, 4, 280.0);   // Jaipur ↔ Delhi
    graph.addRoute(2, 7, 12.0);    // Jaipur ↔ Amber Fort (local)
    graph.addRoute(3, 8, 340.0);   // Mumbai ↔ Ellora Caves
    graph.addRoute(4, 3, 1400.0);  // Delhi ↔ Mumbai
    graph.addRoute(5, 6, 140.0);   // Hampi ↔ Mysore
    graph.addRoute(6, 3, 1000.0);  // Mysore ↔ Mumbai
    graph.addRoute(8, 5, 580.0);   // Ellora ↔ Hampi

    std::cout << "\n";
    graph.printGraph();

    // BFS from Taj Mahal
    auto bfsResult = graph.bfs(1);
    std::cout << "\n  BFS Order IDs: ";
    for (int id : bfsResult) std::cout << id << " ";
    std::cout << "\n";

    // DFS from Taj Mahal
    auto dfsResult = graph.dfs(1);
    std::cout << "\n  DFS Order IDs: ";
    for (int id : dfsResult) std::cout << id << " ";
    std::cout << "\n";

    // Dijkstra: Taj Mahal → Mysore Palace
    graph.shortestPath(1, 6);
    // Dijkstra: Jaipur → Ellora Caves
    graph.shortestPath(2, 8);
    // Dijkstra: Delhi → Hampi (longer path)
    graph.shortestPath(4, 5);
}

// ============================================================
//  STEP 2: AVL Tree Database Demo
// ============================================================
void demoAVLTree() {
    printBanner("STEP 2: AVL Tree — Tourist Destination Database");
    Tourism::DestinationDatabase db;

    // Insert destinations
    db.insert(105, "Taj Mahal",          "Agra",        50.0);
    db.insert(210, "Eiffel Tower",        "Paris",       28.0);
    db.insert(315, "Colosseum",           "Rome",        18.0);
    db.insert(150, "Machu Picchu",        "Cusco",       47.0);
    db.insert(420, "Angkor Wat",          "Siem Reap",   37.0);
    db.insert(85,  "Sagrada Familia",     "Barcelona",   26.0);
    db.insert(260, "Burj Khalifa Top",   "Dubai",        40.0);
    db.insert(175, "Chichen Itza",        "Mexico",      27.0);
    db.insert(330, "Petra",               "Jordan",      71.0);
    db.insert(95,  "Stonehenge",          "England",     23.0);

    // Show tree structure
    db.printTree();
    db.printInorder();

    // Search
    std::cout << "\n  -- Searching --\n";
    auto* rec = db.search(210);
    if (rec) rec->print();

    rec = db.search(999);   // not found

    // Range query
    std::cout << "\n  -- Range Search: ID 100 to 250 --\n";
    auto range = db.rangeSearch(100, 250);
    for (const auto& r : range) {
        std::cout << "  [" << r.id << "] " << r.name
                  << " ($" << std::fixed << std::setprecision(2)
                  << r.price << ")\n";
    }

    // Delete
    std::cout << "\n  -- Deleting ID 210 (Eiffel Tower) --\n";
    db.remove(210);
    db.printTree();

    // Delete root-like node to test rotations
    std::cout << "\n  -- Deleting ID 315 (Colosseum) --\n";
    db.remove(315);
    db.printTree();
}

// ============================================================
//  STEP 3: Booking Queue & Itinerary Stack Demo
// ============================================================
void demoBookingManager() {
    printBanner("STEP 3: Queue (Bookings) & Stack (Itinerary Undo/Redo)");

    // ---- Booking Queue ----
    std::cout << "\n  ◈ Booking Queue (FIFO)\n  "
              << std::string(40, '-') << "\n\n";

    Tourism::BookingQueue queue;

    queue.enqueue(Tourism::BookingRequest("BK-001", "Aisha Sharma",    101, "Taj Mahal",        299.99));
    queue.enqueue(Tourism::BookingRequest("BK-002", "Carlos Rivera",   203, "Machu Picchu",     549.00));
    queue.enqueue(Tourism::BookingRequest("BK-003", "Yuki Tanaka",     305, "Mount Fuji Tour",  199.50));
    queue.enqueue(Tourism::BookingRequest("BK-004", "Emma Watson",     410, "Eiffel Tower",     129.99));
    queue.enqueue(Tourism::BookingRequest("BK-005", "James Liu",       502, "Great Wall Tour",  389.00));

    queue.printAll();

    std::cout << "\n  -- Processing bookings one by one --\n";
    queue.processNext();  // BK-001 → CONFIRMED
    queue.processNext();  // BK-002 → CONFIRMED
    queue.cancelNext();   // BK-003 → CANCELLED

    std::cout << "\n  -- Remaining queue & status --\n";
    queue.printAll();
    queue.printStatus();

    // ---- Itinerary Stack ----
    std::cout << "\n  ◈ Itinerary Stack (Undo/Redo)\n  "
              << std::string(40, '-') << "\n\n";

    Tourism::ItineraryStack stk;
    using IC = Tourism::ItineraryChange;
    using AT = IC::ActionType;

    stk.pushChange({ AT::ADD_DESTINATION,    "Added Paris stop",       "",        "Paris"      });
    stk.pushChange({ AT::UPDATE_DATE,         "Changed departure date", "June 10", "June 15"   });
    stk.pushChange({ AT::ADD_DESTINATION,    "Added Rome stop",        "",        "Rome"       });
    stk.pushChange({ AT::UPDATE_BUDGET,       "Budget updated",         "$3000",   "$3500"     });
    stk.pushChange({ AT::REMOVE_DESTINATION, "Removed Rome stop",      "Rome",    ""          });

    stk.printUndoStack();
    stk.printHistory();

    std::cout << "\n  -- Undo 3 times --\n";
    stk.undo();
    stk.undo();
    stk.undo();
    stk.printHistory();

    std::cout << "\n  -- Redo 1 time --\n";
    stk.redo();
    stk.printHistory();

    std::cout << "\n  -- Add new change (invalidates redo) --\n";
    stk.pushChange({ AT::ADD_DESTINATION, "Added Tokyo stop", "", "Tokyo" });
    stk.printHistory();

    stk.printUndoStack();
}

// ============================================================
//  STEP 4: Sort, Search & Hash Map Demo
// ============================================================
void demoSearchSortHash() {
    printBanner("STEP 4: Sorting, Binary Search & Hash Map");

    // ---- Travel Packages ----
    std::vector<Tourism::TravelPackage> packages = {
        { "PKG-001", "Golden Triangle",      "India",       1299.99, 7,  "Cultural"  },
        { "PKG-002", "Bali Paradise",         "Indonesia",    899.00, 5,  "Leisure"   },
        { "PKG-003", "Amazon Adventure",      "Brazil",      2199.50, 10, "Adventure" },
        { "PKG-004", "Swiss Alps Trek",       "Switzerland", 3499.00, 8,  "Adventure" },
        { "PKG-005", "Santorini Getaway",     "Greece",      1799.00, 6,  "Leisure"   },
        { "PKG-006", "Japan Cherry Blossom",  "Japan",       2499.00, 9,  "Cultural"  },
        { "PKG-007", "African Safari",        "Kenya",       4199.00, 12, "Adventure" },
        { "PKG-008", "Maldives Overwater",    "Maldives",    2999.00, 7,  "Leisure"   },
        { "PKG-009", "New York City Break",   "USA",          749.00, 4,  "Urban"     },
        { "PKG-010", "Machu Picchu Explorer", "Peru",        1599.00, 8,  "Adventure" },
    };

    // -- Merge Sort --
    auto pkgsCopy = packages;
    std::cout << "\n  [Merge Sort] Price Ascending\n  "
              << std::string(40, '-') << "\n";
    Tourism::PackageSorter::mergeSort(pkgsCopy);
    std::cout << "\n  Sorted Results:\n";
    for (const auto& p : pkgsCopy) p.print();

    // -- Quick Sort (descending) --
    auto pkgsCopy2 = packages;
    std::cout << "\n  [Quick Sort] Price Descending\n  "
              << std::string(40, '-') << "\n";
    Tourism::PackageSorter::quickSort(pkgsCopy2, false);
    std::cout << "\n  Sorted Results:\n";
    for (const auto& p : pkgsCopy2) p.print();

    // -- Binary Search --
    std::cout << "\n  [Binary Search] on price-sorted array\n  "
              << std::string(40, '-') << "\n\n";
    int idx = Tourism::PackageSearcher::binarySearchByPrice(pkgsCopy, 1799.00);
    if (idx >= 0) {
        std::cout << "\n  Found package:\n  -----------------------------------\n";
        pkgsCopy[idx].print();
    }

    // Range search
    std::cout << "\n  [Range Search] $1000 to $2500\n  "
              << std::string(40, '-') << "\n";
    auto inRange = Tourism::PackageSearcher::rangeSearchByPrice(pkgsCopy, 1000.0, 2500.0);
    std::cout << "  Found " << inRange.size() << " packages:\n";
    for (const auto& p : inRange)
        std::cout << "    " << std::setw(22) << std::left << p.name
                  << " $" << std::fixed << std::setprecision(2) << p.price << "\n";

    // -- Hash Map --
    std::cout << "\n  [Hash Map] Custom Booking ID Verification\n  "
              << std::string(40, '-') << "\n\n";

    Tourism::BookingHashMap hashMap(32);

    std::vector<Tourism::BookingRecord> records = {
        { "BK-1001", "Alice Johnson", "PKG-001", "Golden Triangle",    1299.99, "2025-07-01", true  },
        { "BK-1002", "Bob Smith",     "PKG-004", "Swiss Alps Trek",    3499.00, "2025-08-15", true  },
        { "BK-1003", "Priya Patel",   "PKG-006", "Japan Cherry Blos.", 2499.00, "2025-04-01", true  },
        { "BK-1004", "David Lee",     "PKG-007", "African Safari",     4199.00, "2025-09-20", false },
        { "BK-1005", "Mei Chen",      "PKG-002", "Bali Paradise",       899.00, "2025-06-10", true  },
    };

    for (const auto& r : records) hashMap.insert(r);
    hashMap.printStats();

    // Verifications
    std::cout << "\n  -- Booking ID Verification --\n";
    std::vector<std::string> lookups = { "BK-1001","BK-1003","BK-9999","BK-1005","BK-0000" };
    for (const auto& id : lookups) {
        bool found = hashMap.exists(id);
        std::cout << "  \"" << id << "\" → "
                  << (found ? "✓ EXISTS" : "✗ NOT FOUND") << "\n";
    }

    // Detail lookup
    std::cout << "\n  -- Full Record Lookup for BK-1002 --\n";
    auto* rec = hashMap.lookup("BK-1002");
    if (rec) rec->print();

    // Remove and verify
    hashMap.remove("BK-1004");
    std::cout << "  After removal — BK-1004 exists: "
              << (hashMap.exists("BK-1004") ? "YES" : "NO") << "\n";

    hashMap.printStats();

    // std::unordered_map demo
    Tourism::BookingHashMap::demonstrateStdHashMap(records);
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════════════════╗\n";
    std::cout << "  ║          C++ SMART TOURISM SYSTEM                   ║\n";
    std::cout << "  ║   Data Structures & Algorithms Showcase             ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════╝\n";

    demoGraph();
    std::cout << "\n";

    demoAVLTree();
    std::cout << "\n";

    demoBookingManager();
    std::cout << "\n";

    demoSearchSortHash();

    std::cout << "\n╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║          All modules completed successfully!         ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";

    return 0;
}

