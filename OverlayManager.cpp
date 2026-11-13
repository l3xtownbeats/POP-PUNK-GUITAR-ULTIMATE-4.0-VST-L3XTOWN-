#include "OverlayManager.h"

namespace ppgu
{
OverlayManager::OverlayManager(Listener* l) : listener_(l) {}

bool OverlayManager::push(const Entry& e)
{
    Listener* l = nullptr;
    std::string id;
    {
        std::lock_guard<std::mutex> g(mtx_);
        for (const auto& x : stack_)
            if (x.id == e.id) return false;                 // duplicate id rejected
        if (stack_.size() >= kCapacity) return false;       // capacity hard limit
        stack_.push_back(e);
        l = listener_;
        id = e.id;
    }
    if (l) l->overlayAdded(id);                             // outside the lock
    return true;
}

bool OverlayManager::close(const std::string& id)
{
    Listener* l = nullptr;
    {
        std::lock_guard<std::mutex> g(mtx_);
        for (auto it = stack_.begin(); it != stack_.end(); ++it)
        {
            if (it->id != id) continue;
            if (!it->closable) return false;                // deterministic refusal
            stack_.erase(it);
            l = listener_;
            break;
        }
    }
    if (l) l->overlayClosed(id, true);                      // outside the lock
    return l != nullptr;
}

bool OverlayManager::setVisible(const std::string& id, bool v)
{
    std::lock_guard<std::mutex> g(mtx_);
    for (auto& e : stack_)
        if (e.id == id) { e.visible = v; return true; }
    return false;
}

bool OverlayManager::setModal(const std::string& id, bool v)
{
    std::lock_guard<std::mutex> g(mtx_);
    for (auto& e : stack_)
        if (e.id == id) { e.modal = v; return true; }
    return false;
}

bool OverlayManager::topId(std::string& out) const
{
    std::lock_guard<std::mutex> g(mtx_);
    for (auto it = stack_.rbegin(); it != stack_.rend(); ++it)
        if (it->visible) { out = it->id; return true; }
    return false;
}

bool OverlayManager::topModalId(std::string& out) const
{
    std::lock_guard<std::mutex> g(mtx_);
    for (auto it = stack_.rbegin(); it != stack_.rend(); ++it)
        if (it->visible && it->modal) { out = it->id; return true; }
    return false;
}

bool OverlayManager::blockedForNew() const
{
    std::string t;
    return topModalId(t);
}
} // namespace ppgu
