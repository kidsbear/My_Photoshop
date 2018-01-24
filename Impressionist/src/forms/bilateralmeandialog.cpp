#include "bilateralmeandialog.h"
#include "ui_bilateralmeandialog.h"
#include <paintview.h>
#include <filters/filter.h>

BilateralMeanDialog::BilateralMeanDialog(PaintView& paint_view, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BilateralMeanDialog),
    paint_view_(&paint_view),
    original_image_(nullptr)
{
    ui->setupUi(this);

    // Whenever something is changed, trigger preview after half a second
    preview_timer_.setInterval(1000);
    preview_timer_.setSingleShot(true);
    connect(&preview_timer_, &QTimer::timeout, this, &BilateralMeanDialog::Preview);

    // Connect all controls to trigger previewing
    connect(ui->domain_spinbox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]() { preview_timer_.start(); });
    connect(ui->range_spinbox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]() { preview_timer_.start(); });

    // Preview Checkbox
    connect(ui->preview_checkbox, &QCheckBox::stateChanged, this, [this]() {
        if (ui->preview_checkbox->isChecked()) Preview();
        else paint_view_->DrawImage(original_image_->Bytes, original_image_->Width, original_image_->Height);
    });

    // Dialog Button Box
    connect(ui->button_box, &QDialogButtonBox::clicked, this, [this](QAbstractButton* button) {
        switch (ui->button_box->buttonRole(button)) {
            case QDialogButtonBox::ResetRole:
                Reset();
                break;
            case QDialogButtonBox::RejectRole:
                // Draw the original image back to the paint view
                paint_view_->DrawImage(original_image_->Bytes, original_image_->Width, original_image_->Height);
                break;
        }
    });
}

BilateralMeanDialog::~BilateralMeanDialog() {
    delete ui;
}

int BilateralMeanDialog::exec() {
    paint_view_->SetCurrentLayer(PaintView::BASE_LAYER);
    // Save a copy of the original image
    original_image_ = paint_view_->GetSnapshot();
    Preview();
    return QDialog::exec();
}

void BilateralMeanDialog::Preview() {
    if (!ui->preview_checkbox->isChecked() || !original_image_) return;

    // Allocate space for the filtered image
    unsigned int width = paint_view_->GetWidth();
    unsigned int height = paint_view_->GetHeight();
    RGBABuffer filtered(width, height);

    // REQUIREMENT: Compute the filtered image
    Filter::ApplyBilateralMeanBlur(original_image_->Bytes, filtered.Bytes, width, height, ui->domain_spinbox->value(), ui->range_spinbox->value());

    // REQUIREMENT: Draw the filtered image
    paint_view_->DrawImage(filtered.Bytes, width, height);
}

void BilateralMeanDialog::Reset() {
    ui->domain_spinbox->setValue(2);
    ui->range_spinbox->setValue(50);
    Preview();
}
