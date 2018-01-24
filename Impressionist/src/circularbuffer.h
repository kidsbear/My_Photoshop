#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <cassert>

// Generic fixed size CircularBuffer
// FIFO data structure with constant time operations.
// Elements are copied in on Push() and copied out on access.
template<typename T>
class CircularBuffer {
public:
    CircularBuffer(int capacity) {
        assert(capacity > 0);
        buffer = new T[capacity];
        head = 0;
        this->capacity = capacity;
        this->size = 0;
    }

    ~CircularBuffer() {
        delete[] buffer;
    }

    // Allows the buffer to be accessed like an array.
    // Index 0 is the oldest value in the buffer.
    T operator [] (int index) {
        if(index < 0) index = 0;
        assert(index < size);
        return buffer[(head + index) % capacity];
    }

    // Fills the buffer to capacity with the specified element.
    void Fill(T element) {
        for(int i = 0; i < capacity; i++)
            buffer[i] = element;
        size = capacity;
    }

    // Puts an element into the buffer,
    // replacing the oldest element.
    void Push(T element) {
        if(size == capacity) {
            buffer[head] = element;
            head++;
            if(head >= capacity)
                head = head % capacity;
        } else if(size < capacity) {
            buffer[size] = element;
            size++;
        }
    }

    // Returns the number of elements in the buffer
    int Size() { return size; }
private:
    T* buffer;		// buffer in memory
    int head;		// index of the oldest element
    int	capacity;	// physical capacity of buffer
    int size;		// number of elements in buffer
};

#endif // CIRCULARBUFFER_H
