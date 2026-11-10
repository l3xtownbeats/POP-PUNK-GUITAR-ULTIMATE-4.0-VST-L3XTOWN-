#include "MappingEditorModel.h"
#include <algorithm>
#include <cmath>

namespace ppgu::sampler
{
bool MappingEditorModel::replace(std::vector<MappingRegion> incoming, std::string& error)
{
    for (const auto& item : incoming)
    {
        const auto& r = item.region;
        if (r.lowKey < 0 || r.highKey > 127 || r.lowKey > r.highKey
            || r.rootKey < r.lowKey || r.rootKey > r.highKey
            || r.lowVelocity < 0 || r.highVelocity > 127 || r.lowVelocity > r.highVelocity
            || item.roundRobinGroup < 0)
        { error = "Mapping contains an invalid key, velocity or round-robin range."; return false; }
    }
    std::sort(incoming.begin(), incoming.end(), [](const auto& a, const auto& b) { return a.id < b.id; });
    std::lock_guard lock(mutex);
    regions = std::move(incoming);
    rrCursors.clear();
    error.clear();
    return true;
}

bool MappingEditorModel::setKeyRange(std::size_t id, int low, int high, int root, std::string& error)
{
    if (low < 0 || high > 127 || low > high || root < low || root > high)
    { error = "Key range must be 0..127 and contain its root note."; return false; }
    std::lock_guard lock(mutex);
    auto it = std::find_if(regions.begin(), regions.end(), [id](const auto& r) { return r.id == id; });
    if (it == regions.end()) { error = "Mapping region not found."; return false; }
    it->region.lowKey = low; it->region.highKey = high; it->region.rootKey = root;
    error.clear(); return true;
}

bool MappingEditorModel::setVelocityRange(std::size_t id, int low, int high, std::string& error)
{
    if (low < 0 || high > 127 || low > high)
    { error = "Velocity range must be within 0..127."; return false; }
    std::lock_guard lock(mutex);
    auto it = std::find_if(regions.begin(), regions.end(), [id](const auto& r) { return r.id == id; });
    if (it == regions.end()) { error = "Mapping region not found."; return false; }
    it->region.lowVelocity = low; it->region.highVelocity = high;
    error.clear(); return true;
}

bool MappingEditorModel::setRoundRobin(std::size_t id, int group, std::string& error)
{
    if (group < 0 || group > 1024) { error = "Round-robin group must be 0..1024."; return false; }
    std::lock_guard lock(mutex);
    auto it = std::find_if(regions.begin(), regions.end(), [id](const auto& r) { return r.id == id; });
    if (it == regions.end()) { error = "Mapping region not found."; return false; }
    it->roundRobinGroup = group;
    error.clear(); return true;
}

std::vector<MappingRegion> MappingEditorModel::snapshot() const
{
    std::lock_guard lock(mutex);
    return regions;
}

std::optional<MappingSelection> MappingEditorModel::select(int note, int velocity)
{
    if (note < 0 || note > 127 || velocity < 0 || velocity > 127) return std::nullopt;
    std::lock_guard lock(mutex);
    std::vector<const MappingRegion*> matches;
    for (const auto& item : regions)
        if (note >= item.region.lowKey && note <= item.region.highKey
            && velocity >= item.region.lowVelocity && velocity <= item.region.highVelocity)
            matches.push_back(&item);
    if (matches.empty()) return std::nullopt;

    const int rrGroup = matches.front()->roundRobinGroup;
    if (rrGroup > 0)
    {
        std::vector<const MappingRegion*> rr;
        for (const auto* item : matches)
            if (item->roundRobinGroup == rrGroup) rr.push_back(item);
        if (!rr.empty())
        {
            auto cursor = std::find_if(rrCursors.begin(), rrCursors.end(),
                [rrGroup](const auto& p) { return p.first == rrGroup; });
            if (cursor == rrCursors.end()) { rrCursors.emplace_back(rrGroup, 0); cursor = std::prev(rrCursors.end()); }
            const auto* chosen = rr[cursor->second % rr.size()];
            cursor->second = (cursor->second + 1) % rr.size();
            return MappingSelection{ chosen->id, chosen->region.samplePath, chosen->region.rootKey, 1.0f };
        }
    }
    const auto* chosen = matches.front();
    return MappingSelection{ chosen->id, chosen->region.samplePath, chosen->region.rootKey, 1.0f };
}

void MappingEditorModel::resetRoundRobin()
{
    std::lock_guard lock(mutex);
    rrCursors.clear();
}
}
