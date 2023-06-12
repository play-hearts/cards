#include "gstate/Trick.hpp"
#include "prim/range.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::gstate
{

auto Trick::getTrickPlay(unsigned i) const -> Card
{
    auto seat = (mLead+i) % kNumPlayers;
    auto play = mRep.at(seat);
    assert(play != Card::kNone);
    return play;
}

auto Trick::leadCard() const -> Card
{
    return getTrickPlay(0);
}

auto Trick::trickSuit() const -> Suit
{
    return suitOf(leadCard());
}

// Return the absolute seat that won the trick
auto Trick::winner() const -> uint32_t
{
    auto lead = leadCard();
    auto suit = suitOf(lead);
    auto rank = rankOf(lead);
    auto winner = 0xff;
    for (auto p : prim::range(kNumPlayers))
    {
        auto card = mRep.at(p);
        if (card!=Card::kNone && suit == suitOf(card) && rank<=rankOf(card))
        {
            rank = rankOf(card);
            winner = p;
        }
    }
    assert(winner != 0xff);
    return winner;
}

auto Trick::highCard() const -> Card
{
    auto suit = trickSuit();
    auto rank = Rank{};
    for (auto c : mRep)
    {
        if (c!=Card::kNone && suit == suitOf(c) && rank<rankOf(c))
        {
            rank = rankOf(c);
        }
    }
    return cardFor(suit, rank);
}

#if __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(Trick) {

    class_<Trick>("Trick")
        .constructor<>()
        .function("at", &Trick::at)
        .function("trickSuit", &Trick::trickSuit)
        .function("lead", &Trick::lead)
        .function("leadCard", &Trick::leadCard)
        .function("winner", &Trick::winner)
        .function("highCard", &Trick::highCard)
        .function("getTrickPlay", &Trick::getTrickPlay)
        .function("rep", &Trick::rep)
        ;

}

#endif

} // namespace pho::gstate
