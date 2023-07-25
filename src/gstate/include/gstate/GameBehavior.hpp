#pragma once

#include "cards/CardSet.hpp"
#include "gstate/GameVariant.hpp"

#include <memory>

namespace pho::gstate {

class GState;
class Trick;

using CardSet = cards::CardSet;
using Card = cards::Card;
using Suit = cards::Suit;

class GameBehavior
{
public:
    using Variant = GameVariant;

    static auto make(Variant variant) -> GameBehavior;

    /// @brief return the set of cards in the current player's hand that are legal to play
    [[nodiscard]] auto legal(const GState& gameState) const -> CardSet { return mRep->legal(gameState); }

    /// @brief return the (fixed) set of cards that have point value for this game variant
    auto pointCards() const -> CardSet { return mRep->pointCards(); };

    /// @brief return the suit of the current trick
    /// In all variants the trick suit is the suit of the first card played in the trick.
    /// E.g. followers must play a card of this suit if they hold a card of the suit.
    [[nodiscard]] auto trickSuit(const Trick& trick) const -> Suit;

    /// @brief return the suit of the the trick that won the trick.
    /// In all current variants except spades this is the trickSuit.
    /// In spades this is the suit kSpades if a spades was played, otherwise it is the trickSuit.
    [[nodiscard]] auto trumpSuit(const Trick& trick) const -> Suit { return mRep->trumpSuit(trick); }

    auto trickWinner(const Trick& trick) const -> uint32_t { return mRep->trickWinner(trick); }

    auto highCard(const Trick& trick) const -> Card { return mRep->highCard(trick); }

    GameBehavior(const GameBehavior&) = default;
    GameBehavior(GameBehavior&&) = default;

    GameBehavior& operator=(const GameBehavior&) = default;
    GameBehavior& operator=(GameBehavior&&) = default;

    auto variant() const -> Variant { return mRep->mVariant; }

    auto firstLead(const GState& state) const -> uint32_t { return mRep->firstLead(state); }

private:
    struct Concept;

    using Rep = std::shared_ptr<Concept>;

    GameBehavior() = delete;
    GameBehavior(Rep rep)
    : mRep(rep)
    { }

    struct Concept
    {
        virtual ~Concept();

        Concept() = delete;
        Concept(Variant variant)
        : mVariant{variant}
        { }

        /// @brief return the (fixed) set of cards that have point value for this game variant
        // This is used to determine which cards are legal to lead with (and is currently not used for anything else).
        [[nodiscard]] virtual auto pointCards() const -> CardSet = 0;

        /// @brief return the set of cards in the current player's hand that are legal to play
        [[nodiscard]] virtual auto legal(const GState& gameState) const -> CardSet;

        /// @brief return the cards that are legal to lead with
        [[nodiscard]] auto legalLeadPlays(const GState& state) const -> CardSet;

        /// @brief return the cards that are legal to follow with
        [[nodiscard]] auto legalFollowPlays(const GState& state) const -> CardSet;

        [[nodiscard]] virtual auto trickWinner(const Trick& trick) const -> uint32_t;

        [[nodiscard]] auto highCard(const Trick& trick) const -> Card;

        [[nodiscard]] virtual auto firstLead(const GState& state) const -> uint32_t;

        [[nodiscard]] virtual auto trumpSuit(const Trick& trick) const -> Suit;

        Variant mVariant;
    };

    struct StandardHearts : public Concept
    {
        ~StandardHearts();
        StandardHearts()
        : Concept{standard}
        { }

        [[nodiscard]] auto pointCards() const -> CardSet override { return kAllPointCards; }

        using BitSetMask = uint64_t;
        static constexpr BitSetMask kAllHeartsMask = cards::CoreCardSetConstants::maskOfSuit(cards::kHearts);
        static constexpr BitSetMask kPointCardsMask
            = BitSetMask{kAllHeartsMask | cards::CoreCardSetConstants::maskOf(cards::kSpades, cards::kQueen)};
        static constexpr auto kAllPointCards = CardSet{kPointCardsMask};
    };

    struct JackDiamondsHearts : public Concept
    {
        ~JackDiamondsHearts();
        JackDiamondsHearts()
        : Concept{jack}
        { }

        [[nodiscard]] auto pointCards() const -> CardSet override { return kAllPointCards; }

        using BitSetMask = uint64_t;
        static constexpr BitSetMask kPointCardsMask
            = StandardHearts::kPointCardsMask
            | BitSetMask{cards::CoreCardSetConstants::maskOf(cards::kDiamonds, cards::kJack)};
        static constexpr auto kAllPointCards = CardSet{kPointCardsMask};
    };

    struct Spades : public Concept
    {
        ~Spades();
        Spades()
        : Concept{spades}
        { }

        [[nodiscard]] auto pointCards() const -> CardSet override { return kAllPointCards; }
        [[nodiscard]] auto trumpSuit(const Trick& trick) const -> Suit override;
        [[nodiscard]] auto trickWinner(const Trick& trick) const -> uint32_t override;
        [[nodiscard]] auto firstLead(const GState& state) const -> uint32_t override;

        using BitSetMask = uint64_t;
        static constexpr BitSetMask kPointCardsMask = cards::CoreCardSetConstants::maskOfSuit(cards::kSpades);
        static constexpr auto kAllPointCards = CardSet{kPointCardsMask};
    };

    Rep mRep;
};

class ActiveGameBehavior
{
public:
    ActiveGameBehavior(GameBehavior::Variant variant);

    static auto activeBehavior() -> GameBehavior;

private:
    ActiveGameBehavior() = delete;
    ActiveGameBehavior(const ActiveGameBehavior&) = delete;
    ActiveGameBehavior(ActiveGameBehavior&&) = delete;
    ActiveGameBehavior& operator=(const ActiveGameBehavior&) = delete;
    ActiveGameBehavior& operator=(ActiveGameBehavior&&) = delete;

private:
    static ActiveGameBehavior* gActive;
    GameBehavior mBehavior;
};

} // namespace pho::gstate
