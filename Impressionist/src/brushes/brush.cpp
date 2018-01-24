#include "brush.h"
#include <filters/filter.h>
#include <QWidget>
#include <QFormLayout>
#include <QComboBox>
#include <qlabeledslider.h>

Brush::Brush(const std::string& name) :
    widget_(new QWidget),
    layout_(new QFormLayout),
    name_(name),
    size_slider_(new QLabeledSlider),
    color_(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
    color_image_(nullptr),
    color_image_width_(0),
    color_image_height_(0),
    color_mode_(ColorMode::Solid),
    color_location_(-1),
    opacity_slider_(new QLabeledSlider),
    angle_slider_(new QLabeledSlider)
{
    widget_->setLayout(layout_);

    // Size Slider
    size_slider_->SetRange(1, 100);
    layout_->addRow("Size", size_slider_);

    // REQUIREMENT: Allow user to control the opacity via a slider
    opacity_slider_->SetRange(0, 100);
    layout_->addRow("Opacity", opacity_slider_);
    opacity_slider_->SetValue(100);

    // Angle Slider
    angle_slider_->SetRange(0, 359);
    layout_->addRow("Angle", angle_slider_);
    angle_slider_->SetValue(0);

    // Default Values
    SetSize(12);
}

QWidget* Brush::GetWidget() const {
    return widget_;
}

std::string Brush::GetName() const {
    return name_;
}

void Brush::SetAngle(unsigned int newAngle) {
    angle_slider_->SetValue(newAngle);
}

void Brush::SetSize(unsigned int size) {
    size = size > 100 ? 100 : size;
    size_slider_->SetValue(size);
}

void Brush::SetColor(const glm::vec3& color) {
    color_ = color;
}

void Brush::SetColorImage(const unsigned char* color_image, unsigned int width, unsigned int height) {
    color_image_ = color_image;
    color_image_width_ = width;
    color_image_height_ = height;
}

void Brush::SetColorMode(ColorMode color_mode) {
    color_mode_ = color_mode;
}

void Brush::SetColorLocation(GLint color_location) {
    color_location_ = color_location;
}

void Brush::UseColor(const glm::vec4& color) {
    assert(color_location_ != -1);
    glUniform4fv(color_location_, 1, glm::value_ptr(color));
}

unsigned int Brush::GetSize() const {
    return size_slider_->GetValue();
}

// Added functionality
unsigned int Brush::GetOpacity() const {
    return opacity_slider_->GetValue();
}

unsigned int Brush::GetAngle() const {
    return angle_slider_->GetValue();
}


glm::vec4 Brush::GetColor(glm::ivec2 position) const {
    if (color_mode_ == ColorMode::Sample && color_image_ != nullptr) {
        unsigned int width = color_image_width_;
        unsigned int height = color_image_height_;

        // Don't exceed the coordinates of the color image
        if (position.x < 0) position.x = 0;
        else if (position.x >= width) position.x = width - 1;
        if (position.y < 0) position.y = 0;
        else if (position.y >= height) position.y = height - 1;

        // Sample the color image at the position
        glm::vec4 color;
        int index = (position.y * 4) * width + (position.x * 4);
        color.r = color_image_[index++] / 255.0f;
        color.g = color_image_[index++] / 255.0f;
        color.b = color_image_[index++] / 255.0f;
        color.a = color_image_[index++] / 255.0f;

        return color;
    } else {
        return glm::vec4(color_, 1.0f);
    }
}

