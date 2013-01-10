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
        i_currentIndex ++;
        if (i_currentIndex>=p_data.size()) {
            p_data.resize(p_data.size()+i_deltaSize);
        }
        p_data[i_currentIndex] = t;
    }

    inline T pop()
    {
        i_currentIndex--;
        return p_data[i_currentIndex+1];
    }

    inline T& top()
    {
        return p_data[i_currentIndex];
    }

    inline const T& top() const
    {
        return p_data[i_currentIndex];
    }

    inline T& at(int index)
    {
        return p_data[index];
    }

    inline const T& at(int index) const
    {
        return p_data[index];
    }

    inline int size() const { return i_currentIndex+1; }

    inline void reset()
    {
        p_data = std::vector<T>(i_initialSize);
        i_currentIndex = -1;
    }

    inline ~Stack()
    {
        p_data.clear();
    }

private:
    int i_initialSize;
    int i_deltaSize;
    int i_currentIndex;
    std::vector<T> p_data;
public:
    inline Stack() {
        i_initialSize = i_deltaSize = 100;
        i_currentIndex = 0;
        p_data = std::vector<T>(100);
    }
};

}

#endif
