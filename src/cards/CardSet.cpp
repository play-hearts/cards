#include "cards/CardSet.hpp"
#include "prim/range.hpp"

namespace pho::cards {

std::string to_string(CardSet cards)
{
    std::string result;
    for (Card card : cards)
    {
        result += nameOfCard(card);
    }
    return result.size()>0 ? result : "<empty>";
}

} // namespace pho::cards
