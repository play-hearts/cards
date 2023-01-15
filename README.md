# Cards: a fast implementation for operations on sets of cards

This library is the core for Monte Carlo simulations of the card game Hearts. No Heart-specific algorithms are
included here though we intend to release a separate library for Hearts in the future.

## CardSet

A set of cards up to a full deck of 52 cards is implemented with the class CardSet. This class has a single
64-bit word data member that represents the set of cards in the set using 52 bits of the word.

This representation is efficient
in both space and time. Even iterating over the cards in a subset of cards (e.g a hand of 13 cards) is efficient
using bit manipulation intrinsics that map to single machine instructions.

## Dealing hands for Hearts, Spades, Bridge, etc.

There are 52!/(13!)^4 possible deals of 13 cards to each of 4 players. Let's call this number D. D requires 96 bits to represent,
and can be computed using 128-bit arithmetic only if care is done to ensure an order of operations that does not lead
to overflow. For example 52! is way too large to represent in 128 bits -- it would require at least 226 bits.

One can create a 1:1 mapping between integers in that range and distinct deals. To generate a deal, simply generate
a random integer in the range, and then create the deal corresponding to that integer. We provide generalized versions
of these algorithms.

For example, at any point in a game of Hearts the player whose turn it is to play knows where some of the cards are but
in general there will be a set of cards that could be still unplayed in two or three of the other players hands. How many possible
arrangements are there for the unknown cards? We provide the function to compute that number, and the function to map
a random number in that range to a distinct arrangement of unknown cards.

## Random Number Generation

We use the superb `xoshiro256** 1.0` algorithm to generate 64 random bits twice, concatenating the result into a single 128-bit
word. We then carefully derive a random number in the correct range (i.e. we don't naively use modulo arithmetic, which would
produce a biased result where some arrangements are slightly more likely than other arrangements).

The `xoshiro256** 1.0` uses 256 bits of internal state. The default method of initializing the state is to use the Posix `/dev/urandom`
device. We provide an API using thread local static memory so that each thread will have its own unique random sequence.
