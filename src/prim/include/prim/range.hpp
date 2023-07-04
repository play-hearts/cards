#pragma once

namespace pho::prim {

template <typename Int>
class range
{
public:
    static_assert(std::is_integral_v<Int>);

    class iterator
    {
    public:
        iterator(Int value_)
        : value(value_)
        { }

        bool operator!=(iterator const& other) const { return value != other.value; }

        Int const& operator*() const { return value; }

        iterator& operator++()
        {
            ++value;
            return *this;
        }

    private:
        Int value;
    };

    explicit range(Int to_)
    : from{}
    , to{to_}
    { }
    explicit range(Int from_, Int to_)
    : from{from_}
    , to{to_}
    { }

    iterator begin() const { return iterator(from); }
    iterator end() const { return iterator(to); }

private:
    const Int from;
    const Int to;

    range() = delete;
};

} // namespace pho::prim
