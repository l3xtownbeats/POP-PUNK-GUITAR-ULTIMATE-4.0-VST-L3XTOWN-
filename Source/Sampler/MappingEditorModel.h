#pragma once

#include "SfzParser.h"
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <vector>

namespace ppgu::sampler
{
struct MappingRegion
{
    std::size_t id = 0;
    ppgu::sfz::Region region;
    int roundRobinGroup = 0;
};

struct MappingSelection
{
    std::size_t regionId = 0;
    std::filesystem::path samplePath;
    int rootKey = 60;
    float velocityGain = 1.0f;
};

// Thread-safe editable mapping data. Editing and selection happen off the audio
// callback; the audio engine can snapshot the immutable region vector.
class MappingEditorModel final
{
public:
    [[nodiscard]] bool replace(std::vector<MappingRegion> regions, std::string& error);
    [[nodiscard]] bool setKeyRange(std::size_t id, int low, int high, int root, std::string& error);
    [[nodiscard]] bool setVelocityRange(std::size_t id, int low, int high, std::string& error);
    [[nodiscard]] bool setRoundRobin(std::size_t id, int group, std::string& error);
    [[nodiscard]] std::vector<MappingRegion> snapshot() const;
    [[nodiscard]] std::optional<MappingSelection> select(int midiNote, int velocity);
    void resetRoundRobin();

private:
    mutable std::mutex mutex;
    std::vector<MappingRegion> regions;
    std::vector<std::pair<int, std::size_t>> rrCursors;
};
}
