#ifndef STACK_HPP
#define STACK_HPP

#include <cstdlib>
#include <vector>

namespace VM {

template <class T> class Stack
{
public:
    inline void push(const T& t)
    {
        currentIndex_ ++;
        if (currentIndex_>=(int)data_.size()) {
            data_.resize(data_.size()+deltaSize_);
        }
        data_[currentIndex_] = t;
    }

    inline T pop()
    {
        currentIndex_--;
        return data_[currentIndex_+1];
    }

    inline T& top()
    {
        return data_[currentIndex_];
    }

    inline const T& top() const
    {
        return data_[currentIndex_];
    }

    inline T& at(int index)
    {
        return data_[index];
    }

    inline const T& at(int index) const
    {
        return data_[index];
    }

    inline int size() const { return currentIndex_+1; }
    inline int reservedSize() const { return data_.size(); }

    inline void reset()
    {
        data_ = std::vector<T>(initialSize_);
        currentIndex_ = -1;
    }

    inline ~Stack()
    {
        data_.clear();
    }

private:
    int initialSize_;
    int deltaSize_;
    int currentIndex_;
    std::vector<T> data_;
public:
    inline Stack() {
        initialSize_ = deltaSize_ = 100;
        currentIndex_ = 0;
        data_ = std::vector<T>(100);
    }
};

}

#endif
