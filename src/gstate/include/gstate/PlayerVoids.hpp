#pragma once

#include "cards/Card.hpp"
#include "cards/CardSet.hpp"
#include "cards/CardHands.hpp"
#include "cards/FourHands.hpp"
#include <vector>

namespace pho::gstate {

using namespace pho::cards;

using FourHands = FourHands;

struct SuitVoids
{
    SuitVoids(Suit s, uint8_t n)
        : suit(s)
        , numVoids(n)
    {
        assert(n <= 3);
    }
    SuitVoids()
        : suit(kUnknown)
        , numVoids(0)
    {}
    Suit suit;
    uint8_t numVoids;

    // sorts in ascending order by numVoids
    // we will process in descending order, but using .pop_back()
    static bool compare(const SuitVoids& s1, const SuitVoids& s2) { return s1.numVoids < s2.numVoids; }
};

typedef std::vector<SuitVoids> PriorityList;

class PlayerVoids
{
public:
    ~PlayerVoids();

    using Rep = uint16_t;

    PlayerVoids(Rep bits = 0);

    PlayerVoids(const PlayerVoids& other);

    void operator=(const PlayerVoids& other) { mBits = other.mBits; }

    PriorityList MakePriorityList() const;

    bool isVoid(int player, Suit suit) const { return (mBits & VoidBit(player, suit)) != 0; }

    void setIsVoid(int player, Suit suit) { mBits |= VoidBit(player, suit); }

    void clearIsVoid(int player, Suit suit) { mBits &= ~VoidBit(player, suit); }

    void setAllOthersVoid(Suit suit, unsigned p);

    uint8_t CountVoidInSuit(Suit suit) const;

    void VerifyVoids(const FourHands& hands) const;

    bool operator==(const PlayerVoids& other) const { return mBits == other.mBits; }
    bool operator!=(const PlayerVoids& other) const { return mBits != other.mBits; }

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, const PlayerVoids& bits) {
        return os << bits.toString();
    }

    Rep OthersKnownVoid(int currentPlayer) const
    {
        // Here we mask out the bits for the current player. Since there is one nibble per Suit
        // we have to touch each of the four nibbles. The mask will be one of the values 0x1111, 0x2222, 0x4444, 0x8888.
        const Rep mask = playerMask(currentPlayer);
        return mBits & ~mask;
    }

private:
    static inline int VoidBit(int player, Suit suit)
    {
        // This scheme creates one nibble per suit, with each nibble containing one bit per player.
        return 1u << (4 * suit + player);
    }

    static Rep playerMask(int p)
    {
        return 0x1111 << p;
    }

    static Rep suitMask(Suit suit)
    {
        return 0x000F << (suit*4);
    }

private:
    Rep mBits;
};

} // namespace pho::gstate
