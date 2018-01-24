#ifndef RGBABUFFER_H
#define RGBABUFFER_H

#include <cstring>

// Simple wrapper around an RGBA32 uchar buffer
class RGBABuffer {
public:
    RGBABuffer(unsigned int width, unsigned int height) :
        Size(width * height * 4),
        Width(width),
        Height(height),
        Bytes(new unsigned char[Size]) { }

    ~RGBABuffer() { delete[] Bytes; }

    const unsigned int Size;
    const unsigned int Width;
    const unsigned int Height;
    unsigned char* Bytes;
};

#endif // UCHARBUFFER_H
