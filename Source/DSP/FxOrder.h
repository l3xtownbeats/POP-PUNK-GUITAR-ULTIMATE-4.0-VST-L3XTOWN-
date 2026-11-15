#pragma once

#include <array>
#include <cstddef>

namespace ppgu::dsp
{
inline constexpr std::array<int, 7> defaultFxOrder {{ 0, 1, 2, 3, 4, 5, 6 }};

// A seven-slot FX order must be a permutation. Invalid or temporarily
// duplicated APVTS values fail closed to the canonical order so automation
// cannot accidentally omit or double-run a processor stage.
inline std::array<int, 7> resolveFxOrder(const std::array<int, 7>& requested) noexcept
{
    std::array<bool, 7> seen {};
    for (const int stage : requested)
    {
        if (stage < 0 || stage >= static_cast<int>(seen.size())
            || seen[static_cast<std::size_t>(stage)])
            return defaultFxOrder;
        seen[static_cast<std::size_t>(stage)] = true;
    }
    return requested;
}
} // namespace ppgu::dsp
