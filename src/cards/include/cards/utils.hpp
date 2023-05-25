#pragma once

#include "cards/CardSet.hpp"

namespace pho::cards {

Card aCardAtRandom(CardSet set);

CardSet chooseThreeAtRandom(CardSet dealt);

} // namespace pho::cards
