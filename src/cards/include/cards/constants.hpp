// cards/constants.h

#pragma once

#include <cstdint>

namespace pho::cards {

constexpr uint32_t kSuitsPerDeck = 4u;
constexpr uint32_t kCardsPerSuit = 13u;
constexpr uint32_t kCardsPerHand = 13u;
constexpr uint32_t kCardsPerDeck = kSuitsPerDeck * kCardsPerSuit;
constexpr uint32_t kNumPlayers = 4u;
constexpr uint32_t kCardsPerTrick = 4u;

} // namespace pho::cards
