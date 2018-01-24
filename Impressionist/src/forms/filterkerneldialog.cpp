#include "filterkerneldialog.h"
#include "ui_filterkerneldialog.h"
#include <paintview.h>
#include <filters/filter.h>
#include <cassert>
#include <iostream>

FilterKernelDialog::FilterKernelDialog(PaintView& paint_view, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterKernelDialog),
    paint_view_(&paint_view),
    original_image_(nullptr)
{
    ui->setupUi(this);

    // Whenever something is changed, trigger preview after half a second
    preview_timer_.setInterval(500);
    preview_timer_.setSingleShot(true);
    connect(&preview_timer_, &QTimer::timeout, this, &FilterKernelDialog::Preview);

    // Connect all controls to trigger previewing
    for (unsigned int j = 0; j < KERNEL_HEIGHT; j++) {
        for (unsigned int i = 0; i < KERNEL_WIDTH; i++) {
            QDoubleSpinBox* widget = qobject_cast<QDoubleSpinBox*>(ui->matrix_vals_layout->itemAtPosition(i, j)->widget());
            assert(widget);
            connect(widget, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]() { preview_timer_.start(); });
        }
    }
    connect(ui->divisor_spinbox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]() { preview_timer_.start(); });
    connect(ui->offset_spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() { preview_timer_.start(); });
    connect(ui->normalize_checkbox, &QCheckBox::stateChanged, this, &FilterKernelDialog::Preview);

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

FilterKernelDialog::~FilterKernelDialog() {
    delete ui;
}

int FilterKernelDialog::exec() {
    paint_view_->SetCurrentLayer(PaintView::BASE_LAYER);
    // Save a copy of the original image
    original_image_ = paint_view_->GetSnapshot();
    return QDialog::exec();
}

float FilterKernelDialog::GetKernelValue(int i, int j) {
    QDoubleSpinBox* widget = qobject_cast<QDoubleSpinBox*>(ui->matrix_vals_layout->itemAtPosition(j, i)->widget());
    assert(widget);
    return widget->value();
}

void FilterKernelDialog::Preview() {
    if (!ui->preview_checkbox->isChecked() || !original_image_) return;

    // Allocate space for the filtered image
    unsigned int width = paint_view_->GetWidth();
    unsigned int height = paint_view_->GetHeight();
    RGBABuffer filtered(width, height);

    // REQUIREMENT: Compute the filtered image
    // See FilterKernelDialog::GetKernelValue to access kernel values from UI
    // Filter::ApplyFilterKernel(...);

    int offset = ui->offset_spinbox->value();
    float epsilon = 0.0000001;
    float divisor = ui->divisor_spinbox->value();
    if (divisor - 0 < epsilon) divisor = 1.0;
    float total = 0.0;

    Kernel* kernel = new Kernel(KERNEL_HEIGHT, KERNEL_WIDTH);
    for (unsigned int i = 0; i < KERNEL_HEIGHT; i++) {
        for (unsigned int j = 0; j < KERNEL_WIDTH; j++) {
            kernel->matrix[i][j] = GetKernelValue(j,i) / divisor;
            total += kernel->matrix[i][j];
        }
    }

    if(ui->normalize_checkbox->isChecked()) {
        if (total - 0 > epsilon) {
            for (unsigned int i = 0; i < KERNEL_HEIGHT; i++) {
                for (unsigned int j = 0; j < KERNEL_WIDTH; j++) {
                    kernel->matrix[i][j] /= total;
                }
            }
        }
    }

    Filter::ApplyFilterKernel(original_image_->Bytes, filtered.Bytes, width, height, *kernel, offset, true);
    // REQUIREMENT: Draw the filtered image
    paint_view_->DrawImage(filtered.Bytes, width, height);
}

void FilterKernelDialog::Reset() {
    // Reset the Kernel
    for (unsigned int j = 0; j < KERNEL_HEIGHT; j++) {
        for (unsigned int i = 0; i < KERNEL_WIDTH; i++) {
            QDoubleSpinBox* widget = qobject_cast<QDoubleSpinBox*>(ui->matrix_vals_layout->itemAtPosition(i, j)->widget());
            assert(widget);
            if (i == 2 && j == 2) widget->setValue(1);
            else widget->setValue(0);

        }
    }
    ui->divisor_spinbox->setValue(1);
    ui->offset_spinbox->setValue(0);
    Preview();
}
