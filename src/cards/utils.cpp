#include "cards/utils.hpp"
#include "math/random.hpp"
#include "prim/range.hpp"

namespace pho::cards {

Card aCardAtRandom(CardSet set) { return set.nthCard(pho::math::RandomGenerator::Range64(set.size())); }

CardSet chooseThreeAtRandom(CardSet dealt)
{
    auto tmp = dealt;
    assert(tmp.size() == 13);
    auto result = CardSet();
    for (auto i : pho::prim::range(3))
    {
        (void)i;
        auto card = aCardAtRandom(tmp);
        result += card;
        tmp -= card;
    }
    assert(tmp.size() == 10);
    return result;
}

} // namespace pho::cards
