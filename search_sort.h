// ============================================================
//  Smart Tourism System — Sorting, Searching & Hashing (Header)
//  • Merge Sort / Quick Sort  → rank packages by price
//  • Binary Search            → find a package in sorted array
//  • Hash Map                 → instant Booking ID lookup
// ============================================================
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>
#include <list>

namespace Tourism {

// -------------------------------------------------------
//  TravelPackage — item sorted and searched
// -------------------------------------------------------
struct TravelPackage {
    std::string packageId;
    std::string name;
    std::string destination;
    double      price;
    int         durationDays;
    std::string category;   // e.g., "Adventure", "Cultural", "Leisure"

    void print() const;
};

// -------------------------------------------------------
//  SortAlgorithms
//  Merge Sort and Quick Sort on TravelPackage vectors.
// -------------------------------------------------------
class PackageSorter {
public:
    // Sort by price ascending (default) or descending
    static void mergeSort(std::vector<TravelPackage>& packages,
                          bool ascending = true);
    static void quickSort(std::vector<TravelPackage>& packages,
                          bool ascending = true,
                          int  left = -1, int right = -1);

    // Generic comparator-based sort (delegates to mergeSort)
    static void sortBy(std::vector<TravelPackage>& packages,
                       std::function<bool(const TravelPackage&,
                                         const TravelPackage&)> cmp);

private:
    static void mergeSortHelper(std::vector<TravelPackage>& arr,
                                int l, int r,
                                std::function<bool(const TravelPackage&,
                                                   const TravelPackage&)> cmp);
    static void merge(std::vector<TravelPackage>& arr, int l, int m, int r,
                      std::function<bool(const TravelPackage&,
                                        const TravelPackage&)> cmp);
    static int  partition(std::vector<TravelPackage>& arr,
                          int low, int high,
                          std::function<bool(const TravelPackage&,
                                            const TravelPackage&)> cmp);
    static void quickSortHelper(std::vector<TravelPackage>& arr,
                                int low, int high,
                                std::function<bool(const TravelPackage&,
                                                   const TravelPackage&)> cmp);
};

// -------------------------------------------------------
//  PackageSearcher — Binary Search on sorted package array
// -------------------------------------------------------
class PackageSearcher {
public:
    // Returns index or -1 if not found (array must be sorted by price first)
    static int  binarySearchByPrice(const std::vector<TravelPackage>& packages,
                                    double targetPrice,
                                    double tolerance = 0.01);

    // Find all packages within a price range [lo, hi]
    static std::vector<TravelPackage> rangeSearchByPrice(
        const std::vector<TravelPackage>& packages,
        double lo, double hi);

    // Linear search by package ID (unsorted)
    static int  linearSearchById(const std::vector<TravelPackage>& packages,
                                 const std::string& packageId);
};

// -------------------------------------------------------
//  BookingHashMap — O(1) average Booking ID lookup
//  Custom chaining hash table implementation
// -------------------------------------------------------
struct BookingRecord {
    std::string bookingId;
    std::string customerName;
    std::string packageId;
    std::string packageName;
    double      amountPaid;
    std::string bookingDate;
    bool        isActive;

    void print() const;
};

class BookingHashMap {
public:
    explicit BookingHashMap(std::size_t buckets = 64);
    ~BookingHashMap() = default;

    // Core operations
    void   insert(const BookingRecord& record);
    bool   exists(const std::string& bookingId)   const;
    BookingRecord* lookup(const std::string& bookingId);
    bool   remove(const std::string& bookingId);

    // Stats & display
    std::size_t size()         const { return size_; }
    bool        empty()        const { return size_ == 0; }
    double      loadFactor()   const;
    void        printAll()     const;
    void        printStats()   const;

    // std::unordered_map wrapper (simpler alternative demo)
    static void demonstrateStdHashMap(
        const std::vector<BookingRecord>& records);

private:
    using Bucket = std::list<BookingRecord>;
    std::vector<Bucket> table_;
    std::size_t         buckets_;
    std::size_t         size_ = 0;

    std::size_t hashKey(const std::string& key) const;
};

} // namespace Tourism