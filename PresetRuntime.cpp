#include "PresetRuntime.h"

namespace ppgu {

bool PresetRuntime::load(const PresetDef& def, std::shared_ptr<const SampleSnapshot> built) {
    if (!built) return false;
    ++generation_;
    auto newSnap = std::make_shared<SampleSnapshot>(*built);
    newSnap->generation = generation_;
    if (!pool_.commit(newSnap)) return false;
    engine_.setSnapshot(pool_.current());
    uint64_t oldGen = generation_ - 1;
    if (oldGen > 0) pool_.retireOld(oldGen); // unload previous preset bank (quiescence-checked)
    currentId_ = def.id;
    return true;
}

} // namespace ppgu