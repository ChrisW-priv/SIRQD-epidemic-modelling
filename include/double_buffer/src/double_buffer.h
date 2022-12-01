#ifndef DOUBLE_BUFFER_DOUBLE_BUFFER_H
#define DOUBLE_BUFFER_DOUBLE_BUFFER_H


#include <memory>

template<class T>
class DoubleBuffer{
    size_t N;
    // allocate space for buffers:
    std::unique_ptr<T[]> current = nullptr;
    std::unique_ptr<T[]> next = nullptr;

    typedef T* iterator;
    typedef const T* const_iterator;

public:
    explicit DoubleBuffer(size_t size) : N(size), current(new T[N]), next(new T[N]){
        size_t i=0;
        while (i<N) current[i++] = T{};
        match_buffers();
    }

    DoubleBuffer(DoubleBuffer const &other) : N(other.N), current(new T[N]), next(new T[N]){
        size_t i=0;
        while (i<N){
            current[i] = other.current[i];
            ++i;
        }
        i=0;
        while (i<N){
            next[i] = other.next[i];
            ++i;
        }
    }

    template<class Iter>
    DoubleBuffer(Iter begin, Iter end) : N(std::distance(begin, end)), current(new T[N]), next(new T[N]){
        size_t i=0;
        while (begin != end) {
            current[i] = *begin;
            ++begin; ++i;
        }
        match_buffers();
    }

    void swap_buffers(){ next.swap(current); }

    void match_buffers(){
        size_t i=0;
        while (i<N){ next[i] = current[i]; ++i; }
    }

    T& operator[](size_t index){ return current[index]; }
    T& at_curr(size_t index){ return current[index]; }
    T& at_next(size_t index){ return next[index]; }

    // iterators for range base loops
    iterator begin() { return &current[0]; }
    iterator end() { return &current[N]; }
    const_iterator cbegin() const { return &current[0]; }
    const_iterator cend() const { return &current[N]; }
};

#endif //DOUBLE_BUFFER_DOUBLE_BUFFER_H
