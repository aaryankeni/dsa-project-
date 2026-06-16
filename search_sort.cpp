// ============================================================
//  search_sort.cpp — Sorting, Binary Search & Hash Map
// ============================================================
#include "../include/search_sort.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cassert>

namespace Tourism {

// -------------------------------------------------------
//  TravelPackage
// -------------------------------------------------------
void TravelPackage::print() const {
    std::cout << "  │  ID       : " << packageId << "\n";
    std::cout << "  │  Name     : " << name << "\n";
    std::cout << "  │  Dest     : " << destination << "\n";
    std::cout << "  │  Price    : $" << std::fixed
              << std::setprecision(2) << price << "\n";
    std::cout << "  │  Duration : " << durationDays << " days\n";
    std::cout << "  │  Category : " << category << "\n";
    std::cout << "  ├────────────────────────────────────────\n";
}

// ═══════════════════════════════════════════════════════════
//  MERGE SORT
// ═══════════════════════════════════════════════════════════

void PackageSorter::merge(
    std::vector<TravelPackage>& arr, int l, int m, int r,
    std::function<bool(const TravelPackage&, const TravelPackage&)> cmp)
{
    // Create left and right sub-arrays
    std::vector<TravelPackage> L(arr.begin() + l, arr.begin() + m + 1);
    std::vector<TravelPackage> R(arr.begin() + m + 1, arr.begin() + r + 1);

    int i = 0, j = 0, k = l;
    while (i < (int)L.size() && j < (int)R.size()) {
        // FIX: Use !cmp(R[j], L[i]) to preserve stable sorting
        if (!cmp(R[j], L[i])) arr[k++] = L[i++];
        else                  arr[k++] = R[j++];
    }
    while (i < (int)L.size()) arr[k++] = L[i++];
    while (j < (int)R.size()) arr[k++] = R[j++];
}

void PackageSorter::mergeSortHelper(
    std::vector<TravelPackage>& arr, int l, int r,
    std::function<bool(const TravelPackage&, const TravelPackage&)> cmp)
{
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortHelper(arr, l,   m, cmp);
    mergeSortHelper(arr, m+1, r, cmp);
    merge(arr, l, m, r, cmp);
}

void PackageSorter::mergeSort(std::vector<TravelPackage>& packages,
                               bool ascending) {
    if (packages.empty()) return;
    auto cmp = ascending
        ? [](const TravelPackage& a, const TravelPackage& b){ return a.price < b.price; }
        : std::function<bool(const TravelPackage&, const TravelPackage&)>(
            [](const TravelPackage& a, const TravelPackage& b){ return a.price > b.price; });

    std::cout << "[MergeSort] Sorting " << packages.size()
              << " packages by price "
              << (ascending ? "ascending" : "descending") << "...\n";
    mergeSortHelper(packages, 0, (int)packages.size() - 1, cmp);
    std::cout << "[MergeSort] Done. Price range: $"
              << std::fixed << std::setprecision(2)
              << packages.front().price << " – $"
              << packages.back().price << "\n";
}

// ═══════════════════════════════════════════════════════════
//  QUICK SORT
// ═══════════════════════════════════════════════════════════

int PackageSorter::partition(
    std::vector<TravelPackage>& arr, int low, int high,
    std::function<bool(const TravelPackage&, const TravelPackage&)> cmp)
{
    // Median-of-three pivot selection
    int mid = low + (high - low) / 2;
    if (cmp(arr[mid], arr[low]))  std::swap(arr[mid], arr[low]);
    if (cmp(arr[high], arr[low])) std::swap(arr[high], arr[low]);
    if (cmp(arr[mid], arr[high])) std::swap(arr[mid], arr[high]);
    // arr[high] is now the median → use as pivot
    TravelPackage pivot = arr[high];

    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (cmp(arr[j], pivot)) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void PackageSorter::quickSortHelper(
    std::vector<TravelPackage>& arr, int low, int high,
    std::function<bool(const TravelPackage&, const TravelPackage&)> cmp)
{
    if (low < high) {
        int pi = partition(arr, low, high, cmp);
        quickSortHelper(arr, low,    pi - 1, cmp);
        quickSortHelper(arr, pi + 1, high,   cmp);
    }
}

void PackageSorter::quickSort(std::vector<TravelPackage>& packages,
                               bool ascending, int left, int right) {
    if (packages.empty()) return;
    auto cmp = ascending
        ? [](const TravelPackage& a, const TravelPackage& b){ return a.price < b.price; }
        : std::function<bool(const TravelPackage&, const TravelPackage&)>(
            [](const TravelPackage& a, const TravelPackage& b){ return a.price > b.price; });

    int l = (left  == -1) ? 0 : left;
    int r = (right == -1) ? (int)packages.size() - 1 : right;

    std::cout << "[QuickSort] Sorting " << packages.size()
              << " packages by price "
              << (ascending ? "ascending" : "descending") << "...\n";
    quickSortHelper(packages, l, r, cmp);
    std::cout << "[QuickSort] Done. Price range: $"
              << std::fixed << std::setprecision(2)
              << packages.front().price << " – $"
              << packages.back().price << "\n";
}

void PackageSorter::sortBy(
    std::vector<TravelPackage>& packages,
    std::function<bool(const TravelPackage&, const TravelPackage&)> cmp)
{
    if (packages.empty()) return;
    mergeSortHelper(packages, 0, (int)packages.size() - 1, cmp);
}

// ═══════════════════════════════════════════════════════════
//  BINARY SEARCH
// ═══════════════════════════════════════════════════════════

int PackageSearcher::binarySearchByPrice(
    const std::vector<TravelPackage>& packages,
    double targetPrice, double tolerance)
{
    int lo = 0, hi = (int)packages.size() - 1;

    std::cout << "[BinarySearch] Looking for price $"
              << std::fixed << std::setprecision(2) << targetPrice
              << " (±" << tolerance << ")  in "
              << packages.size() << " packages.\n";

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        double diff = packages[mid].price - targetPrice;

        std::cout << "  Checking index " << mid
                  << " → $" << packages[mid].price << "\n";

        if (std::abs(diff) <= tolerance) {
            std::cout << "  ✓ Found: " << packages[mid].name
                      << " at index " << mid << "\n";
            return mid;
        }
        if (diff < 0) lo = mid + 1;
        else          hi = mid - 1;
    }
    std::cout << "  ✗ Price not found within tolerance.\n";
    return -1;
}

std::vector<TravelPackage> PackageSearcher::rangeSearchByPrice(
    const std::vector<TravelPackage>& packages, double lo, double hi)
{
    // Find left boundary
    int left = 0, right = (int)packages.size();
    {
        int l = 0, r = (int)packages.size();
        while (l < r) {
            int m = (l + r) / 2;
            if (packages[m].price < lo) l = m + 1; else r = m;
        }
        left = l;
    }
    // Find right boundary
    {
        int l = 0, r = (int)packages.size();
        while (l < r) {
            int m = (l + r) / 2;
            if (packages[m].price <= hi) l = m + 1; else r = m;
        }
        right = l;
    }
    return { packages.begin() + left, packages.begin() + right };
}

int PackageSearcher::linearSearchById(
    const std::vector<TravelPackage>& packages, const std::string& packageId)
{
    for (int i = 0; i < (int)packages.size(); ++i)
        if (packages[i].packageId == packageId) return i;
    return -1;
}

// ═══════════════════════════════════════════════════════════
//  BOOKING HASH MAP (custom chaining hash table)
// ═══════════════════════════════════════════════════════════

void BookingRecord::print() const {
    std::cout << "  ┌────────────────────────────────────────\n";
    std::cout << "  │  Booking ID  : " << bookingId << "\n";
    std::cout << "  │  Customer    : " << customerName << "\n";
    std::cout << "  │  Package ID  : " << packageId << "\n";
    std::cout << "  │  Package     : " << packageName << "\n";
    std::cout << "  │  Amount Paid : $" << std::fixed
              << std::setprecision(2) << amountPaid << "\n";
    std::cout << "  │  Date        : " << bookingDate << "\n";
    std::cout << "  │  Active      : " << (isActive ? "Yes" : "No") << "\n";
    std::cout << "  └────────────────────────────────────────\n";
}

BookingHashMap::BookingHashMap(std::size_t buckets)
    : table_(buckets), buckets_(buckets) {}

// djb2 hash function
std::size_t BookingHashMap::hashKey(const std::string& key) const {
    std::size_t hash = 5381;
    for (unsigned char c : key)
        hash = ((hash << 5) + hash) ^ c;   // hash * 33 XOR c
    return hash % buckets_;
}

void BookingHashMap::insert(const BookingRecord& record) {
    std::size_t idx = hashKey(record.bookingId);
    // Check for duplicate
    for (auto& r : table_[idx]) {
        if (r.bookingId == record.bookingId) {
            r = record;   // update
            std::cout << "[HashMap] Updated existing booking: "
                      << record.bookingId << "\n";
            return;
        }
    }
    table_[idx].push_back(record);
    ++size_;
    std::cout << "[HashMap] Inserted: " << record.bookingId
              << " → bucket " << idx << "\n";
}

bool BookingHashMap::exists(const std::string& bookingId) const {
    std::size_t idx = hashKey(bookingId);
    for (const auto& r : table_[idx])
        if (r.bookingId == bookingId) return true;
    return false;
}

BookingRecord* BookingHashMap::lookup(const std::string& bookingId) {
    std::size_t idx = hashKey(bookingId);
    for (auto& r : table_[idx])
        if (r.bookingId == bookingId) return &r;
    return nullptr;
}

bool BookingHashMap::remove(const std::string& bookingId) {
    std::size_t idx = hashKey(bookingId);
    auto& bucket = table_[idx];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->bookingId == bookingId) {
            bucket.erase(it);
            --size_;
            std::cout << "[HashMap] Removed booking: " << bookingId << "\n";
            return true;
        }
    }
    std::cout << "[HashMap] Booking ID not found: " << bookingId << "\n";
    return false;
}

