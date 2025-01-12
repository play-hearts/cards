#pragma once

#include "cards/CardSet.hpp"

namespace pho::cards {

Card aCardAtRandom(CardSet set);

CardSet chooseSomeAtRandom(CardSet set, unsigned N);

inline CardSet chooseThreeAtRandom(CardSet dealt) { return chooseSomeAtRandom(dealt, 3); }

CardSet removeSomeAtRandom(CardSet& set, unsigned N);

} // namespace pho::cards
