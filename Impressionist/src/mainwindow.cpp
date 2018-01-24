#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <brushes/brush.h>
#include <filters/filter.h>
#include <assert.h>
#include <QScrollArea>
#include <QOffscreenSurface>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <math.h>

#include <iostream>

QString MainWindow::LastPath = QDir::currentPath();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    left_view_(nullptr),
    right_view_(nullptr),
    brush_dialog_(nullptr),
    filter_kernel_dialog_(nullptr),
    bilat_mean_dialog_(nullptr),
    bilat_gauss_dialog_(nullptr),
    mouse_buttons_(0),
    reference_image_(nullptr),
    reference_image_width_(0),
    reference_image_height_(0),
    marker_brush_("Marker Brush"),
    angle_indicator_brush_("Angle Indicator Brush")
{
    ui->setupUi(this);
    setWindowTitle(tr("Impressionist"));

    InitializeContext();
    // Must be initialized after InitializeContext
    left_view_ = new PaintView(this);
    right_view_ = new PaintView(this);
    // Must be initialized after PaintViews are constructed
    brush_dialog_ = new BrushDialog(this);
    filter_kernel_dialog_ = new FilterKernelDialog(*right_view_, this);
    bilat_mean_dialog_ = new BilateralMeanDialog(*right_view_, this);
    bilat_gauss_dialog_ = new BilateralGaussDialog(*right_view_, this);

    CreateActions();
    CreateMenus();

    QScrollArea* scroll_area_left_ = new QScrollArea;
    scroll_area_left_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scroll_area_left_->setBackgroundRole(QPalette::Dark);
    scroll_area_left_->setWidget(left_view_);

    QScrollArea* scroll_area_right_ = new QScrollArea;
    scroll_area_right_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scroll_area_right_->setBackgroundRole(QPalette::Dark);
    scroll_area_right_->setWidget(right_view_);

    ui->horizontalLayout->setSpacing(CANVAS_SPACING);
    ui->horizontalLayout->addWidget(scroll_area_left_);
    ui->horizontalLayout->addWidget(scroll_area_right_);

    // Default Size
    ResizeCanvases(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);

    // Connect mouse events
    connect(right_view_, &PaintView::MousePress, this, &MainWindow::CanvasMousePressed);
    connect(right_view_, &PaintView::MouseMove, this, &MainWindow::CanvasMouseMoved);
    connect(right_view_, &PaintView::MouseRelease, this, &MainWindow::CanvasMouseReleased);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::InitializeContext() {
    QOpenGLContext* context = new QOpenGLContext();
    context->setShareContext(QOpenGLContext::globalShareContext());
    assert(context->create());

    QOffscreenSurface* surface = new QOffscreenSurface();
    surface->create();
    assert(surface->isValid());
    context->makeCurrent(surface);

    glewInit();
}

void MainWindow::CreateActions() {
    // Load reference image
    connect(ui->load_ref_action, &QAction::triggered, this, [this](){
	
        // if reference image is loaded, prompt user to save or discard current image
        if (reference_image_ != nullptr) {

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("Image may have been modified.");
            msgBox.setInformativeText("Do you want to save changes before opening new reference image?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);

            switch(msgBox.exec()) {
            case QMessageBox::Save: {
                // Save image to file, then open new image file
                QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), MainWindow::LastPath, "Image Files (*.jpg | *.jpeg | *.png | *.bmp)");
                if (!filename.isNull() && !filename.isEmpty()) {
                    MainWindow::LastPath = QFileInfo(filename).path();
                    auto snapshot = right_view_->GetSnapshot();
                    QImage image(snapshot->Bytes, snapshot->Width, snapshot->Height, QImage::Format_RGBA8888);
                    image.mirrored().save(filename);
                }
                break;
            }
            case QMessageBox::Discard:
                // No save, just discard any changes and open new image file
                break;
            case QMessageBox::Cancel:
                // do nothing
                return;
            }
        }

        QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), MainWindow::LastPath, "Image Files (*.jpg | *.jpeg | *.png | *.bmp)");
        if (!filename.isNull() && !filename.isEmpty()) {
            MainWindow::LastPath = QFileInfo(filename).path();

            // Load the image from file into RGBA32 format
            QImage unconverted(filename);
            QImage image = unconverted.convertToFormat(QImage::Format_RGBA8888);
            unsigned int width = image.width();
            unsigned int height = image.height();

            // Make sure we were able to load the image
            if (image.isNull()) {
                qDebug() << "Failed to import image \"" << filename << "\"";
                return;
            }

            // Keep a copy of the reference image
            if (reference_image_ != nullptr) delete[] reference_image_;
            reference_image_ = new unsigned char[image.byteCount()];
            memcpy(reference_image_, image.bits(), image.byteCount());
            reference_image_width_ = width;
            reference_image_height_ = height;

            // Construct the left and right hand side views
            left_view_->Setup(width, height);
            left_view_->CreateLayer(PaintView::BASE_LAYER);
            left_view_->CreateLayer(PaintView::OVERLAY_LAYER);

            right_view_->Setup(width, height);
            right_view_->CreateLayer(PaintView::BASE_LAYER);
            right_view_->CreateLayer(PaintView::OVERLAY_LAYER);

            ResizeCanvases(width, height);

            left_view_->SetCurrentLayer(PaintView::BASE_LAYER);
            left_view_->DrawImage(image.bits(), width, height, true);

            right_view_->SetCurrentLayer(PaintView::BASE_LAYER);
            right_view_->Clear(PaintView::RGBA_WHITE);

            // Enable the UI
            ui->save_canvas_action->setEnabled(true);
            ui->menu_edit->setEnabled(true);
            ui->menu_brushes->setEnabled(true);
            ui->menu_filter->setEnabled(true);
        }
    });

    // Save Canvas
    connect(ui->save_canvas_action, &QAction::triggered, this, [this](){
        QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), MainWindow::LastPath, "Image Files (*.jpg | *.jpeg | *.png | *.bmp)");
        if (!filename.isNull() && !filename.isEmpty()) {
            MainWindow::LastPath = QFileInfo(filename).path();
            auto snapshot = right_view_->GetSnapshot();
            QImage image(snapshot->Bytes, snapshot->Width, snapshot->Height, QImage::Format_RGBA8888);
            image.mirrored().save(filename);
        }
    });

    // Clear Canvas
    connect(ui->clear_canvas_action, &QAction::triggered, this, [this](){
        right_view_->SetCurrentLayer(PaintView::BASE_LAYER);
        right_view_->Clear(PaintView::RGBA_WHITE);
        right_view_->update();
    });

    // Copy Reference image to Canvas
    connect(ui->copy_ref_action, &QAction::triggered, this, [this](){
        right_view_->SetCurrentLayer(PaintView::BASE_LAYER);
        right_view_->DrawImage(reference_image_, reference_image_width_, reference_image_height_, true);
        right_view_->update();
    });

    // Brushes Dialog
    connect(ui->select_brush_action, &QAction::triggered, this, [this]() {
        brush_dialog_->show();
    });

    // Filters
    connect(ui->filter_kernel_action, &QAction::triggered, this, [this](){
        filter_kernel_dialog_->exec();
    });

    connect(ui->gaussian_blur_action, &QAction::triggered, this, [this](){
        auto snapshot = right_view_->GetSnapshot();
        // Allocate space for the filtered image
        unsigned int width = right_view_->GetWidth();
        unsigned int height = right_view_->GetHeight();
        RGBABuffer filtered(width, height);
        // Apply the Filter
        Filter::ApplyGaussianBlur(snapshot->Bytes, filtered.Bytes, width, height);
        right_view_->DrawImage(filtered.Bytes, width, height);
    });

    connect(ui->bilat_mean_action, &QAction::triggered, this, [this](){
        bilat_mean_dialog_->exec();
    });

    connect(ui->bilat_gauss_action, &QAction::triggered, this, [this](){
        bilat_gauss_dialog_->exec();
    });
}

