#include "bilateralgaussdialog.h"
#include "ui_bilateralgaussdialog.h"
#include <paintview.h>
#include <filters/filter.h>

BilateralGaussDialog::BilateralGaussDialog(PaintView& paint_view, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BilateralGaussDialog),
    paint_view_(&paint_view),
    original_image_(nullptr)
{
    ui->setupUi(this);

    // Whenever something is changed, trigger preview after half a second
    preview_timer_.setInterval(1000);
    preview_timer_.setSingleShot(true);
    connect(&preview_timer_, &QTimer::timeout, this, &BilateralGaussDialog::Preview);

    // Connect all controls to trigger previewing
    connect(ui->sigma_domain_spinbox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]() { preview_timer_.start(); });
    connect(ui->sigma_range_spinbox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]() { preview_timer_.start(); });

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

BilateralGaussDialog::~BilateralGaussDialog() {
    delete ui;
}

int BilateralGaussDialog::exec() {
    paint_view_->SetCurrentLayer(PaintView::BASE_LAYER);
    // Save a copy of the original image
    original_image_ = paint_view_->GetSnapshot();
    Preview();
    return QDialog::exec();
}

void BilateralGaussDialog::Preview() {
    if (!ui->preview_checkbox->isChecked() || !original_image_) return;

    // Allocate space for the filtered image
    unsigned int width = paint_view_->GetWidth();
    unsigned int height = paint_view_->GetHeight();
    RGBABuffer filtered(width, height);

    // EXTRA CREDIT: Compute the filtered image
    Filter::ApplyBilateralGaussianBlur(original_image_->Bytes, filtered.Bytes, width, height, ui->sigma_domain_spinbox->value(), ui->sigma_range_spinbox->value());

    // EXTRA CREDIT: Draw the filtered image
    paint_view_->DrawImage(filtered.Bytes, width, height);
}

void BilateralGaussDialog::Reset() {
    ui->sigma_domain_spinbox->setValue(50);
    ui->sigma_range_spinbox->setValue(1);
    Preview();
}
