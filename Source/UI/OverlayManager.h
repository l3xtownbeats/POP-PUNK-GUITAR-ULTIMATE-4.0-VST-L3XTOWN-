#pragma once
// PPGU OverlayManager — O1/O2 contract. JUCE-free and UI-thread owned.
// The vector order is the z-order; callbacks are always made outside the lock.
#include <cstddef>
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

class OverlayManager final
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

    bool push(const Entry& entry);
    bool close(const std::string& id, bool byUser = true);
    bool closeTop(bool byUser = true);
    bool setVisible(const std::string& id, bool visible);
    bool setModal(const std::string& id, bool modal);
    bool raise(const std::string& id);

    bool topId(std::string& out) const;
    bool topModalId(std::string& out) const;
    bool blockedForNew() const;
    std::size_t size() const;

    // Registration and mutations are serialized by the owning UI thread.
    void setListener(Listener* listener);

private:
    mutable std::mutex mutex;
    std::vector<Entry> stack;
    Listener* listener = nullptr; // copied under mutex; invoked after unlock
};
} // namespace ppgu
