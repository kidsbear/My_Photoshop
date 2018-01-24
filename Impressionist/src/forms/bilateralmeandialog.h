#ifndef BILATERALMEANDIALOG_H
#define BILATERALMEANDIALOG_H

#include <rgbabuffer.h>
#include <memory>
#include <QDialog>
#include <QTimer>

class PaintView;

namespace Ui {
    class BilateralMeanDialog;
}

class BilateralMeanDialog : public QDialog {
    Q_OBJECT
public:
    explicit BilateralMeanDialog(PaintView& paint_view, QWidget *parent = 0);
    ~BilateralMeanDialog();
    virtual int exec() override;

private:
    Ui::BilateralMeanDialog *ui;
    PaintView* paint_view_;
    std::unique_ptr<RGBABuffer> original_image_;
    QTimer preview_timer_;

    // Applies the filter to the paint view
    void Preview();

    // Resets the UI controls to their default state
    void Reset();
};

#endif // BILATERALMEANDIALOG_H
