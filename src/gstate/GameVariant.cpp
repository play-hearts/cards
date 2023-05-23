#include "gstate/GameVariant.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::gstate {

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
