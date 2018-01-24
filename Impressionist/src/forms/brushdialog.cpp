#include "brushdialog.h"
#include "ui_brushdialog.h"
#include <brushes/pointbrush.h>
#include <brushes/uwbrush.h>
#include <brushes/linebrush.h>
#include <brushes/scatterlinebrush.h>
#include <brushes/scatterpointbrush.h>
#include <brushes/circlebrush.h>
#include <brushes/scattercirclebrush.h>
#include <brushes/star.h>

BrushDialog::BrushDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrushDialog),
    current_brush_choice_(0),
    current_angle_choice_(0)
{
    ui->setupUi(this);

    // Create the brushes
    brushes_[Brushes::Point] = std::make_unique<PointBrush>("Points");
    // REQUIREMENT: Implement these five brushes. They are set to all be the same for now.
    brushes_[Brushes::Line] = std::make_unique<LineBrush>("Lines");
    brushes_[Brushes::Circle] = std::make_unique<CircleBrush>("Circles");
    brushes_[Brushes::ScatterPoint] = std::make_unique<ScatterPointBrush>("Scattered Points");
    brushes_[Brushes::ScatterLine] = std::make_unique<ScatterLineBrush>("Scattered Lines");
    brushes_[Brushes::ScatterCircle] = std::make_unique<ScatterCircleBrush>("Scattered Circles");
    brushes_[Brushes::UW] = std::make_unique<UWBrush>("UW");
    brushes_[Brushes::Star] = std::make_unique<StarBrush>("Star");

    // Add the brushes to the combo box
    for (auto& kv : brushes_) {
        brush_choices_.push_back(kv.first);
        ui->brush_choices->addItem(QString::fromStdString(kv.second->GetName()));
        ui->brush_props->addWidget(kv.second->GetWidget());
    }

    // Angle control methods
    ui->angle_control_choices->addItem("Slider / RMB");
    ui->angle_control_choices->addItem("Gradient");
    ui->angle_control_choices->addItem("Cursor Movement");
    angle_choices_ = {
        AngleMode::Direct,
        AngleMode::Gradient,
        AngleMode::CursorMovement
    };

    // Connect the combo box to select the current brush
    connect(ui->brush_choices, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index){
        current_brush_choice_ = index;
        ui->brush_props->setCurrentIndex(index);
    });

    // Connect the combo box to select the angle control type
    connect(ui->angle_control_choices, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int index){
        current_angle_choice_ = index;
    });
}

BrushDialog::~BrushDialog() {
    delete ui;
}

Brush& BrushDialog::GetCurrentBrush() {
    Brushes brush_type = brush_choices_[current_brush_choice_];
    assert(brushes_.count(brush_type) > 0);
    return *brushes_[brush_type];
}

AngleMode BrushDialog::GetCurrentAngleControl() {
    return angle_choices_[current_angle_choice_];

}
