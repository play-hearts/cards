#include "gstate/GameBehavior.hpp"
#include "gstate/GState.hpp"
#include "math/random.hpp"

#include <mutex>
#include <stdexcept>

namespace pho::gstate {

auto GameBehavior::make(Variant variant) -> GameBehavior
{
    switch (variant)
    {
        case standard:
            return GameBehavior{std::make_shared<StandardHearts>()};
        case jack:
            return GameBehavior{std::make_shared<JackDiamondsHearts>()};
        case spades:
            return GameBehavior{std::make_shared<Spades>()};
        default:
            throw std::invalid_argument("Unrecognized game variant");
    }
}

auto GameBehavior::trickSuit(const pho::gstate::Trick& trick) const -> Suit { return trick.trickSuit(); }

GameBehavior::Concept::~Concept() { }
GameBehavior::StandardHearts::~StandardHearts() { }
GameBehavior::JackDiamondsHearts::~JackDiamondsHearts() { }
GameBehavior::Spades::~Spades() { }

auto GameBehavior::Concept::legalLeadPlays(const GState& state) const -> CardSet
{
    auto hand = state.currentPlayersHand();
    auto pointsTaken = state.allTaken() & pointCards();
    return pointsTaken.size() > 0 ? hand : hand & ~pointCards();
}

auto GameBehavior::Concept::legalFollowPlays(const GState& state) const -> CardSet
{
    auto hand = state.currentPlayersHand();
    return hand.cardsWithSuit(state.trickSuit());
}

auto GameBehavior::Concept::legal(const GState& state) const -> CardSet
{
    assert(state.gameStarted());

    if (state.playIndex() == 0 && mVariant != spades)
        return CardSet::make({Card::cardFor(kClubs, kTwo)});

    auto choices = state.playInTrick() == 0 ? legalLeadPlays(state) : legalFollowPlays(state);
    if (choices.empty())
        choices = state.currentPlayersHand();

    return choices;
}

auto GameBehavior::Concept::trumpSuit(const pho::gstate::Trick& trick) const -> Suit { return trick.trickSuit(); }
auto GameBehavior::Concept::trickWinner(const pho::gstate::Trick& trick) const -> uint32_t { return trick.winner(); }

auto GameBehavior::Concept::firstLead(const GState& state) const -> uint32_t
{
    static constexpr auto kTwoClubs = Card::cardFor(kClubs, kTwo);
    const auto& hands = state.hands();
    for (auto p : prim::range(kNumPlayers))
    {
        if (hands.at(p).hasCard(kTwoClubs))
        {
            return p;
        }
    }
    assert(false);
    return 0;
}

auto GameBehavior::Spades::trumpSuit(const pho::gstate::Trick& trick) const -> Suit
{
    auto suit = trick.trickSuit();
    if (suit != kSpades)
    {
        for (auto p : prim::range(kNumPlayers))
        {
            auto card = trick.at(p);
            assert(card != Card::kNone);
            if (suitOf(card) == kSpades)
            {
                suit = kSpades;
                break;
            }
        }
    }
    return suit;
}

auto GameBehavior::Spades::trickWinner(const pho::gstate::Trick& trick) const -> uint32_t
{
    auto suit = trumpSuit(trick);
    auto rank = kTwo;
    auto winner = 0xff;
    for (auto p : prim::range(kNumPlayers))
    {
        auto card = trick.at(p);
        assert(card != Card::kNone);
        if (suitOf(card) == suit && rank <= rankOf(card)) // must use <= here to catch case where 2 of spades is winner
        {
            rank = rankOf(card);
            winner = p;
        }
    }
    assert(winner != 0xff);
    return winner;
}

auto GameBehavior::Spades::firstLead(const GState& state) const -> uint32_t
{
    return math::RandomGenerator::Range64(4u);
}

ActiveGameBehavior* ActiveGameBehavior::gActive = nullptr;

ActiveGameBehavior::ActiveGameBehavior(GameBehavior::Variant variant)
: mBehavior(GameBehavior::make(variant))
{
    assert(gActive == nullptr);
    gActive = this;
}

auto ActiveGameBehavior::activeBehavior() -> GameBehavior
{
    constexpr auto kDefaultVariant = GameBehavior::Variant::standard;
    if (gActive == nullptr)
    {
        static std::mutex gMutex;
        auto lock = std::lock_guard(gMutex);
        if (gActive == nullptr)
        {
            // This is a leak
            gActive = new ActiveGameBehavior(kDefaultVariant);
        }
        else if (gActive->mBehavior.variant() != kDefaultVariant)
        {
            throw std::runtime_error("Race to create inconsistent GameBehavior variant");
        }
    }
    return gActive->mBehavior;
}

} // namespace pho::gstate