void MainWindow::CreateMenus() {

}

void MainWindow::ResizeCanvases(unsigned int width, unsigned int height) {
    left_view_->setFixedSize(width, height);
    left_view_->update();
    right_view_->setFixedSize(width, height);
    right_view_->update();
    resize(left_view_->frameSize().width() + right_view_->frameSize().width() + CANVAS_MARGIN + CANVAS_SPACING,
           left_view_->frameSize().height() + CANVAS_MARGIN + ui->menu_bar->frameSize().height());
}

float start_x, start_y;

int MainWindow::calGradient(Brush& brush, glm::vec2 pos) {
    float pos_x = pos[0];
    float pos_y = pos[1];

    glm::vec2 tempPos = {pos_x - 1, pos_y - 1};
    glm::vec4 tempRGB = brush.GetColor(tempPos);
    float y1 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    tempPos = {pos_x, pos_y - 1};
    tempRGB = brush.GetColor(tempPos);
    float y2 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    tempPos = {pos_x + 1, pos_y - 1};
    tempRGB = brush.GetColor(tempPos);
    float y3 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    tempPos = {pos_x - 1, pos_y};
    tempRGB = brush.GetColor(tempPos);
    float y4 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    tempPos = {pos_x + 1, pos_y};
    tempRGB = brush.GetColor(tempPos);
    float y6 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    tempPos = {pos_x - 1, pos_y + 1};
    tempRGB = brush.GetColor(tempPos);
    float y7 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    tempPos = {pos_x, pos_y + 1};
    tempRGB = brush.GetColor(tempPos);
    float y8 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    tempPos = {pos_x + 1, pos_y + 1};
    tempRGB = brush.GetColor(tempPos);
    float y9 = tempRGB.r * 0.299 + tempRGB.g * 0.578 + tempRGB.b * 0.114;

    float sx = y1 * (-1) + y3 + y4 * (-2) + y6 * 2 + y7 * (-1) + y9;
    float sy = y1  + y2 * 2 + y3 - y7 - 2 * y8 - y9;

    float angle = 90 - atan2(sy, sx) * 180 / M_PI; // 90 dgrees to gradient angle

    while (angle < 0) {
        angle += 360;
    }

    return angle;
}


