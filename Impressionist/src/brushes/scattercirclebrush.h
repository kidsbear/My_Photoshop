#ifndef SCATTERCIRCLEBRUSH_H
#define SCATTERCIRCLEBRUSH_H

#include <brushes/brush.h>

class ScatterCircleBrush : public Brush {
public:
    ScatterCircleBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;

private:
    QLabeledSlider* radius_slider_;
    QLabeledSlider* density_slider_;

    // Called inside the BrushBegin/BrushMove/BrushEnd methods
    unsigned int GetRadius() const;
    unsigned int GetDensity() const;

};

#endif // SCATTERCIRCLEBRUSH_H
