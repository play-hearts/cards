#include "cards/Card.hpp"
#include "prim/range.hpp"
#include <assert.h>
#include <string>

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::cards {

static const std::string kSuits[] = {"♣️", "♦️", "♠️", "♥️"};
static const std::string kRanks[] = {" 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", " J", " Q", " K", " A"};

static std::string kNames[kCardsPerDeck];

static void InitializeCardNames()
{
    for (auto c : prim::range(kCardsPerDeck))
    {
        Card card(c);
        std::string s(kRanks[card.rankOrd()]);
        s += kSuits[card.suitOrd()];
        kNames[c] = s;
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
    assert(card.ord() < kCardsPerDeck);
    return kNames[card.ord()];
}

#if __EMSCRIPTEN__
using namespace emscripten;
EMSCRIPTEN_BINDINGS(cards) {
    class_<Card>("Card")
        .constructor<Ord>()
        .function("ord", &Card::ord)
        .function("suitOrd", &Card::suitOrd)
        .function("rankOrd", &Card::rankOrd)
        ;

    function("cardFrom", cardFrom);
    function("nameOfCard", nameOfCard);
}
#endif

} // namespace pho::cards