int smoothFactor = 5;
std::vector<float> angles;

void MainWindow::CanvasMousePressed(QMouseEvent *event) {
    float pos_x = event->pos().x();
    float pos_y = event->pos().y();
    glm::vec2 pos = glm::vec2(pos_x, pos_y);
    mouse_buttons_ = event->buttons();
    angles.clear(); // should it be cleared?

    if (mouse_buttons_.testFlag(Qt::LeftButton)) {
        // Sample the Color from the reference image
        Brush& current_brush = brush_dialog_->GetCurrentBrush();
        current_brush.SetColorMode(ColorMode::Sample);
        current_brush.SetColorImage(reference_image_, reference_image_width_, reference_image_height_);
        right_view_->SetCurrentLayer(PaintView::BASE_LAYER);
        right_view_->DrawBegin(current_brush, pos);
        // REQUIREMENT: Set brush angle if needed.
        if(brush_dialog_->GetCurrentAngleControl() == AngleMode::CursorMovement) {
            start_x = pos_x;
            start_y = pos_y;
            angles.insert(angles.begin(), current_brush.GetAngle());
        }
        if (brush_dialog_->GetCurrentAngleControl() == AngleMode::Gradient) {
            current_brush.SetAngle(calGradient(current_brush, pos));
        }
    } else if (mouse_buttons_.testFlag(Qt::RightButton)) {
        right_view_->SetCurrentLayer(PaintView::OVERLAY_LAYER);
        angle_indicator_brush_.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
        right_view_->DrawBegin(angle_indicator_brush_, pos);
        start_x = pos_x;
        start_y = pos_y;
    }
}

