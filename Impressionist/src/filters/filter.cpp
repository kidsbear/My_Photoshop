#include "filter.h"
#include <iostream>

void Filter::ApplyFilterKernel(const unsigned char *source, unsigned char *dest, unsigned int width, unsigned int height, Kernel &k, int offset, bool clamping) {
    // REQUIREMENT: Implement this function
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            // Use calculated value for r,g,b channel
            for (unsigned int p = 0; p < 3; p++) {
                float filteredValue = 0.0;
                for (int l = -2; l <= 2; l++) {
                    for (int h = -2; h <= 2; h++) {
                        filteredValue += GetValue(source, i + l, j + h, width, height, p) * k.matrix[2-l][h+2];
                    }
                }
                int value = (int)filteredValue + offset;
                if (value > 255) {
                    value = 255;
                }
                if (value < 0) {
                    value = 0;
                }
                dest[4 * (i * width + j) + p] = value;
            }
            // Use origin value for alpha channel
            dest[4 * (i * width + j) + 3] = source[4 * (i * width + j) + 3];
        }
    }
}

void Filter::ApplyGaussianBlur(const unsigned char *source, unsigned char *dest, unsigned int width, unsigned int height, float sigma) {
    unsigned int blur_radius = sigma * 3;
    unsigned int kernel_size = blur_radius * 2 + 1;
    // EXTRA CREDIT: Implement this function
    int start = -1 * blur_radius;
    int end = blur_radius;
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            for (unsigned int p = 0; p < 3; p++) {
                float filteredValue = 0.0;
                float totalWeight = 0.0;
                for (int l = start; l <= end; l++) {
                    for (int h = start; h <= end; h++) {
                        float weight = exp(-((l * l + h * h) / (2 * sigma * sigma)));
                        totalWeight += weight;
                        filteredValue += weight * GetValue(source, i + l, j + h, width, height, p);
                    }
                }
                unsigned int value = (unsigned int)filteredValue / totalWeight;
                dest[4 * (i * width + j) + p] = value;
            }
            // Use origin value for alpha channel
            dest[4 * (i * width + j) + 3] = source[4 * (i * width + j) + 3];
        }
    }
}

void Filter::ApplyBilateralMeanBlur(const unsigned char *source, unsigned char *dest, unsigned int width, unsigned int height, unsigned int domain_half_width, unsigned int range) {
    unsigned int kernel_size = domain_half_width * 2 + 1;
    // REQUIREMENT: Implement this function
    int start = -1 * domain_half_width;
    int end = domain_half_width;

    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            unsigned int totalRValue = 0;
            unsigned int totalGValue = 0;
            unsigned int totalBValue = 0;
            unsigned int count = 0;
            for (int l = start; l <= end; l++) {
                for (int h = start; h <= end; h++) {
                    if (isPointInRange(source, i, j, l, h, range, width, height)) {
                        count++;
                        totalRValue += GetValue(source, i + l, j + h, width, height, 0);
                        totalGValue += GetValue(source, i + l, j + h, width, height, 1);
                        totalBValue += GetValue(source, i + l, j + h, width, height, 2);
                    }
                }
            }
            // Since the point itself must be in the range, so don't need to worry about divide by 0
            dest[4 * (i * width + j)] = (int) (totalRValue / count);
            dest[4 * (i * width + j) + 1] = (int) (totalGValue / count);
            dest[4 * (i * width + j) + 2] = (int) (totalBValue / count);
            // Use origin value for alpha channel
            dest[4 * (i * width + j) + 3] = source[4 * (i * width + j) + 3];
        }
    }
}

void Filter::ApplyBilateralGaussianBlur(const unsigned char *source, unsigned char *dest, unsigned int width, unsigned int height, float sigma_space, float sigma_range) {
    unsigned int kernel_radius = sigma_space * 3;
    unsigned int kernel_size = kernel_radius * 2 + 1;
    // EXTRA CREDIT: Implement this function

    int start = -1 * kernel_radius;
    int end = kernel_radius;
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            unsigned int totalRValue = 0;
            unsigned int totalGValue = 0;
            unsigned int totalBValue = 0;
            float totalWeight = 0.0;
            for (int l = start; l <= end; l++) {
                for (int h = start; h <= end; h++) {
                    int dist = rangeDist(source, i, j, l, h, width, height);
                    float weight = exp(-((l * l + h * h) / (2 * sigma_space * sigma_space))) * exp(-(dist / (2 * sigma_range * sigma_range)));
                    totalRValue += GetValue(source, i + l, j + h, width, height, 0) * weight;
                    totalGValue += GetValue(source, i + l, j + h, width, height, 1) * weight;
                    totalBValue += GetValue(source, i + l, j + h, width, height, 2) * weight;
                    totalWeight += weight;

                }
            }
            dest[4 * (i * width + j)] = (int) (totalRValue / totalWeight);
            dest[4 * (i * width + j) + 1] = (int) (totalGValue / totalWeight);
            dest[4 * (i * width + j) + 2] = (int) (totalBValue / totalWeight);
            // Use origin value for alpha channel
            dest[4 * (i * width + j) + 3] = source[4 * (i * width + j) + 3];
        }
    }

}

unsigned int Filter::GetValue(const unsigned char *source, int i, int j, unsigned int width, unsigned int height, unsigned int p) {
    if (i < 0) {
        i = 0;
    }
    if (i >= height) {
        i = height - 1;
    }
    if (j < 0) {
        j = 0;
    } if (j >= width) {
        j = width - 1;
    }
    return source[4 * (i * width + j) + p];
}

bool Filter::isPointInRange(const unsigned char *source, unsigned int i, unsigned int j, int l, int h, int range, unsigned int width, unsigned int height) {
    int difference = 0;
    for (int p = 0; p < 3; p++) {
        int sourceValue = GetValue(source, i, j, width, height, p);
        int destValue = GetValue(source, i + l, j + h, width, height, p);
        difference += (sourceValue - destValue) * (sourceValue - destValue) ;
    }
    // Sample solution uses range as one direction range, slides use range as both direction
    return difference <= range * range;
}

int Filter::rangeDist(const unsigned char *source, unsigned int i, unsigned int j, int l, int h, unsigned int width, unsigned int height) {
    int difference = 0;
    for (int p = 0; p < 3; p++) {
        int sourceValue = GetValue(source, i, j, width, height, p);
        int destValue = GetValue(source, i + l, j + h, width, height, p);
        difference += (sourceValue - destValue) * (sourceValue - destValue) ;
    }
    return difference;
}
