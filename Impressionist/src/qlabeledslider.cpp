#include "qlabeledslider.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

QLabeledSlider::QLabeledSlider(Qt::Orientation orientation, QWidget *parent) :
    QWidget(parent),
    slider_(new QSlider),
    label_(new QSpinBox)
{
    QLayout* layout = nullptr;
    if (orientation == Qt::Horizontal) {
        layout = new QHBoxLayout;
        slider_->setOrientation(Qt::Horizontal);
    } else {
        layout = new QVBoxLayout;
        slider_->setOrientation(Qt::Vertical);
    }
    slider_->setSingleStep(1);
    label_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    label_->setEnabled(false);
    layout->setMargin(0);
    layout->addWidget(slider_);
    layout->addWidget(label_);
    setLayout(layout);

    connect(slider_, &QSlider::valueChanged, label_, &QSpinBox::setValue);
}

QSlider &QLabeledSlider::GetSlider() const {
    return *slider_;
}

void QLabeledSlider::SetRange(int min, int max) {
    slider_->setRange(min, max);
    label_->setRange(min, max);
}

void QLabeledSlider::SetValue(int value) {
    slider_->setValue(value);
}

int QLabeledSlider::GetValue() const {
    return slider_->value();
}
