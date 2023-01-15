#include "cards/Card.hpp"
#include "prim/range.hpp"
#include <assert.h>
#include <string>

namespace pho::cards {

static const std::string kSuits[] = {"♣️", "♦️", "♠️", "♥️"};
static const std::string kRanks[] = {" 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", " J", " Q", " K", " A"};

static std::string kNames[kCardsPerDeck];

static void InitializeCardNames()
{
    for (auto card : prim::range(kCardsPerDeck))
    {
        std::string s(kRanks[iRankOf(card)]);
        s += kSuits[iSuitOf(card)];
        kNames[card] = s;
    }
}

class Initializer
{
public:
    Initializer() { InitializeCardNames(); }
};

static Initializer gInitializer;

std::string nameOfSuit(Suit suit) { return kSuits[suit]; }

std::string nameOfCard(Card card)
{
    if (card == kNoCard)
        return "{}";
    assert(card < kCardsPerDeck);
    return kNames[card];
}

} // namespace pho::cards
