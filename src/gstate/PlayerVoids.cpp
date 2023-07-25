#include "gstate/PlayerVoids.hpp"
#include "prim/range.hpp"

#include <algorithm>
#include <fmt/format.h>

namespace pho::gstate {

PlayerVoids::~PlayerVoids() { }

PlayerVoids::PlayerVoids(uint16_t bits)
: mBits(bits)
{ }

PlayerVoids::PlayerVoids(const PlayerVoids& other)
: mBits(other.mBits)
{ }

PriorityList PlayerVoids::MakePriorityList() const
{
    PriorityList prioList;
    for (Suit suit : allSuits)
    {
        uint8_t numVoids = CountVoidInSuit(suit);
        prioList.push_back(SuitVoids(suit, numVoids));
    }

    std::sort(prioList.begin(), prioList.end(), SuitVoids::compare);
    return prioList;
}

uint8_t PlayerVoids::CountVoidInSuit(Suit suit) const
{
    uint8_t count = 0;
    for (auto p : prim::range(kNumPlayers))
    {
        if (isVoid(p, suit))
            ++count;
    }
    return count;
}

void PlayerVoids::setAllOthersVoid(Suit suit, unsigned p)
{
    auto mask = suitMask(suit);
    mask &= ~playerMask(p);
    mBits |= mask;
    const auto numVoids = CountVoidInSuit(suit);
    if (numVoids != 3)
    {
        fmt::print(stderr, "Player:{}, Suit:{} Voids:{}, NumVoids: {}\n", p, nameOfSuit(suit), toString(), numVoids);
        fmt::print(
            stderr, "suitMask: {:016b}, playerMask:{:016b}, mBits:{:016b}\n", suitMask(suit), playerMask(p), mBits);
        assert(numVoids == 3);
    }
}

void PlayerVoids::VerifyVoids(const FourHands& hands) const
{
    (void)hands;
#ifndef NDEBUG
    for (auto p : prim::range(kNumPlayers))
    {
        const CardSet& hand = hands.at(p);
        for (Suit suit : allSuits)
        {
            if (isVoid(p, suit))
            {
                assert(hand.cardsWithSuit(suit).empty());
            }
        }
    }
#endif
}

std::string PlayerVoids::toString() const
{
    std::string result;

    const auto V = [this](unsigned p, Suit s) { return isVoid(p, s) ? nameOfSuit(s) : "."; };

    for (auto p : prim::range(kNumPlayers))
        result += fmt::format("{}{}{}{} ", V(p, kClubs), V(p, kDiamonds), V(p, kSpades), V(p, kHearts));

    return result;
}

} // namespace pho::gstate
