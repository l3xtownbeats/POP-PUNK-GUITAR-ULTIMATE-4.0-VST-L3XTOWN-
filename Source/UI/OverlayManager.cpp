#include "OverlayManager.h"

#include <algorithm>
#include <iterator>
#include <utility>

namespace ppgu
{
OverlayManager::OverlayManager(Listener* listenerIn) : listener(listenerIn) {}

bool OverlayManager::push(const Entry& entry)
{
    if (entry.id.empty())
        return false;

    Listener* callback = nullptr;
    const auto id = entry.id;
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (stack.size() >= kCapacity)
            return false;
        if (std::any_of(stack.begin(), stack.end(),
                        [&id](const Entry& current) { return current.id == id; }))
            return false;

        stack.push_back(entry);
        callback = listener;
    }

    if (callback != nullptr)
        callback->overlayAdded(id);
    return true;
}

bool OverlayManager::close(const std::string& id, bool byUser)
{
    Listener* callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex);
        const auto found = std::find_if(stack.begin(), stack.end(),
            [&id](const Entry& entry) { return entry.id == id; });
        if (found == stack.end() || !found->closable)
            return false;

        stack.erase(found);
        callback = listener;
    }

    if (callback != nullptr)
        callback->overlayClosed(id, byUser);
    return true; // Closing succeeds independently of listener registration.
}

bool OverlayManager::closeTop(bool byUser)
{
    Listener* callback = nullptr;
    std::string id;
    {
        std::lock_guard<std::mutex> lock(mutex);
        const auto found = std::find_if(stack.rbegin(), stack.rend(),
            [](const Entry& entry) { return entry.visible; });
        if (found == stack.rend() || !found->closable)
            return false;

        id = found->id;
        stack.erase(std::next(found).base());
        callback = listener;
    }

    if (callback != nullptr)
        callback->overlayClosed(id, byUser);
    return true;
}

bool OverlayManager::setVisible(const std::string& id, bool visible)
{
    std::lock_guard<std::mutex> lock(mutex);
    const auto found = std::find_if(stack.begin(), stack.end(),
        [&id](const Entry& entry) { return entry.id == id; });
    if (found == stack.end())
        return false;
    found->visible = visible;
    return true;
}

bool OverlayManager::setModal(const std::string& id, bool modal)
{
    std::lock_guard<std::mutex> lock(mutex);
    const auto found = std::find_if(stack.begin(), stack.end(),
        [&id](const Entry& entry) { return entry.id == id; });
    if (found == stack.end())
        return false;
    found->modal = modal;
    return true;
}

bool OverlayManager::raise(const std::string& id)
{
    Listener* callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex);
        const auto found = std::find_if(stack.begin(), stack.end(),
            [&id](const Entry& entry) { return entry.id == id; });
        if (found == stack.end() || !found->visible)
            return false;

        Entry raised = std::move(*found);
        stack.erase(found);
        stack.push_back(std::move(raised));
        callback = listener;
    }

    if (callback != nullptr)
        callback->overlayRaised(id);
    return true;
}

bool OverlayManager::topId(std::string& out) const
{
    std::lock_guard<std::mutex> lock(mutex);
    const auto found = std::find_if(stack.rbegin(), stack.rend(),
        [](const Entry& entry) { return entry.visible; });
    if (found == stack.rend())
        return false;
    out = found->id;
    return true;
}

bool OverlayManager::topModalId(std::string& out) const
{
    std::lock_guard<std::mutex> lock(mutex);
    const auto found = std::find_if(stack.rbegin(), stack.rend(),
        [](const Entry& entry) { return entry.visible && entry.modal; });
    if (found == stack.rend())
        return false;
    out = found->id;
    return true;
}

bool OverlayManager::blockedForNew() const
{
    std::string modalId;
    return topModalId(modalId);
}

std::size_t OverlayManager::size() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return stack.size();
}

void OverlayManager::setListener(Listener* listenerIn)
{
    std::lock_guard<std::mutex> lock(mutex);
    listener = listenerIn;
}
} // namespace ppgu
