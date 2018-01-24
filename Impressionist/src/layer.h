#ifndef LAYER_H
#define LAYER_H

#include <glinclude.h>

// Each Layer encapulates a framebuffer to be drawn on.
class Layer {
public:
    Layer(unsigned int width, unsigned int height);

    QOpenGLFramebufferObject& Framebuffer() { return framebuffer_; }

private:
    QOpenGLFramebufferObject framebuffer_;
};

#endif // LAYER_H
