#ifndef QLABELEDSLIDER_H
#define QLABELEDSLIDER_H

#include <QWidget>
#include <QSlider>
#include <QSpinBox>

// Simple wrapper around a QSlider that also has a QSpinbox showing its numerical value
class QLabeledSlider : public QWidget {
public:
    // Either Horizontal or Vertical orientation
    QLabeledSlider(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = Q_NULLPTR);
    QSlider& GetSlider() const;
    void SetRange(int min, int max);
    void SetValue(int value);
    int GetValue() const;
private:
    QSlider* slider_;
    QSpinBox* label_;
};

#endif // QLABELEDSLIDER_H
