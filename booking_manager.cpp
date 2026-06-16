// ============================================================
//  booking_manager.cpp — BookingQueue & ItineraryStack
// ============================================================
#include "../include/booking_manager.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace Tourism {

// -------------------------------------------------------
//  Helper: current timestamp string
// -------------------------------------------------------
static std::string nowTimestamp() {
    auto now   = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// -------------------------------------------------------
//  BookingRequest
// -------------------------------------------------------
BookingRequest::BookingRequest(const std::string& bId,
                               const std::string& customer,
                               int destId,
                               const std::string& destName,
                               double price)
    : bookingId(bId),
      customerName(customer),
      destinationId(destId),
      destinationName(destName),
      totalPrice(price),
      timestamp(nowTimestamp()),
      status(Status::PENDING) {}

std::string BookingRequest::statusToStr(Status s) {
    switch (s) {
        case Status::PENDING:    return "PENDING";
        case Status::PROCESSING: return "PROCESSING";
        case Status::CONFIRMED:  return "CONFIRMED";
        case Status::CANCELLED:  return "CANCELLED";
    }
    return "UNKNOWN";
}

void BookingRequest::print() const {
    std::cout << "  ┌────────────────────────────────────────\n";
    std::cout << "  │  Booking ID  : " << bookingId << "\n";
    std::cout << "  │  Customer    : " << customerName << "\n";
    std::cout << "  │  Destination : [" << destinationId << "] "
              << destinationName << "\n";
    std::cout << "  │  Price       : $" << std::fixed
              << std::setprecision(2) << totalPrice << "\n";
    std::cout << "  │  Timestamp   : " << timestamp << "\n";
    std::cout << "  │  Status      : " << statusToStr(status) << "\n";
    std::cout << "  └────────────────────────────────────────\n";
}

// -------------------------------------------------------
//  ItineraryChange
// -------------------------------------------------------
std::string ItineraryChange::actionToStr(ActionType a) {
    switch (a) {
        case ActionType::ADD_DESTINATION:    return "ADD_DESTINATION";
        case ActionType::REMOVE_DESTINATION: return "REMOVE_DESTINATION";
        case ActionType::UPDATE_DATE:        return "UPDATE_DATE";
        case ActionType::UPDATE_BUDGET:      return "UPDATE_BUDGET";
    }
    return "UNKNOWN";
}

void ItineraryChange::print() const {
    std::cout << "  [" << actionToStr(action) << "] " << description << "\n";
    std::cout << "     Previous: " << previousValue << "\n";
    std::cout << "     New     : " << newValue      << "\n";
}

// ═══════════════════════════════════════════════════════════
//  BookingQueue — FIFO Processing
// ═══════════════════════════════════════════════════════════

void BookingQueue::enqueue(const BookingRequest& req) {
    queue_.push(req);
    std::cout << "[Queue] Enqueued booking: " << req.bookingId
              << " | " << req.customerName
              << " → " << req.destinationName
              << " | Queue size: " << queue_.size() << "\n";
}

bool BookingQueue::processNext() {
    if (queue_.empty()) {
        std::cout << "[Queue] No pending bookings to process.\n";
        return false;
    }
    BookingRequest req = queue_.front();
    queue_.pop();

    req.status = BookingRequest::Status::CONFIRMED;
    processed_.push_back(req);

    std::cout << "\n[Queue] ✓ Processing Booking:\n";
    req.print();
    return true;
}

bool BookingQueue::cancelNext() {
    if (queue_.empty()) {
        std::cout << "[Queue] No pending bookings to cancel.\n";
        return false;
    }
    BookingRequest req = queue_.front();
    queue_.pop();

    req.status = BookingRequest::Status::CANCELLED;
    processed_.push_back(req);

    std::cout << "[Queue] ✗ Cancelled Booking: " << req.bookingId << "\n";
    return true;
}

const BookingRequest& BookingQueue::peek() const {
    if (queue_.empty())
        throw std::runtime_error("BookingQueue is empty");
    return queue_.front();
}

void BookingQueue::printAll() const {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║     Pending Booking Queue            ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    if (queue_.empty()) {
        std::cout << "  (queue is empty)\n\n";
        return;
    }
    // Copy to iterate
    auto copy = queue_;
    int pos = 1;
    while (!copy.empty()) {
        std::cout << "  [Position " << pos++ << "]\n";
        copy.front().print();
        copy.pop();
    }
}

void BookingQueue::printStatus() const {
    std::cout << "\n  Queue Status:\n";
    std::cout << "    Pending   : " << queue_.size() << "\n";
    std::cout << "    Processed : " << processed_.size() << "\n";
    if (!processed_.empty()) {
        std::cout << "\n  Processed Bookings:\n";
        for (const auto& r : processed_) {
            std::cout << "    " << r.bookingId << " — "
                      << BookingRequest::statusToStr(r.status) << "\n";
        }
    }
    std::cout << "\n";
}

// ═══════════════════════════════════════════════════════════
//  ItineraryStack — Undo / Redo
// ═══════════════════════════════════════════════════════════

void ItineraryStack::pushChange(const ItineraryChange& change) {
    undoStack_.push(change);
    clearRedoHistory();   // new action invalidates redo history
    std::cout << "[Stack] Change recorded: "
              << ItineraryChange::actionToStr(change.action)
              << " — " << change.description << "\n";
    std::cout << "        Undo depth: " << undoStack_.size() << "\n";
}

bool ItineraryStack::undo() {
    if (undoStack_.empty()) {
        std::cout << "[Stack] Nothing to undo.\n";
        return false;
    }
    ItineraryChange change = undoStack_.top();
    undoStack_.pop();
    redoStack_.push(change);

    std::cout << "\n[Stack] ↩ UNDO: " << change.description << "\n";
    std::cout << "        Reverting: \"" << change.newValue
              << "\" → \"" << change.previousValue << "\"\n";
    std::cout << "        Undo remaining: " << undoStack_.size()
              << " | Redo available: " << redoStack_.size() << "\n";
    return true;
}

bool ItineraryStack::redo() {
    if (redoStack_.empty()) {
        std::cout << "[Stack] Nothing to redo.\n";
        return false;
    }
    ItineraryChange change = redoStack_.top();
    redoStack_.pop();
    undoStack_.push(change);

    std::cout << "\n[Stack] ↪ REDO: " << change.description << "\n";
    std::cout << "        Re-applying: \"" << change.previousValue
              << "\" → \"" << change.newValue << "\"\n";
    std::cout << "        Undo depth: " << undoStack_.size()
              << " | Redo remaining: " << redoStack_.size() << "\n";
    return true;
}

void ItineraryStack::clearRedoHistory() {
    while (!redoStack_.empty()) redoStack_.pop();
}

void ItineraryStack::printUndoStack() const {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║     Undo Stack (top = most recent)   ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    if (undoStack_.empty()) { std::cout << "  (empty)\n\n"; return; }
    auto copy = undoStack_;
    int pos = 1;
    while (!copy.empty()) {
        std::cout << "  [" << pos++ << "] ";
        copy.top().print();
        copy.pop();
    }
    std::cout << "\n";
}

void ItineraryStack::printRedoStack() const {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║     Redo Stack                       ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    if (redoStack_.empty()) { std::cout << "  (empty)\n\n"; return; }
    auto copy = redoStack_;
    int pos = 1;
    while (!copy.empty()) {
        std::cout << "  [" << pos++ << "] ";
        copy.top().print();
        copy.pop();
    }
    std::cout << "\n";
}

void ItineraryStack::printHistory() const {
    std::cout << "\n  Undo depth : " << undoStack_.size() << "\n";
    std::cout << "  Redo depth : " << redoStack_.size() << "\n";
    std::cout << "  Can undo   : " << (canUndo() ? "Yes" : "No") << "\n";
    std::cout << "  Can redo   : " << (canRedo() ? "Yes" : "No") << "\n\n";
}

} // namespace Tourism