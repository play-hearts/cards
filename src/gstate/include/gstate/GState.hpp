#pragma once

#include "cards/CardSet.hpp"
#include "cards/Deal.hpp"
#include "cards/constants.hpp"

#include "cards/FourHands.hpp"
#include "gstate/GameBehavior.hpp"
#include "gstate/PlayerVoids.hpp"
#include "gstate/Trick.hpp"

#include "prim/range.hpp"

#include <fmt/format.h>
#include <stdexcept>

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif

namespace pho::hearts {
class KState;
}

namespace pho::gstate {

using namespace pho::cards;
using uint128_t = __uint128_t;

struct CardSetConstants : public CoreCardSetConstants
{
    // We split CoreCardSetConstants and CardSetConstants to enable use of maskOf() and maskOfSuit() to define more
    // constants. Placing them in the same struct would lead to `maskOf(...)’ called in a constant expression before its
    // definition is complete`
    static constexpr BitSetMask kAllHeartsMask = maskOfSuit(kHearts);
    static constexpr BitSetMask kNonHeartsMask = ~kAllHeartsMask;
    static constexpr BitSetMask kPointCardsMask = BitSetMask{kAllHeartsMask | maskOf(kSpades, kQueen)};
    static constexpr BitSetMask kNonPointCardsMask = BitSetMask{~kPointCardsMask & kAllCards};
};

constexpr auto kPointCards = CardSet{CardSetConstants::kPointCardsMask};
constexpr auto kNonPointCards = CardSet{CardSetConstants::kNonPointCardsMask};

using PlayerNum = uint32_t; // 0..3
using PlayIndex = uint32_t; // 0..52 (52 -> game over)
using DealIndex = uint128_t;
using PassOffset = uint8_t;

struct GStateInit
{
    std::string dealHexStr;
    PassOffset passOffset;
};

class GState
{
public:
    struct Init
    {
        static constexpr DealIndex kRandomDealIndex = ~uint128_t{0} >> 2;
        static constexpr PassOffset kRandomPassOffset = ~uint8_t{0};

        const DealIndex dealIndex;
        const PassOffset passOffset;

        Init(const GStateInit& init)
        : dealIndex{math::parseHex128(init.dealHexStr)}
        , passOffset{init.passOffset}
        { }

        constexpr Init(DealIndex dealIndex, PassOffset passOffset)
        : dealIndex{dealIndex}
        , passOffset{passOffset}
        { }

        constexpr Init(DealIndex dealIndex)
        : dealIndex{dealIndex}
        , passOffset{kRandomPassOffset}
        { }

        constexpr Init(PassOffset passOffset)
        : dealIndex{kRandomDealIndex}
        , passOffset{passOffset}
        { }

        constexpr Init()
        : dealIndex{kRandomDealIndex}
        , passOffset{kRandomPassOffset}
        { }

        constexpr auto operator==(const Init& other) const -> bool
        {
            return dealIndex == other.dealIndex && passOffset == other.passOffset;
        }

        constexpr auto operator!=(const Init& other) const -> bool { return !(*this == other); }

#if __EMSCRIPTEN__
        auto toVal() const -> GStateInit
        {
            return GStateInit{.dealHexStr = math::asHexString(dealIndex), .passOffset = passOffset};
        }

        static auto kNoPassVal() -> GStateInit;

        static auto kRandomVal() -> GStateInit;
#endif

        static Init kRandom;
        static Init kNoPass;
    };

    static Init kRandom;
    static Init kNoPass;

    static GameBehavior kStandard;
    static GameBehavior kJackDiamonds;
    static GameBehavior kSpades;

    GState(Init init = kNoPass, GameBehavior behavior = kStandard);
    GState(const cards::Deal& deal, PassOffset passOffset = Init::kRandomPassOffset, GameBehavior behavior = kStandard);
    GState(const GState&) = default;

#if __EMSCRIPTEN__
    GState(const GStateInit& init, GameVariant variant);
#endif

    auto gameStarted() const -> bool { return mPassingComplete; }

    auto setPassFor(PlayerNum p, CardSet pass) -> void;

    auto passOffset() const -> PassOffset { return mPassOffset; }

    auto startGame() -> void;
    auto startGameNoPass() -> void;

    // Return the index 0..52 for the current play.
    // 0 means the first card (the two of clubs) is to be played.
    // 52 means all cards have been played (the game is over).
    auto playIndex() const -> PlayIndex { return mPlayIndex; }

    static constexpr auto kPlaysPerTrick = kNumPlayers;
    auto trickLead() const -> PlayerNum { return mTrick.lead(); }
    auto playInTrick() const -> uint32_t { return mPlayIndex % kPlaysPerTrick; }

    // The "current" player is the player whose turn it is to play a card.
    // We sometimes call this player "Carl"
    auto currentPlayer() const -> PlayerNum { return mCurrent; }

    auto playersHand(PlayerNum p) const -> CardSet { return mHands.at(p); }
    auto currentPlayersHand() const -> CardSet { return playersHand(currentPlayer()); }

