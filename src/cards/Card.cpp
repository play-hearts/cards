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
        std::string s(kRanks[card.rank()]);
        s += kSuits[card.suit()];
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

#if USE_INT_SUIT_RANK
static_assert(std::is_same_v<Suit, uint8_t>);
static_assert(std::is_same_v<Rank, uint8_t>);
#endif

#if __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(Card)
{
    class_<Card>("Card")
        .constructor<Ord>()
        .function("ord", &Card::ord)
        .function("suit", &Card::suit)
        .function("rank", &Card::rank);

    function("cardFrom", cardFrom);
    function("nameOfCard", nameOfCard);

#if USE_INT_SUIT_RANK
    constant("kClubs", kClubs);
    constant("kDiamonds", kDiamonds);
    constant("kSpades", kSpades);
    constant("kHearts", kHearts);

    constant("kTwo", kTwo);
    constant("kThree", kThree);
    constant("kFour", kFour);
    constant("kFive", kFive);
    constant("kSix", kSix);
    constant("kSeven", kSeven);
    constant("kEight", kEight);
    constant("kNine", kNine);
    constant("kTen", kTen);
    constant("kJack", kJack);
    constant("kQueen", kQueen);
    constant("kKing", kKing);
    constant("kAce", kAce);
#else

    enum_<Suit>("Suit")
        .value("kClubs", kClubs)
        .value("kDiamonds", kDiamonds)
        .value("kSpades", kSpades)
        .value("kHearts", kHearts);

    enum_<Rank>("Rank")
        .value("kTwo", kTwo)
        .value("kThree", kThree)
        .value("kFour", kFour)
        .value("kFive", kFive)
        .value("kSix", kSix)
        .value("kSeven", kSeven)
        .value("kEight", kEight)
        .value("kNine", kNine)
        .value("kTen", kTen)
        .value("kJack", kJack)
        .value("kQueen", kQueen)
        .value("kKing", kKing)
        .value("kAce", kAce);

#endif
}
#endif

} // namespace pho::cards
