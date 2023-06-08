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

std::string nameOfSuit(Suit suit) { return kSuits[Nib(suit)]; }

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

    enum_<Suit>("Suit")
        .value("kClubs", Suit::kClubs)
        .value("kDiamonds", Suit::kDiamonds)
        .value("kSpades", Suit::kSpades)
        .value("kHearts", Suit::kHearts)
        ;

    enum_<Rank>("Rank")
        .value("kTwo", Rank::kTwo)
        .value("kThree", Rank::kThree)
        .value("kFour", Rank::kFour)
        .value("kFive", Rank::kFive)
        .value("kSix", Rank::kSix)
        .value("kSeven", Rank::kSeven)
        .value("kEight", Rank::kEight)
        .value("kNine", Rank::kNine)
        .value("kTen", Rank::kTen)
        .value("kJack", Rank::kJack)
        .value("kQueen", Rank::kQueen)
        .value("kKing", Rank::kKing)
        .value("kAce", Rank::kAce)
        ;

    class_<Card>("Card")
        .constructor<Ord>()
        .function("ord", &Card::ord)
        .function("suit", &Card::suit)
        .function("rank", &Card::rank)
        ;

    function("suitOf", &pho::cards::suitOf);
    function("rankOf", &pho::cards::rankOf);
    function("cardFor", cardFor);
    function("nameOfSuit", nameOfSuit);
    function("nameOfCard", nameOfCard);
}
#endif

} // namespace pho::cards