    // Return the set of cards that the current play can play (as allowed by the rules)
    auto legalPlays() const -> CardSet { return mBehavior.legal(*this); }

    // Play the card (must be legal) and advance the game state to the next player.
    auto playCard(Card card) -> void;

    // Return true when all cards have been played.
    auto done() const -> bool { return mPlayIndex == kCardsPerDeck; }

    // Return the player number of the player that the current player passed to (or will pass to)
    // at the beginning of the game. Will return the current player's own player number when cards were held
    // as dealt.
    auto currentPassedTo() const -> PlayerNum { return (currentPlayer() + mPassOffset) % kNumPlayers; }

    // Return the player number of the player that the current player receieved from (or will receive from)
    // at the beginning of the game. Will return the current player's own player number when cards were held
    // as dealt.
    auto currentReceivedFrom() const -> PlayerNum
    {
        return (currentPlayer() - mPassOffset + kNumPlayers) % kNumPlayers;
    }

    auto unplayedCards() const -> CardSet { return mUnplayedCards; }

    auto passedBy(PlayerNum player) const -> CardSet { return mPassed.at(player); }

    auto hands() const -> const FourHands& { return mHands; }

    auto takenBy(PlayerNum player) const -> CardSet { return mTaken.at(player); }

    using ProbRow = std::array<float, kNumPlayers>;
    using ProbArray = std::array<ProbRow, kCardsPerDeck>;
    auto asProbabilities() const -> ProbArray;

    auto getTrickPlay(unsigned i) const -> Card { return mTrick.getTrickPlay(i); }

    auto highCardInTrick() const { return playInTrick() == 0 ? kNoCard : mTrick.highCard(); }

    auto voidsForOthers() const -> PlayerVoids;

    auto trickSuit() const -> Suit;

    auto allTaken() const -> CardSet { return mAllTaken; }

    auto behavior() const -> GameBehavior { return mBehavior; }

    auto taken() const -> FourHands { return mTaken; }

    auto getVariantOutcomeRep() const -> VariantOutcomeRep;
    void getStandardOutcome(VariantOutcome::Standard& outcome) const;
    auto getStandardOutcome() const -> VariantOutcome::Standard;
    auto getJackOutcome() const -> VariantOutcome::Jack;
    auto getSpadesOutcome() const -> VariantOutcome::Spades;

    struct PlayerOutcome
    {
        float zms;
        float winPts;
    };

    auto getPlayerOutcome(unsigned p) const -> PlayerOutcome
    {
        auto outcome = getVariantOutcomeRep();
        return std::visit(
            [p](auto&& arg) {
                return PlayerOutcome{arg.normalizedScore(p), arg.playerOutcomeResult(p)};
            },
            outcome);
    }

    using Bid = uint8_t;
    void setBid(PlayerNum p, Bid bid)
    {
        assert(bid > 0);
        assert(bid <= 13);
        mBids[p] = bid;
    }

    auto dealIndex() const -> DealIndex { return mDealIndex; }

    Trick currentTrick() const { return mTrick; }
    Trick priorTrick() const { return mPriorTrick; }

private:
    friend hearts::KState;
    auto alternate(const FourHands& hands) const -> GState;

    auto adjustPassedState() -> void;

private:
    auto finishTrick() -> void;

private:
    // The members used to represent the game state here are chosen to strke a good balance
    // between minimal redundancy and efficiency.
    // We prefer efficient updates of state during game play over efficient calculation of the game outcome.

    // The index be used to create the original deal (before passing).
    uint128_t mDealIndex;

    GameBehavior mBehavior;

    // The cards not yet played. Each card is removed as soon as it is placed on the table,
    // before the trick is completed.
    CardSet mUnplayedCards;

    // The cards currently still held for each player.
    // A card just placed on the table for the current trick is immediately removed from the hand.
    FourHands mHands;

    // The cards passed by each player. These cards are NOT in the respective player's hand,
    // but that player can remember they passed the card to the recipient player.
    FourHands mPassed;

    // The cards already played by each player. Updated immediately when card put on table.
    FourHands mCardsPlayed;

    // The cards each player has taken (i.e. from won tricks) so far in the game.
    FourHands mTaken;

    // A compact representation (bitset of 4x4 bits) of which players have revealed that they are void per suit
    PlayerVoids mPlayerVoids;

    // The passing direction offset. 0 -> hold. 1 -> pass left. 2 -> pass across. 3 -> pass right
    PassOffset mPassOffset;

    // This is the number 0..51 of the *next* play.
    // When the play index is zero passing may still be in progress.
    // When the play index is 52 the game is finished.
    PlayIndex mPlayIndex;

    // The player whose turn it is.
    PlayerNum mCurrent;

    // The set of all cards taken off the table in prior tricks
    CardSet mAllTaken;

    // The cards currently on the table. This trick is never fully populated.
    Trick mTrick;

    // The just completed trick. Fully populated except during the first trick.
    Trick mPriorTrick;

    // True if all steps for the passing phase are complete.
    bool mPassingComplete;

    // The bids for each player. This is only used for the "spades" variant.
    std::array<Bid, kNumPlayers> mBids;
};

} // namespace pho::gstate
