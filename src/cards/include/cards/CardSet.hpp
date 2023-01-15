// cards/CardSet.h

#pragma once

#include <assert.h>
#include <stdint.h>

#include "cards/Card.hpp"
#include "math/Bits.hpp"

namespace pho::cards {

struct CoreCardSetConstants
{
    using BitSetMask = uint64_t;
    static constexpr BitSetMask kOne{1ul};

    static constexpr BitSetMask maskOfSuit(Suit suit) { return BitSetMask{((kOne << kCardsPerSuit) - 1) << (suit * kCardsPerSuit)}; }
    static constexpr BitSetMask maskOf(Card card) { return BitSetMask{kOne << card}; }
    static constexpr BitSetMask maskOf(Suit suit, Rank rank) { return BitSetMask{kOne << (suit * kCardsPerSuit + rank)}; }

    static constexpr BitSetMask kNoCards = BitSetMask{0};
    static constexpr BitSetMask kAllCards = BitSetMask{(kOne << kCardsPerDeck) - 1};
    static constexpr BitSetMask kInvalidCards = BitSetMask{~kAllCards};
};

/// @brief CardSet: a set of zero or more cards from a 52-card deck
/// Most of thd methods of this class are const. An immutable functional style may be used
/// A few operator overloads can modify the CardSet in place but they are the only mutable methods.
class CardSet : public CoreCardSetConstants
{
public:
    constexpr CardSet(BitSetMask bits = kNoCards)
    : mCardBits(bits)
    {
        assert((mCardBits & kInvalidCards) == kNoCards);
    }

    static constexpr CardSet make(const std::initializer_list<Card>& cards)
    {
        CardSet result;
        for (auto card: cards)
            result += card;
        return result;
    }

    // ---- value semantics
    CardSet(const CardSet& other) = default;
    CardSet(CardSet&&) = default;
    CardSet& operator=(const CardSet& other) = default;
    CardSet& operator=(CardSet&&) = default;

    // ---- factory methods
    [[nodiscard]] static constexpr CardSet fullDeck() {return CardSet(kAllCards); }

public:
    // ---- immutable/functional methods

    [[nodiscard]] BitSetMask asBits() const { return mCardBits; }

    [[nodiscard]] auto operator==(const CardSet& other) const { return mCardBits == other.mCardBits; }
    [[nodiscard]] auto operator!=(const CardSet& other) const { return mCardBits != other.mCardBits; }

    [[nodiscard]] auto size() const { return math::countBits(mCardBits); }
    [[nodiscard]] auto empty() const { return mCardBits == kNoCards;}

    [[nodiscard]] CardSet setUnion(CardSet other) const { return CardSet{ mCardBits | other.mCardBits }; }
    [[nodiscard]] CardSet setIntersection(CardSet other) const { return CardSet{ mCardBits & other.mCardBits }; }
    [[nodiscard]] CardSet setNegation() const { return CardSet(~mCardBits & kAllCards); }
    [[nodiscard]] CardSet setSubtract(CardSet other) const
    {
        auto inBoth = other.setIntersection(*this);
        return CardSet{mCardBits & ~inBoth.mCardBits};
    }

    /// @brief check if this CardSet has all cards in the given mask.
    /// @param mask a set of cards in the BitSetMask representation
    /// @return true if all cards in the mask are present
    [[nodiscard]] bool hasCards(BitSetMask mask) const { return (mCardBits & mask) == mask; }

    [[nodiscard]] bool hasCard(Card card) const { return hasCards(maskOf(card)); }
    [[nodiscard]] bool hasCard(Suit suit, Rank rank) const { return hasCards(maskOf(suit, rank)); }

    [[nodiscard]] Card front() const
    {
        assert(!empty());
        return math::leastSetBitIndex(mCardBits);
    }

    [[nodiscard]] Card back() const
    {
        assert(!empty());
        return math::greatestSetBitIndex(mCardBits);
    }

