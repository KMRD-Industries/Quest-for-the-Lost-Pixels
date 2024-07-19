#pragma once

template<typename Iterator>
class CircularIterator {
public:
    CircularIterator(Iterator begin, Iterator end)
        : begin_(begin), end_(end), current_(begin) {}

    typename Iterator::value_type& operator*() {
        return *current_;
    }

    CircularIterator& operator++() {
        ++current_;
        if (current_ == end_) {
            current_ = begin_;
        }
        return *this;
    }

    CircularIterator operator++(int) {
        CircularIterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator!=(const CircularIterator& other) const {
        return current_ != other.current_;
    }

    typename Iterator::value_type* operator->() {
        return &(*current_);
    }

private:
    Iterator begin_;
    Iterator end_;
    Iterator current_;
};