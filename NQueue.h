#pragma once
#include <deque>
#include <stdexcept>

template<typename T>
class NQueue {
private:
    std::deque<T> queue;
public:
    NQueue() {};
    NQueue(int n, T initialValue) {
        for (int i = 0; i < n; i++) queue.push_back(initialValue);
    }
    T Dequeue() {
        if (queue.empty()) throw std::out_of_range("");
        T value = queue.front();
        queue.pop_front();
        return value;
    }
    void Enqueue(T val) {
        queue.push_back(val);
    }
    int Size() const {
        return static_cast<int>(queue.size());
    }
    void Clear() {
        queue.clear();
    }
    void ChangeAt(int index, T val) {
        if (index < 0 || index >= queue.size()) throw std::out_of_range("");
        queue[index] = val;
    }
    T peek(int index) const {
        if (index < 0 || index >= queue.size()) throw std::out_of_range("");
        return queue[index];
    }
    void pop_front() {
        if (queue.empty()) throw std::out_of_range("");
        queue.pop_front();
    }
    void pop_back() {
        if (queue.empty()) throw std::out_of_range("");
        queue.pop_back();
    }
};
template<typename T>
NQueue<T> operator+(const NQueue<T>& l, const NQueue<T>& r) {
    int min = l.Size() < r.Size() ? l.Size() : r.Size();
    NQueue<T> result;
    for (int i = 0; i < min; i++) {
        result.Enqueue(l.peek(i) + r.peek(i));
    }
    return result;
}
struct NReal{
    double v;
    explicit NReal(double val)
        : v(val)
    {}
};

inline double operator+=(double& a, NReal b){
    a += b.v;
    return a;
}
