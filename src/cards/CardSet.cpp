#include "cards/CardSet.hpp"
#include "cards/utils.hpp"
#include "prim/range.hpp"

#if __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace pho::cards {

std::string to_string(CardSet cards)
{
    std::string result;
    for (Card card : cards)
    {
        result += nameOfCard(card);
    }
    return result.size() > 0 ? result : "<empty>";
}

std::vector<Card> CardSet::asCardVector() const
{
    std::vector<Card> result;
    for (Card card : *this)
    {
        result.push_back(card);
    }
    return result;
}

#if __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(CardSet)
{
    class_<CardSet>("CardSet")
        .constructor<>()
        .function("asCardVector", &CardSet::asCardVector)
        .function("size", &CardSet::size)
        .function("setUnion", &CardSet::setUnion)
        .function("setSubtract", &CardSet::setSubtract)
        .function("setIntersection", &CardSet::setIntersection)
        .function("equal", &CardSet::operator==)
        // hasOrd and addOrd do not exist in the C++ API, but are useful for the JS API
        .function("hasOrd", optional_override([](CardSet& cardSet, Ord ord) { return cardSet.hasCard(Card(ord)); }))
        .function("addOrd", optional_override([](CardSet& cardSet, Ord ord) { cardSet += Card(ord); }));

    function("to_string", &pho::cards::to_string);
    function("chooseThreeAtRandom", &pho::cards::chooseThreeAtRandom);
    function("aCardAtRandom", &pho::cards::aCardAtRandom);

    register_vector<Card>("CardVector");
}
#endif

} // namespace pho::cards
