#ifndef FILTER_H
#define FILTER_H

#include <vectors.h>
#include <functional>
#include <rgbabuffer.h>
#include <QDebug>

// Utility class for applying filters
// OPTIONAL: You can choose to implement this as a helper class for filter kernel.
class Kernel {
public:
    std::vector<std::vector<float>> matrix;

    Kernel(int height, int width) {
        matrix.resize(height);
        for(int i = 0; i < height; i++) {
            matrix[i].resize(width);
        }
    }
};

class Filter {
public:
    // Applies a filter kernel to the RGB channels of the source image and stores it into dest
    static void ApplyFilterKernel(const unsigned char* source, unsigned char* dest, unsigned int width, unsigned int height, Kernel& k, int offset = 0, bool clamping = true);

    // Applies a gaussian blur to the RGB channels of the source image and stores it into dest
    static void ApplyGaussianBlur(const unsigned char* source, unsigned char* dest, unsigned int width, unsigned int height, float sigma = 1);

    // Applies a bilateral mean blur to the RGB channels of the source image and stores it into dest
    static void ApplyBilateralMeanBlur(const unsigned char* source, unsigned char* dest, unsigned int width, unsigned int height, unsigned int domain_half_width, unsigned int range);

    // Applies a bilateral gaussian blur to the RGB channels of the source image and stores it into dest
    static void ApplyBilateralGaussianBlur(const unsigned char* source, unsigned char* dest, unsigned int width, unsigned int height, float sigma_space, float sigma_range);

private:
    // Get the pixel value from source, for pixels outside the boundary, uses flipped image pixel
    static unsigned int GetValue(const unsigned char *source, int i, int j, unsigned int width, unsigned int height, unsigned int p);

    static bool isPointInRange(const unsigned char *source, unsigned int i, unsigned int j, int l, int h, int range, unsigned int width, unsigned int height);

    static int rangeDist(const unsigned char *source, unsigned int i, unsigned int j, int l, int h, unsigned int width, unsigned int height);
};

#endif // FILTER_H
