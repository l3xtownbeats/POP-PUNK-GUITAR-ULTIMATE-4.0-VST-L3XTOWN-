#include "DSP/FxOrder.h"

#include <array>
#include <iostream>

int main()
{
    using ppgu::dsp::resolveFxOrder;
    constexpr std::array<int, 7> canonical {{ 0, 1, 2, 3, 4, 5, 6 }};
    constexpr std::array<int, 7> reverse {{ 6, 5, 4, 3, 2, 1, 0 }};
    constexpr std::array<int, 7> mixed {{ 2, 0, 6, 4, 1, 5, 3 }};
    constexpr std::array<int, 7> duplicate {{ 0, 1, 2, 2, 4, 5, 6 }};
    constexpr std::array<int, 7> outOfRange {{ 0, 1, 2, 3, 4, 5, 7 }};

    if (resolveFxOrder(canonical) != canonical
        || resolveFxOrder(reverse) != reverse
        || resolveFxOrder(mixed) != mixed
        || resolveFxOrder(duplicate) != canonical
        || resolveFxOrder(outOfRange) != canonical)
    {
        std::cerr << "FX order permutation contract failed.\n";
        return 1;
    }
    std::cout << "FX order contracts passed.\n";
    return 0;
}
