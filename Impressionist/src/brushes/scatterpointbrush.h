#ifndef SCATTERPOINTBRUSH_H
#define SCATTERPOINTBRUSH_H

#include <brushes/brush.h>

class ScatterPointBrush : public Brush {
public:
    ScatterPointBrush(const std::string& name);

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

#endif // SCATTERPOINTBRUSH_H