double BookingHashMap::loadFactor() const {
    return static_cast<double>(size_) / static_cast<double>(buckets_);
}

void BookingHashMap::printAll() const {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║     Booking Hash Map Contents        ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    for (std::size_t i = 0; i < buckets_; ++i) {
        if (!table_[i].empty()) {
            std::cout << "  Bucket [" << std::setw(3) << i << "]:\n";
            for (const auto& r : table_[i]) r.print();
        }
    }
}

void BookingHashMap::printStats() const {
    int collisions = 0;
    int maxChain   = 0;
    for (const auto& bucket : table_) {
        if (bucket.size() > 1) collisions += (int)bucket.size() - 1;
        maxChain = std::max(maxChain, (int)bucket.size());
    }
    std::cout << "\n  ── Hash Map Statistics ──────────────────\n";
    std::cout << "  Buckets     : " << buckets_ << "\n";
    std::cout << "  Elements    : " << size_    << "\n";
    std::cout << "  Load Factor : " << std::fixed << std::setprecision(3)
              << loadFactor() << "\n";
    std::cout << "  Collisions  : " << collisions << "\n";
    std::cout << "  Max Chain   : " << maxChain << "\n";
    std::cout << "  ─────────────────────────────────────────\n\n";
}

// -------------------------------------------------------
//  Demonstrate std::unordered_map as an alternative
// -------------------------------------------------------
void BookingHashMap::demonstrateStdHashMap(
    const std::vector<BookingRecord>& records)
{
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║  std::unordered_map Demo             ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";

    std::unordered_map<std::string, BookingRecord> stdMap;
    for (const auto& r : records)
        stdMap[r.bookingId] = r;

    std::cout << "  Inserted " << stdMap.size() << " records.\n\n";

    // Lookups
    for (const auto& r : records) {
        bool found = stdMap.count(r.bookingId) > 0;
        std::cout << "  Lookup \"" << r.bookingId << "\": "
                  << (found ? "FOUND" : "NOT FOUND") << "\n";
    }

    // Non-existent lookup
    std::cout << "  Lookup \"BK-9999\": "
              << (stdMap.count("BK-9999") ? "FOUND" : "NOT FOUND") << "\n";
    std::cout << "  Load factor: " << std::fixed << std::setprecision(3)
              << stdMap.load_factor() << "\n\n";
}

} // namespace Tourism