void MainWindow::CanvasMouseMoved(QMouseEvent *event) {
    float pos_x = event->pos().x();
    float pos_y = event->pos().y();
    glm::vec2 pos = glm::vec2(pos_x, pos_y);
    mouse_buttons_ = event->buttons();

    if (mouse_buttons_.testFlag(Qt::LeftButton)) {
        // Sample the Color from the reference image
        Brush& current_brush = brush_dialog_->GetCurrentBrush();
        current_brush.SetColorMode(ColorMode::Sample);
        current_brush.SetColorImage(reference_image_, reference_image_width_, reference_image_height_);
        right_view_->SetCurrentLayer(PaintView::BASE_LAYER);
        right_view_->DrawMove(current_brush, pos);
        // REQUIREMENT: Set brush angle if needed.
        // Also consider the previous "smoothFactor" number of angles, average them
        if (brush_dialog_->GetCurrentAngleControl() == AngleMode::CursorMovement) {
            float dX = pos_x - start_x;
            float dY = pos_y - start_y;

            float newAngle = atan2(dY, dX) * 180 / M_PI;

            newAngle = newAngle > 0? newAngle : newAngle + 360;

            float totalSin = sin(newAngle * M_PI / 180) / (angles.size() + 1);
            float totalCos = cos(newAngle * M_PI / 180) / (angles.size() + 1);

            for(int i = 0; i < angles.size(); i++) {
               totalSin += sin(angles[i] * M_PI / 180) / (angles.size() + 1);
               totalCos += cos(angles[i] * M_PI / 180) / (angles.size() + 1);

            }

            float smoothed = atan2(totalSin, totalCos)* 180 / M_PI;
            smoothed = smoothed > 0? smoothed : smoothed + 360;

            if(angles.size() >= smoothFactor) {
                angles.pop_back();
            }
            angles.insert(angles.begin(), newAngle);

            start_x = pos_x;
            start_y = pos_y;
            brush_dialog_->GetCurrentBrush().SetAngle((int)smoothed);
        }
        if (brush_dialog_->GetCurrentAngleControl() == AngleMode::Gradient) {
            current_brush.SetAngle(calGradient(current_brush, pos));
        }
    } else if (mouse_buttons_.testFlag(Qt::RightButton)) {
        right_view_->SetCurrentLayer(PaintView::OVERLAY_LAYER);
        right_view_->Clear(PaintView::RGBA_TRANSPARENT);
        angle_indicator_brush_.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
        right_view_->DrawMove(angle_indicator_brush_, pos);
    }

    // EXTRA CREDIT: Draw an overlay marker on the left view
    left_view_->SetCurrentLayer(PaintView::OVERLAY_LAYER);
    left_view_->Clear(PaintView::RGBA_TRANSPARENT);
    marker_brush_.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
    marker_brush_.SetSize(4);
    left_view_->DrawMove(marker_brush_, pos);
}

void MainWindow::CanvasMouseReleased(QMouseEvent *event) {
    float pos_x = event->pos().x();
    float pos_y = event->pos().y();
    glm::vec2 pos = glm::vec2(pos_x, pos_y);

    if (mouse_buttons_.testFlag(Qt::LeftButton)) {
        // Sample the Color from the reference image
        Brush& current_brush = brush_dialog_->GetCurrentBrush();
        current_brush.SetColorMode(ColorMode::Sample);
        current_brush.SetColorImage(reference_image_, reference_image_width_, reference_image_height_);
        right_view_->SetCurrentLayer(PaintView::BASE_LAYER);
        right_view_->DrawEnd(current_brush, pos);
    }

    if (mouse_buttons_.testFlag(Qt::RightButton)) {
        right_view_->SetCurrentLayer(PaintView::OVERLAY_LAYER);
        right_view_->Clear(PaintView::RGBA_TRANSPARENT);
        // REQUIREMENT: Set brush angle if needed.
        float dX = start_x - pos_x;
        float dY = start_y - pos_y;
        float newSize = sqrt(pow(dX, 2) + pow(dY, 2));
        brush_dialog_->GetCurrentBrush().SetSize((int)newSize);

        float newAngle = atan2(dY, dX) * 180 / M_PI;
        while (newAngle < 0) {
            newAngle += 360;
        }
        (brush_dialog_->GetCurrentBrush()).SetAngle((int)newAngle);
    }

    // Reset mouse buttons state
    mouse_buttons_ = 0;
}
