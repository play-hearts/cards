#include "cards/FourHands.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::cards {

auto FourHands::hand(int i) const -> CardSet { return mRep.at(i); }

#if __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(FourHands) { class_<FourHands>("FourHands").constructor<>().function("hand", &FourHands::hand); }
#endif

} // namespace pho::cards
