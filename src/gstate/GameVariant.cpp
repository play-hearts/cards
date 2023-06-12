#include "gstate/GameVariant.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::gstate {

auto VariantOutcome::Base::playerOutcomeResult(unsigned p) const -> float
{
    auto best = *std::min_element(mScores.begin(), mScores.end());
    if (mScores[p] > best)
        return 0.0;
    else
    {
        auto ties = std::count(mScores.begin(), mScores.end(), best);
        return 1.0f / ties;
    }
}


#if __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(GameVariant) {

enum_<GameVariant>("GameVariant")
    .value("STANDARD", standard)
    .value("JACK", jack)
    .value("SPADES", spades)
    ;
}

#endif

} // namespace pho::gstate
