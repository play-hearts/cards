#include "gstate/GState.hpp"
#include "cards/utils.hpp"
#include "prim/range.hpp"
#include "math/Int126.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::gstate
{

using math::RandomGenerator;

namespace {
auto actualDealIndex(uint128_t dealIndex) -> uint128_t
{
    if (dealIndex == GState::Init::kRandomDealIndex)
        dealIndex = Deal::randomDealIndex();
    return dealIndex;
}

auto actualPassOffset(uint8_t passOffset) -> uint8_t
{
    if (passOffset == GState::Init::kRandomPassOffset)
        passOffset = RandomGenerator::Range64(4u);
    return passOffset;
}
}

GameBehavior GState::kStandard = GameBehavior::make(GameVariant::standard);
GameBehavior GState::kJackDiamonds = GameBehavior::make(GameVariant::jack);
GameBehavior GState::kSpades = GameBehavior::make(GameVariant::spades);

GState::Init GState::Init::kRandom = GState::Init{GState::Init::kRandomDealIndex, GState::Init::kRandomPassOffset};
GState::Init GState::Init::kNoPass = GState::Init{GState::Init::kRandomDealIndex, uint8_t{0}};

GState::Init GState::kRandom = GState::Init::kRandom;
GState::Init GState::kNoPass = GState::Init::kNoPass;

GState::GState(const cards::Deal& deal, uint8_t passOffset, GameBehavior behavior)
: mDealIndex{deal.dealIndex()}
, mBehavior(behavior)
, mUnplayedCards{CardSet::fullDeck()}
, mHands{deal.hands()}
, mPassed{}
, mTaken{}
, mPlayerVoids{}
, mPassOffset{actualPassOffset(passOffset)}
, mPlayIndex{}
, mCurrent{}
, mAllTaken{}
, mTrick{}
, mPriorTrick{}
, mPassingComplete{}
{}

GState::GState(Init init, GameBehavior behavior)
: GState{Deal{actualDealIndex(init.dealIndex)}, actualPassOffset(init.passOffset), behavior}
{}

#if __EMSCRIPTEN__
GState::GState(const emscripten::val& init, GameVariant variant)
: GState{Init::fromVal(init), GameBehavior::make(variant)}
{}
#endif


auto GState::setPassFor(PlayerNum p, CardSet pass) -> void
{
    assert(mPassOffset != 0);
    assert(pass.size() == 3);
    assert(mHands.at(p).size() == 13);
    assert(pass.setIntersection(mHands.at(p)).size() == 3);
    mPassed.at(p) = pass;
}

auto GState::startGameNoPass() -> void
{
    assert(mPassOffset == 0);
    startGame();
}

auto totalCards(const FourHands& hands) -> uint32_t
{
    return hands.at(0).size() + hands.at(1).size() + hands.at(2).size() + hands.at(3).size();
}

auto GState::startGame() -> void
{
    assert(mPlayIndex == 0u);
    assert(!mPassingComplete);
    assert(mPassOffset < 4u);
    if (mPassOffset == 0)
    {
        assert(totalCards(mPassed) == 0);
    }
    else
    {
        assert(totalCards(mPassed) == 12);
        for (auto p : prim::range(kNumPlayers))
        {
            assert(mHands.at(p).size() == 13);
            mHands.at(p) -= mPassed.at(p);
            assert(mHands.at(p).size() == 10);
        }
        for (auto p : prim::range(kNumPlayers))
        {
            auto passTo = (p+mPassOffset)%4;
            mHands.at(passTo) += mPassed.at(p);
            assert(mHands.at(passTo).size() == 13);
        }
    }
    mPassingComplete = true;

    mCurrent = mBehavior.firstLead(*this);

    mUnplayedCards = CardSet::fullDeck();
    mAllTaken = CardSet{};
    mTrick.resetTrick(mCurrent);
}

auto GState::finishTrick() -> void
{
    auto winner = mTrick.winner();
    assert(mCardsPlayed.at(winner).hasCard(mTrick.at(winner)));
    for (auto i : prim::range(kCardsPerTrick))
    {
        auto card = mTrick.at(i);
        assert(card.ord() != Card::kNone);
        assert(mCardsPlayed.at(i).hasCard(card));
        mTaken.at(winner) += card;
        mAllTaken += card;
    }
    std::swap(mTrick, mPriorTrick);
    mTrick.resetTrick(winner);
    mCurrent = winner;
}

auto GState::playCard(Card card) -> void
{
    auto player = currentPlayer();
    assert(mHands.at(player).hasCard(card));

    if (!legalPlays().hasCard(card))
        throw std::invalid_argument(fmt::format("Card {} is not a legal play ({})", nameOfCard(card), to_string(legalPlays())));
    assert(mUnplayedCards.hasCard(card));

    if (playInTrick() != 0 && suitOf(card) != trickSuit())
        mPlayerVoids.setIsVoid(player, trickSuit());

    mHands.at(player) -= card;
    mCardsPlayed.at(player) += card;
    mTrick[player] = card;
    ++mPlayIndex;
    ++mCurrent;
    mCurrent = mCurrent % kNumPlayers;

    mUnplayedCards -= card;
    assert(mUnplayedCards.size() + mPlayIndex == kCardsPerDeck);

    if (playInTrick() == 0) finishTrick();
}

auto GState::trickSuit() const -> cards::Suit
{
    assert(playInTrick() != 0);
    return mTrick.trickSuit();
}

auto GState::getVariantOutcomeRep() const -> VariantOutcomeRep
{
    auto outcome = VariantOutcomeRep{};
    switch(mBehavior.variant())
    {
        case GameVariant::standard:
        {
            outcome = getStandardOutcome();
            break;
        }
        case GameVariant::jack:
        {
            outcome = getJackOutcome();
            break;
        }
        case GameVariant::spades:
        {
            outcome = getSpadesOutcome();
            break;
        }
        default:
        {
            assert(false);
            throw std::runtime_error("Bad behavior variant");
        }
    }
    return outcome;
}

constexpr auto kNoOne = kNumPlayers;
constexpr auto kExpectedTotalHearts = 13;

auto GState::getStandardOutcome() const -> VariantOutcome::Standard
{
    assert(mBehavior.variant() == GameVariant::standard);
    auto outcome = VariantOutcome::Standard{};
    getStandardOutcome(outcome);
    return outcome;
}

void GState::getStandardOutcome(VariantOutcome::Standard& outcome) const
{
    outcome.mTookQueen = kNoOne;
    for (auto p : prim::range(kNumPlayers))
    {
        auto taken = takenBy(p);
        outcome.mHeartsTaken[p] = taken.cardsWithSuit(Suit::kHearts).size();
        if (taken.hasCard(cards::cardFor(cards::Suit::kSpades, cards::Rank::kQueen)))
        {
            outcome.mTookQueen = p;
            if (outcome.mHeartsTaken[p] == kExpectedTotalHearts)
                outcome.mShooter = p;
        }
    }
    assert(outcome.mTookQueen != kNoOne);
    for (auto p : prim::range(kNumPlayers))
    {
        if (outcome.mShooter == kNoOne)
        {
            outcome.mScores[p] = outcome.mHeartsTaken[p];
            if (outcome.mTookQueen == p)
                outcome.mScores[p] += 13;
            outcome.mScores[p] -= 6.5;
        }
        else
        {
            if (p == outcome.mShooter)
                outcome.mScores[p] = -19.5;
            else
                outcome.mScores[p] = 6.5;
        }
        outcome.mScores[p] /= 19.5;
    }
}

auto GState::getJackOutcome() const -> VariantOutcome::Jack
{
    assert(mBehavior.variant() == GameVariant::jack);
    auto outcome = VariantOutcome::Jack{};
    getStandardOutcome(outcome);

    auto tookJack = kNoOne;
    for (auto p : prim::range(kNumPlayers))
    {
        assert(outcome.mScores[p] >= -1.0);
        assert(outcome.mScores[p] <= 1.0);
        outcome.mScores[p] *= 19.5;
        outcome.mScores[p] += 6.5 - 4.0;
        if (takenBy(p).hasCard(cards::cardFor(cards::Suit::kDiamonds, cards::Rank::kJack)))
        {
            tookJack = p;
            outcome.mScores[p] -= 10;
        }
        outcome.mScores[p] /= 24.0;
    }

    outcome.mTookJack = tookJack;
    return outcome;
}

auto GState::getSpadesOutcome() const -> VariantOutcome::Spades
{
    assert(mBehavior.variant() == GameVariant::spades);
    auto outcome = VariantOutcome::Spades{};

    // In Spades, the score is derived from number of tricks taken and the bid.
    // The bid has a strong affect on the score the player can obtain, but it is irrelevant to the strategy
    // that a player should take during the hand (except for some variations of the game that we don't support).
    // This is because it is always in the player's best interest to take as many tricks as possible, as every
    // trick taken makes it harder for other players to make their bid.

    // So it is useful to create a model that just tries to take as many tricks as possible.
    // For that model, the scoring we return is just the number of tricks taken, scaled to the range [-1, 1]
    // (so that we can use tanh as the activation function in the neural network).

    // We also invert the score so that the low scores are better as that is the convention used by Hearts.
    // It is somewhat baked into the implementation of this library, and while we could add the flexibility to
    // to have the score inverted or not, it is not worth the effort at this time.

    // Finally, when possible we prefer zero-sum scores. That is, the sum of the scores for all players should
    // be zero. We can easily do that when not accounting for bidding.
    // The odd formula below does that. When a player takes all 52 cards they will be assigned a score of -1.
    // and the other three playes will be assigned a score of 1/3.
    // The closest a round can come to a tie is when one player takes 16 cards and the other three take 12 each.
    // In that case, the scores will be -1/13, 1/39, 1/39, 1/39.
    for (auto p : prim::range(kNumPlayers))
    {
        outcome.mScores[p] = (13.0 - takenBy(p).size()) / 39.0;
    }

    // Now we can adjust the scores based on the bids.
    // We use the convention that if no bids were set, then we just use the above scoring.
    // If the bids were set, then the scores we return are derived from the actual scores that would be used in the game,
    // with two modifications: 1) we negate the scores so that the low scores are better, and 2) we subtract the average
    // score from each player so that the scores are zero-sum.
    // We could also scale the scores to the range [-1, 1] but it seems undesirable to squeeze the possible absolute range
    // of scores from 0 to 130 down to the range [-1, 1], especially we we don't use a global mean to obtain zero-sum.
    constexpr auto kNoBids = std::array<Bid, kNumPlayers>{};
    if (mBids != kNoBids)
    {
        auto sum = 0.0;
        for (auto p : prim::range(kNumPlayers))
        {
            auto bid = mBids.at(p);
            auto tricksTaken = takenBy(p).size() / 4;
            if (tricksTaken >= bid)
            {
                outcome.mScores[p] = -1.0 * (10.0*bid + tricksTaken - bid);
            }
            else
            {
                outcome.mScores[p] = 0.0;
            }
            sum += outcome.mScores[p];
        }
        for (auto p : prim::range(kNumPlayers))
        {
            outcome.mScores[p] -= sum / 4.0;
        }
    }

    return outcome;
}


auto GState::adjustPassedState() -> void
{
    const auto kCarl = currentPlayer();
    const auto kAlan = (kCarl + mPassOffset) % 4u;
    const auto kBetty = (kCarl + 4 - mPassOffset) % 4u;

    for (auto p : prim::range(kNumPlayers))
    {
        const auto passedTo = (p + mPassOffset) % 4;
        const auto receivedFrom = (p + 4 - mPassOffset) % 4;
        (void) passedTo;

        auto handAtStart = mHands.at(p) +  mCardsPlayed.at(p);
        // fmt::print(stderr, "mHands.at(p) = {}\n", to_string(mHands.at(p)));
        // fmt::print(stderr, "mCardsPlayed.at(p) = {}\n", to_string(mCardsPlayed.at(p)));
        // fmt::print(stderr, "handAtStart.size() = {}\n", handAtStart.size());
        assert(handAtStart.size() == kCardsPerHand);

        // Player p passed to the player `passedTo`
        // and received from the player `receivedFrom`.
        // Their hand right after passing was the `handAtStart`.
        // That hand must include three cards that player received
        // from the player `receivedFrom`.

        auto received = CardSet{};
        if (receivedFrom == kCarl)
        {
            (void) kAlan;
            assert(p == kAlan);
            received = mPassed.at(receivedFrom);
            assert((handAtStart & received) == received);
        }
        else if (receivedFrom == kBetty)
        {
            assert(p == kCarl);
            received = mPassed.at(receivedFrom);
            assert((handAtStart & received) == received);
        }
        else
        {
            assert(p!=kCarl && p!=kAlan);
            received = chooseThreeAtRandom(handAtStart); // not ideal, but should be good enough
            assert((handAtStart & received) == received);
        }

        // Normally we consider mPassed to be indexed such that
        // mPassed[passedFrom] = passed
        // But we can also think of it as being organized this way:
        mPassed.at(receivedFrom) = received;
    }
}

auto GState::alternate(const FourHands& hands) const -> GState
{
    auto alt{*this};

    alt.mDealIndex = ~uint128_t{0};
    alt.mHands = hands;

    assert(mPassingComplete);
    if (mPassOffset != 0)
    {
        alt.adjustPassedState();
    }

    return alt;
}

PlayerVoids GState::voidsForOthers() const
{
    // Here is where we must ensure that when there are no cards remaining for a suit, that
    // all three players are  marked void in the void bits
    const auto kCarl = currentPlayer();
    auto voidBits = PlayerVoids{mPlayerVoids.OthersKnownVoid(kCarl)};

    // These unknown cards do NOT include cards that Carl passed to Alan.
    // So if there are no unknown cards for a given suit we cannot
    // conclude that Alan is void in that suit.
    // We must check whether mPassed has any cards of that suit.
    const CardSet unknown = mUnplayedCards - mHands.at(kCarl) - mPassed.at(kCarl);

    for (const auto suit: allSuits)
    {
        if (unknown.cardsWithSuit(suit).empty())
        {
            voidBits.setAllOthersVoid(suit, kCarl);
            if (!mPassed.at(kCarl).cardsWithSuit(suit).empty())
            {
                voidBits.clearIsVoid(currentPassedTo(), suit);
            }
        }
        else
        {
            assert(voidBits.CountVoidInSuit(suit) < 3);
        }
    }

    return voidBits;
}

#define INFO(exp) fmt::print(stderr, "{}: {}\n", #exp, exp)

auto GState::asProbabilities() const -> ProbArray
{
    // There is a bug here somewhere related to the case where the last unknown card
    // of a suit has just been played. That player should be marked void in the suit.
    // And if either of the other two players had not yet been forced to declare they were
    // void, then they should also be declared void.
    //
    // Note that Carl can make these inferences since they have all of the
    // remaining cards of the suit. But the other players may not be able to make
    // the same inference.

    auto prob = ProbArray{};

    const unsigned carl = currentPlayer();

    const CardSet hand = currentPlayersHand();
    const CardSet passed = mPassed.at(carl) & mUnplayedCards;
    const CardSet unknown = mUnplayedCards - hand - passed;

    CardSet onTable;
    for (auto card : mTrick)
    {
        if (!(card == kNoCard))
            onTable += card;
    }

    auto passedCardsAreNotInHand = hand.setIntersection(passed).empty();
    auto noUnknownCardsOnTable = unknown.setIntersection(onTable).empty();
    auto noUnknownCardsWerePassed = unknown.setIntersection(passed).empty();

    auto unknownSize = unknown.size();
    auto playNumber = playIndex();
    auto currentPlayersHandSize = hand.size();
    auto passedSize = passed.size();
    auto sizesCheck = unknownSize + playNumber + currentPlayersHandSize + passedSize == kCardsPerDeck;
    auto allChecksGood = passedCardsAreNotInHand && noUnknownCardsOnTable && noUnknownCardsWerePassed && sizesCheck;
    if (!allChecksGood)
    {
        fmt::print("unknown: {}\n", to_string(unknown));
        fmt::print("hand: {}\n", to_string(hand));
        fmt::print("passed: {}\n", to_string(passed));
        fmt::print("onTable: {}\n", to_string(onTable));
        fmt::print("{} {} {} {}: {}\n", unknownSize, playNumber, currentPlayersHandSize, passedSize,
            unknownSize + playNumber + currentPlayersHandSize + passedSize);
        INFO(unknownSize);
        INFO(playNumber);
        INFO(currentPlayersHandSize);
        INFO(passedSize);
        INFO(unknownSize + playNumber + currentPlayersHandSize + passedSize);
        INFO(passedCardsAreNotInHand);
        INFO(noUnknownCardsOnTable);
        INFO(noUnknownCardsWerePassed);
        INFO(sizesCheck);
        assert(allChecksGood);
    }

    PlayerVoids voids = voidsForOthers();

    auto suitProb = ProbRow{};
    for (Suit suit : allSuits)
    {
        unsigned remaining = unknown.cardsWithSuit(suit).size();
        if (remaining == 0)
        {
            suitProb[Nib(suit)] = 0.0;
        }
        else
        {
            unsigned numVoid = voids.CountVoidInSuit(suit);
            assert(numVoid < 3);
            suitProb[Nib(suit)] = 1.0 / (3 - numVoid);
        }
    }

    for (auto p : prim::range(kNumPlayers))
    {
        if (p == carl)
        {
            for (Card card : currentPlayersHand())
            {
                prob[card.ord()][p] = 1.0;
            }
        }
        else
        {
            {
                for (Card card : unknown)
                {
                    assert(!passed.hasCard(card));
                    Suit suit = suitOf(card);
                    if (voids.isVoid(p, suit))
                    {
                        assert(prob[card.ord()][p] == 0.0);
                    }
                    else
                    {
                        prob[card.ord()][p] = suitProb[Nib(suit)];
                    }
                }
            }
            if (p == currentPassedTo())
            {
                for (Card card : passed)
                {
                    assert(prob[card.ord()][p] == 0.0);
                    prob[card.ord()][p] = 1.0;
                }
            }
        }
    }

    return prob;
}

#if __EMSCRIPTEN__
using namespace emscripten;

auto getDealIndex(const GState& state) -> emscripten::val
{
    auto index = state.dealIndex();
    return math::uint128_to_val(index);
}

EMSCRIPTEN_BINDINGS(GState) {
    value_object<GState::PlayerOutcome>("PlayerOutcome")
        .field("zms", &GState::PlayerOutcome::zms)
        .field("winPts", &GState::PlayerOutcome::winPts)
        ;

    class_<GState>("GState")
        .constructor<emscripten::val, GameVariant>()
        .function("passOffset", &GState::passOffset)
        .function("playersHand", &GState::playersHand)
        .function("currentPlayersHand", &GState::currentPlayersHand)
        .function("legalPlays", &GState::legalPlays)
        .function("setPassFor", &GState::setPassFor)
        .function("startGame", &GState::startGame)
        .function("playCard", &GState::playCard)
        .function("done", &GState::done)
        .function("getPlayerOutcome", &GState::getPlayerOutcome)
        .function("trickSuit", &GState::trickSuit)
        ;

    function("getDealIndex", &getDealIndex);

    class_<GState::Init>("GStateInit")
        .function("toVal", &GState::Init::toVal)
        ;

    function("kNoPassVal", &GState::Init::kNoPassVal);
    function("kRandomVal", &GState::Init::kRandomVal);
    function("fromVal", &GState::Init::fromVal);
    function("fromIndexAndOffset", &GState::Init::fromIndexAndOffset);

}
#endif

} // pho::gstate
