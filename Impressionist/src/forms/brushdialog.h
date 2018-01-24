#ifndef BRUSHDIALOG_H
#define BRUSHDIALOG_H

#include <brushes/brush.h>
#include <map>
#include <memory>
#include <QDialog>

namespace Ui {
    class BrushDialog;
}

class BrushDialog : public QDialog {
    Q_OBJECT

public:
    explicit BrushDialog(QWidget *parent = 0);
    ~BrushDialog();

    Brush& GetCurrentBrush();
    AngleMode GetCurrentAngleControl();

private:
    Ui::BrushDialog *ui;
    std::map<Brushes, std::unique_ptr<Brush>> brushes_;
    std::vector<Brushes> brush_choices_;
    std::vector<AngleMode> angle_choices_;
    std::map<Brushes, QWidget*> brush_widgets_;
    unsigned int current_brush_choice_;
    unsigned int current_angle_choice_;
};

#endif // BRUSHDIALOG_H
