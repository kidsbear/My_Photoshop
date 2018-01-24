#ifndef SCATTERLINEBRUSH_H
#define SCATTERLINEBRUSH_H

#include <brushes/brush.h>

class ScatterLineBrush : public Brush {
public:
    ScatterLineBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;

protected:
    QLabeledSlider* thickness_slider_;
    QLabeledSlider* radius_slider_;
    QLabeledSlider* density_slider_;

    // Called inside the BrushBegin/BrushMove/BrushEnd methods
    unsigned int GetThickness() const;
    unsigned int GetRadius() const;
    unsigned int GetDensity() const;

};

#endif // SCATTERLINEBRUSH_H
