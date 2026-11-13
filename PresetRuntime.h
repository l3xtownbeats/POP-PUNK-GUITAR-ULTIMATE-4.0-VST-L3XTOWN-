#pragma once
#include <memory>
#include <string>
#include "SamplePool.h"
#include "SamplerEngine.h"

namespace ppgu {

// Preset runtime: loads new preset sound on demand (one-by-one), unloads
// previous preset bank; no resident accumulation; FX Edit settings reset
// to preset defaults on switch (journal-approved behaviour).
class PresetRuntime {
public:
    struct PresetDef {
        std::string id;
        std::vector<std::string> sampleIds; // lazy: resolved on demand
    };
    explicit PresetRuntime(SamplePool& pool, SamplerEngine& engine)
        : pool_(pool), engine_(engine) {}

    // Loads preset: publishes new snapshot, retires old bank after quiescence.
    bool load(const PresetDef& def, std::shared_ptr<const SampleSnapshot> built);
    std::string currentId() const { return currentId_; }
private:
    SamplePool& pool_;
    SamplerEngine& engine_;
    std::string currentId_;
    uint64_t generation_ = 0;
};

} // namespace ppgu