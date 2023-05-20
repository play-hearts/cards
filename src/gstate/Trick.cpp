#include "gstate/Trick.hpp"
#include "prim/range.hpp"

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

auto Trick::trickSuit() const -> cards::Suit
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
    auto rank = pho::cards::Rank{};
    for (auto c : mRep)
    {
        if (c!=Card::kNone && suit == suitOf(c) && rank<rankOf(c))
        {
            rank = rankOf(c);
        }
    }
    return cardFor(suit, rank);
}

} // namespace pho::gstate
