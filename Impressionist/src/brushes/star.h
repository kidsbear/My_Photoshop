#ifndef STAR_H
#define STAR_H

#include <brushes/brush.h>

class StarBrush : public Brush {
public:
    StarBrush(const std::string& name);

    virtual void BrushBegin(const glm::vec2 pos) override;
    virtual void BrushMove(const glm::vec2 pos) override;
    virtual void BrushEnd(const glm::vec2 pos) override;

//protected:
//    QLabeledSlider* thickness_slider_;


//    // Called inside the BrushBegin/BrushMove/BrushEnd methods
//    unsigned int GetThickness() const;
};

#endif // STAR_H
