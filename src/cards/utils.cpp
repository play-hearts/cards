#include "cards/utils.hpp"
#include "math/random.hpp"
#include "prim/range.hpp"

namespace pho::cards {

Card aCardAtRandom(CardSet set) { return set.nthCard(pho::math::RandomGenerator::Range64(set.size())); }

CardSet chooseSomeAtRandom(CardSet set, unsigned N)
{
    auto tmp = set;
    auto result = CardSet();
    for (auto i : pho::prim::range(N))
    {
        (void)i;
        auto card = aCardAtRandom(tmp);
        result += card;
        tmp -= card;
    }
    return result;
}

CardSet removeSomeAtRandom(CardSet& set, unsigned N)
{
    auto before = set.size();
    (void) before;
    auto chosen = chooseSomeAtRandom(set, N);
    assert(chosen.size() == N);
    set -= chosen;
    assert(set.size() == before - N);
    return chosen;
}


} // namespace pho::cards
