#ifndef BILATERALGAUSSDIALOG_H
#define BILATERALGAUSSDIALOG_H

#include <rgbabuffer.h>
#include <memory>
#include <QDialog>
#include <QTimer>

class PaintView;

namespace Ui {
    class BilateralGaussDialog;
}

class BilateralGaussDialog : public QDialog {
    Q_OBJECT
public:
    explicit BilateralGaussDialog(PaintView& paint_view, QWidget *parent = 0);
    ~BilateralGaussDialog();
    virtual int exec() override;

private:
    Ui::BilateralGaussDialog *ui;
    PaintView* paint_view_;
    std::unique_ptr<RGBABuffer> original_image_;
    QTimer preview_timer_;

    // Applies the filter to the paint view
    void Preview();

    // Resets the UI controls to their default state
    void Reset();
};

#endif // BILATERALGAUSSDIALOG_H
