#ifndef LINEBRUSH_H
#define LINEBRUSH_H

#include <brushes/brush.h>

class LineBrush : public Brush {
public:
    LineBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;

protected:
    QLabeledSlider* thickness_slider_;


    // Called inside the BrushBegin/BrushMove/BrushEnd methods
    unsigned int GetThickness() const;
};

#endif // LINEBRUSH_H
