#pragma once

#include <cassert>

template <typename Iterator>
class CircularIterator
{
public:
    CircularIterator(Iterator begin, Iterator end)
        : begin_(begin), end_(end), current_(begin)
    {
        //assert(begin_ != end_ && "CircularIterator cannot be initialized with an empty range");
    }

    typename Iterator::value_type& operator*()
    {
        assert(current_ != end_ && "Cannot dereference end iterator");
        return *current_;
    }

    CircularIterator& operator++()
    {
        assert(current_ != end_ && "Cannot increment past the end of the range");
        ++current_;
        if (current_ == end_)
            current_ = begin_;
        return *this;
    }

    CircularIterator operator++(int)
    {
        CircularIterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator!=(const CircularIterator& other) const
    {
        return current_ != other.current_;
    }

    typename Iterator::value_type* operator->()
    {
        assert(current_ != end_ && "Cannot access element through end iterator");
        return &(*current_);
    }

private:
    Iterator begin_;
    Iterator end_;
    Iterator current_;
};