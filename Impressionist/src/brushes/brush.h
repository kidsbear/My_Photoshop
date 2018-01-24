#ifndef BRUSH_H
#define BRUSH_H

#include <glinclude.h>
#include <string>
#include <vectors.h>

class QWidget;
class QFormLayout;
class QLabeledSlider;
class QComboBox;

// Different types of brushes
enum class Brushes {
    Point,
    Line,
    Circle,
    ScatterPoint,
    ScatterLine,
    ScatterCircle,
    UW,
    Star
};

// How to control the angle of the brush
enum class AngleMode {
    Direct,
    CursorMovement,
    Gradient
};

// How to get the color to draw with
enum class ColorMode {
    Solid,
    Sample
};

class Brush {
public:
    Brush(const std::string& name);

    QWidget* GetWidget() const;
    std::string GetName() const;

    virtual void SetAngle(unsigned int angle);
    virtual void SetSize(unsigned int size);
    virtual void SetColor(const glm::vec3& color);
    virtual void SetColorImage(const unsigned char* color_image, unsigned int width, unsigned int height);
    virtual void SetColorMode(ColorMode color_mode);

    unsigned int GetAngle() const;

    // Must be called before drawing
    virtual void SetColorLocation(GLint color_location);
    glm::vec4 GetColor(glm::ivec2 position = glm::ivec2(0, 0)) const;

    // Called for drawing
    virtual void BrushBegin(const glm::vec2 pos) = 0;
    virtual void BrushMove(const glm::vec2 pos) = 0;
    virtual void BrushEnd(const glm::vec2 pos) = 0;

protected:
    QWidget* widget_;
    QFormLayout* layout_;
    std::string name_;
    QLabeledSlider* size_slider_;
    QLabeledSlider* opacity_slider_;
    QLabeledSlider* angle_slider_;
    ColorMode color_mode_;
    GLint color_location_;
    glm::vec3 color_;

    // Image to sample colors from
    const unsigned char* color_image_;
    unsigned int color_image_width_;
    unsigned int color_image_height_;

    // Called inside the BrushBegin/BrushMove/BrushEnd methods
    unsigned int GetSize() const;
    unsigned int GetOpacity() const;


    void UseColor(const glm::vec4& color);
};

#endif // BRUSH_H
