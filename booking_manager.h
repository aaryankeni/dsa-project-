// ============================================================
//  Smart Tourism System — Booking & Itinerary Manager (Header)
//  Queue  → processes Booking Requests (FIFO)
//  Stack  → manages Itinerary Changes  (Undo / Redo)
// ============================================================
#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace Tourism {

// -------------------------------------------------------
//  BookingRequest — a single booking in the FIFO queue
// -------------------------------------------------------
struct BookingRequest {
    std::string bookingId;
    std::string customerName;
    int         destinationId;
    std::string destinationName;
    double      totalPrice;
    std::string timestamp;
    enum class Status { PENDING, PROCESSING, CONFIRMED, CANCELLED } status;

    BookingRequest(const std::string& bId, const std::string& customer,
                   int destId, const std::string& destName, double price);
    void print() const;
    static std::string statusToStr(Status s);
};

// -------------------------------------------------------
//  ItineraryChange — one reversible action on a trip plan
// -------------------------------------------------------
struct ItineraryChange {
    enum class ActionType { ADD_DESTINATION, REMOVE_DESTINATION,
                            UPDATE_DATE,    UPDATE_BUDGET    } action;
    std::string description;    // human-readable summary
    std::string previousValue;  // value before the change (for undo)
    std::string newValue;       // value after  the change

    static std::string actionToStr(ActionType a);
    void print() const;
};

// -------------------------------------------------------
//  BookingQueue — FIFO queue for booking requests
// -------------------------------------------------------
class BookingQueue {
public:
    void    enqueue(const BookingRequest& req);
    bool    processNext();          // dequeue and mark CONFIRMED
    bool    cancelNext();           // dequeue and mark CANCELLED
    const BookingRequest& peek()  const;
    bool    isEmpty()             const { return queue_.empty(); }
    int     size()                const { return static_cast<int>(queue_.size()); }
    void    printAll()            const;
    void    printStatus()         const;

private:
    std::queue<BookingRequest> queue_;
    std::vector<BookingRequest> processed_;  // audit log
};

// -------------------------------------------------------
//  ItineraryStack — undo / redo stack for trip edits
// -------------------------------------------------------
class ItineraryStack {
public:
    void  pushChange(const ItineraryChange& change);
    bool  undo();           // pop from undo stack → push to redo stack
    bool  redo();           // pop from redo stack → push to undo stack
    void  clearRedoHistory();

    bool  canUndo()  const { return !undoStack_.empty(); }
    bool  canRedo()  const { return !redoStack_.empty(); }
    int   undoDepth() const { return static_cast<int>(undoStack_.size()); }
    int   redoDepth() const { return static_cast<int>(redoStack_.size()); }

    void  printUndoStack()  const;
    void  printRedoStack()  const;
    void  printHistory()    const;

private:
    std::stack<ItineraryChange> undoStack_;
    std::stack<ItineraryChange> redoStack_;
};

} // namespace Tourism