    [[nodiscard]] Card nthCard(unsigned n) const
    {
        assert(n < size());
        auto it = begin();
        while (n > 0)
        {
            ++it;
            --n;
        }
        return *it;
    }

    /// @brief Create the subset of cards of the given suit
    /// @param suit the suit of interest
    /// @return the new CardSet containing only cards of the given suit from this CardSet.
    [[nodiscard]] CardSet cardsWithSuit(Suit suit) const { return CardSet{mCardBits & maskOfSuit(suit)}; }

    /// @brief Create the subset of cards NOT of the given suit
    /// @param suit the suit to exclude
    /// @return the new CardSet excluding cards of the given suit from this CardSet.
    [[nodiscard]] CardSet cardsNotWithSuit(Suit suit) const  { return CardSet{mCardBits & ~maskOfSuit(suit)}; }

    // ---- friend free functions

    // Should a set union be presented using the + operator or the | operator, or both?
    // We supply both, but do so only because it is not possible to provide all desired operations
    // with just one family of operators.
    // For example, a set union can be represented well as either a addition or a logical or,
    // but it seems unnatural to represent a set intersection with multiplication.
    // Set difference can be represented using subtraction, but what would be the logical operator?
    // Finally, set negation could be done with a unary minus, but that seems error prone, so we provide just
    // unary ~ operator.

    [[nodiscard]] friend CardSet operator+(CardSet a, CardSet b) { return a.setUnion(b); }
    [[nodiscard]] friend CardSet operator|(CardSet a, CardSet b) { return a.setUnion(b); }
    [[nodiscard]] friend CardSet operator&(CardSet a, CardSet b) { return a.setIntersection(b); }
    [[nodiscard]] friend CardSet operator-(CardSet a, CardSet b) { return a.setSubtract(b); }
    [[nodiscard]] friend CardSet operator~(CardSet a) { return a.setNegation(); }

public:
    // ---- mutable methods ----

    /// @brief Add one card to this set. No-op if the card already exists.
    /// @param card
    /// @return
    CardSet& operator+=(Card card)
    {
        mCardBits |= maskOf(card);
        return *this;
    }

    /// @brief Remove one card from this set. No-op if the card does not exist.
    /// @param card the card to remove.
    /// @return a reference to this set
    CardSet& operator-=(Card card)
    {
        mCardBits &= ~maskOf(card);
        return *this;
    }

    /// @brief Add the cards of the other set to this set (union in place)
    /// @param other
    /// @return a reference to this (modified) set
    CardSet& operator+=(CardSet other)
    {
        mCardBits |= other.mCardBits;
        return *this;
    }

    CardSet& operator|=(CardSet other) { return *this += other; }

    /// @brief Subtract any cards in common with the other set from this set (setSubtract)
    /// @param other
    /// @return a reference to this (modified) set
    CardSet& operator-=(CardSet other)
    {
        auto inBoth = other.setIntersection(*this);
        mCardBits &= ~inBoth.mCardBits;
        return *this;
    }

    // ---- iteration ----

    class iterator
    {
    private:
        friend CardSet;

        iterator(CardSet cards) : mCardBits(cards.mCardBits) {}

        bool done() const { return mCardBits == kNoCards; }

    public:

        bool operator!=(iterator const& other) const
        {
            return mCardBits != other.mCardBits;
        }

        Card operator*() const
        {
            assert(!done());
            return math::leastSetBitIndex(mCardBits);
        }

        iterator& operator++()
        {
            assert(!done());
            Card card = math::leastSetBitIndex(mCardBits);
            mCardBits &= ~maskOf(card);
            return *this;
        }

    private:
        BitSetMask mCardBits;
    };

    iterator begin() const { return iterator(*this); }
    iterator end() const { return iterator(CardSet{}); }

// ---- Rendering

    friend std::string to_string(CardSet cards);

    friend std::ostream& operator<<(std::ostream& os, CardSet cards) { return os << to_string(cards); }

protected:
    // we represent this set of cards using a bit vector.
    BitSetMask mCardBits;
};

} // namespace pho::cards
