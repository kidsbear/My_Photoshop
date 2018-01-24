#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <glinclude.h>
#include <QMainWindow>
#include <QMessageBox>
#include <paintview.h>
#include <forms/filterkerneldialog.h>
#include <forms/bilateralmeandialog.h>
#include <forms/bilateralgaussdialog.h>
#include <forms/brushdialog.h>
#include <brushes/pointbrush.h>
#include <brushes/linesegmentbrush.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    const unsigned int DEFAULT_CANVAS_WIDTH = 500;
    const unsigned int DEFAULT_CANVAS_HEIGHT = 500;
    const unsigned int CANVAS_MARGIN = 50;
    const unsigned int CANVAS_SPACING = 6;

    static QString LastPath; // Last path accessed in the file dialog
    Ui::MainWindow *ui;

    PaintView* left_view_; // Reference View to sample from
    PaintView* right_view_; // Canvas to draw on

    // Dialogs
    BrushDialog* brush_dialog_;
    FilterKernelDialog* filter_kernel_dialog_;
    BilateralMeanDialog* bilat_mean_dialog_;
    BilateralGaussDialog* bilat_gauss_dialog_;

    // Mouse tracking
    Qt::MouseButtons mouse_buttons_;

    // Copy of the reference image
    unsigned char* reference_image_;
    unsigned int reference_image_width_;
    unsigned int reference_image_height_;

    // Overlay Brushes
    PointBrush marker_brush_;
    LineSegmentBrush angle_indicator_brush_;

    // Single-shot initialization
    void InitializeContext();
    void CreateActions();
    void CreateMenus();

    // Added
    int calGradient(Brush& brush, glm::vec2 pos); //return the angle 90 degrees from gradient

    // Physically resizes the two paintview widgets and MainWindow to contain them
    void ResizeCanvases(unsigned int width, unsigned int height);

    void CanvasMousePressed(QMouseEvent* event);
    void CanvasMouseMoved(QMouseEvent* event);
    void CanvasMouseReleased(QMouseEvent* event);
};

#endif // MAINWINDOW_H
