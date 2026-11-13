#pragma once
// PPGU OverlayManager — O1/O2 contract (JUCE-free, testable core).
// Capacity 8, monotonic z-order, modal-blocking by topmost visible modal,
// closeTop honours closable, listener invoked outside the lock.
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace ppgu
{
class Listener
{
public:
    virtual ~Listener() = default;
    virtual void overlayClosed(const std::string& id, bool byUser) = 0;
    virtual void overlayRaised(const std::string& id) = 0;
    virtual void overlayAdded(const std::string& id) = 0;
};

class OverlayManager
{
public:
    struct Entry
    {
        std::string id;
        bool modal = false;
        bool visible = false;
        bool closable = true;
    };

    static constexpr std::size_t kCapacity = 8;

    explicit OverlayManager(Listener* listener = nullptr);

    bool push(const Entry& e);            // false: capacity or duplicate id
    bool close(const std::string& id);
    bool setVisible(const std::string& id, bool v);
    bool setModal(const std::string& id, bool v);

    bool topId(std::string& out) const;   // topmost visible
    bool topModalId(std::string& out) const;
    bool blockedForNew() const;           // true while topmost visible is modal

private:
    mutable std::mutex mtx_;
    std::vector<Entry> stack_;
    Listener* listener_ = nullptr;        // guarded by mtx_
};
} // namespace ppgu
