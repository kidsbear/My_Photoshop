#include "paintview.h"
#include <vectors.h>
#include <assert.h>
#include <QDebug>
#include <brushes/brush.h>

const unsigned int PaintView::BASE_LAYER = 0;
const unsigned int PaintView::OVERLAY_LAYER = 1000;
const glm::vec4 PaintView::RGBA_WHITE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 PaintView::RGBA_TRANSPARENT = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

// To support DPI Scaling, framebuffer size is different than the default framebuffer (aka window) size.
// However this is hidden from us since scaling is done automatically.
PaintView::PaintView(QWidget *parent) :
    QOpenGLWidget(parent),
    current_layer_(nullptr),
    width_(0),
    height_(0)
{
    setMouseTracking(true);
}

void PaintView::DrawImage(const unsigned char* image, unsigned int width, unsigned int height, bool flipped) {
    if(current_layer_ == nullptr) return;

    makeCurrent();
    current_layer_->Framebuffer().bind();

    // Blending mode
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    // Draw with the fullscreen quad
    glBindVertexArray(canvas_vertex_array_);
    glUseProgram(canvas_shader_);

    // DPI projection (if framebuffer size is smaller than default framebuffer)
    GLint uniform_loc = glGetUniformLocation(canvas_shader_, "projection_matrix");
    glm::mat4 dpi_proj = flipped ? dpi_proj_flipped_ : dpi_proj_;
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(dpi_proj));

    // Load the data into the GPU buffer
    glBindTexture(GL_TEXTURE_2D, canvas_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Draw the quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    update();
}

std::unique_ptr<RGBABuffer> PaintView::GetSnapshot() {
    makeCurrent();

    QImage image = current_layer_->Framebuffer().toImage().convertToFormat(QImage::Format_RGBA8888);
    std::unique_ptr<RGBABuffer> snapshot = std::make_unique<RGBABuffer>(image.width(), image.height());
    memcpy(snapshot->Bytes, image.constBits(), image.byteCount());
    return std::move(snapshot);
}

void PaintView::Setup(unsigned int width, unsigned int height) {
    makeCurrent();

    width_ = width;
    height_ = height;

    // Clear all layers
    layers_.clear();
    current_layer_ = nullptr;

    // Fullscreen projection matrix
    canvas_proj_ = glm::ortho(0.0f, float(width_), float(height_), 0.0f);
    // Framebuffer projection matrix compensating for DPI Scaling
    float device_pixel_ratio = devicePixelRatio();
    dpi_proj_ = glm::ortho(0.0f, width_ * device_pixel_ratio, float(height_), height_ - height_ * device_pixel_ratio);
    dpi_proj_flipped_ = glm::ortho(0.0f, width_ * device_pixel_ratio, 0.0f, height_ * device_pixel_ratio);

    ResizeFullscreenQuad();
}

void PaintView::CreateLayer(unsigned int layer_num, glm::vec4 clear_color) {
    makeCurrent();

    layers_[layer_num] = std::make_unique<Layer>(width_, height_);
    layers_[layer_num]->Framebuffer().bind();

    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);

    if(current_layer_ != nullptr) current_layer_->Framebuffer().bind();
}

void PaintView::SetCurrentLayer(unsigned int layer_num) {
    // TODO: Throw an error instead of failing silently
    if (layers_.count(layer_num) == 0) return;

    makeCurrent();

    current_layer_ = layers_[layer_num].get();
}

void PaintView::Clear(glm::vec4 clear_color) {
    if(current_layer_ == nullptr) return;

    makeCurrent();
    current_layer_->Framebuffer().bind();

    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);

    update();
}

unsigned int PaintView::GetWidth() {
    return width_;
}

unsigned int PaintView::GetHeight() {
    return height_;
}

void PaintView::mouseMoveEvent(QMouseEvent* event) {
    emit MouseMove(event);
}

void PaintView::mousePressEvent(QMouseEvent* event) {
    emit MousePress(event);
}

void PaintView::mouseReleaseEvent(QMouseEvent* event) {
    emit MouseRelease(event);
}

void PaintView::DrawBegin(Brush &b, glm::vec2 pos) {
    if(current_layer_ == nullptr) return;

    makeCurrent();
    current_layer_->Framebuffer().bind();

    PrepareBrush(b);
    b.BrushBegin(pos);

    update();
}

void PaintView::DrawMove(Brush &b, glm::vec2 pos) {
    if(current_layer_ == nullptr) return;

    makeCurrent();
    current_layer_->Framebuffer().bind();

    PrepareBrush(b);
    b.BrushMove(pos);

    update();
}

void PaintView::DrawEnd(Brush &b, glm::vec2 pos) {
    if(current_layer_ == nullptr) return;

    makeCurrent();
    current_layer_->Framebuffer().bind();

    PrepareBrush(b);
    b.BrushEnd(pos);

    update();
}

void PaintView::makeCurrent() {
    QOpenGLWidget::makeCurrent();
    // glewInit should be called everytime context changes
    // See: http://stackoverflow.com/questions/35683334/call-glewinit-once-for-each-rendering-context-or-exactly-once-for-the-whole-app
    glewInit();
}

void PaintView::initializeGL() {
    glewInit();
    context()->setShareContext(QOpenGLContext::globalShareContext());

    // Single-shot Initialization
    SetupBrushShader();
    SetupCanvasShader();
    SetupFullscreenQuad();
    SetupBrushes();

    // Clear to a dark grey
    glClearColor(0.62745f, 0.62745f, 0.62745f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PaintView::paintGL() {
    if(current_layer_ == nullptr) return;

    // Clear before drawing
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Alpha blend layers together
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(canvas_vertex_array_);
    glUseProgram(canvas_shader_);

    // Fullscreen projection
    GLint uniform_loc = glGetUniformLocation(canvas_shader_, "projection_matrix");
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(canvas_proj_));

    ResizeFullscreenQuad();

    // Layers are rendered in ascending order of layer number
    for (auto& kv : layers_) {
        glBindTexture(GL_TEXTURE_2D, kv.second->Framebuffer().texture());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void PaintView::SetupBrushShader() {
    GLuint brush_vert_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* brush_vert_cstr = brush_vert_source_.c_str();
    glShaderSource(brush_vert_shader, 1, &brush_vert_cstr, NULL);
    glCompileShader(brush_vert_shader);

    GLuint brush_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* brush_frag_cstr = brush_frag_source_.c_str();
    glShaderSource(brush_frag_shader, 1, &brush_frag_cstr, NULL);
    glCompileShader(brush_frag_shader);

    brush_shader_ = glCreateProgram();
    glAttachShader(brush_shader_, brush_vert_shader);
    glAttachShader(brush_shader_, brush_frag_shader);
    glBindAttribLocation(brush_shader_, 0, "position");
    glLinkProgram(brush_shader_);
}

void PaintView::SetupCanvasShader() {
    GLuint canvas_vert_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* canvas_vert_cstr = canvas_vert_source_.c_str();
    glShaderSource(canvas_vert_shader, 1, &canvas_vert_cstr, NULL);
    glCompileShader(canvas_vert_shader);

    GLuint canvas_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* canvas_frag_cstr = canvas_frag_source_.c_str();
    glShaderSource(canvas_frag_shader, 1, &canvas_frag_cstr, NULL);
    glCompileShader(canvas_frag_shader);

    canvas_shader_ = glCreateProgram();
    glAttachShader(canvas_shader_, canvas_vert_shader);
    glAttachShader(canvas_shader_, canvas_frag_shader);
    glBindAttribLocation(canvas_shader_, 0, "position");
    glBindAttribLocation(canvas_shader_, 1, "texcoord");
    glLinkProgram(canvas_shader_);
}

void PaintView::SetupFullscreenQuad() {
    // VAO
    glGenVertexArrays(1, &canvas_vertex_array_);
    glBindVertexArray(canvas_vertex_array_);

    // Positions
    glGenBuffers(1, &canvas_pos_buffer_);
    ResizeFullscreenQuad();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    std::vector<GLfloat> quad_uv = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    // UV
    glGenBuffers(1, &canvas_uv_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, canvas_uv_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quad_uv.size(), quad_uv.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Texture used for drawing to fullscreen quad
    glGenTextures(1, &canvas_texture_);
    glBindTexture(GL_TEXTURE_2D, canvas_texture_);
    // Set texture properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void PaintView::ResizeFullscreenQuad() {
    std::vector<GLfloat> quad_pos = {
        0.0f, 0.0f,
        float(width_), 0.0f,
        0.0f, float(height_),
        float(width_), float(height_)
    };

    glBindBuffer(GL_ARRAY_BUFFER, canvas_pos_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quad_pos.size(), quad_pos.data(), GL_STATIC_DRAW);
}

void PaintView::SetupBrushes() {
    // Create vertex array and vertex buffer used by brushes
    glGenVertexArrays(1, &brush_vertex_array_);
    glBindVertexArray(brush_vertex_array_);
    glGenBuffers(1, &brush_pos_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, brush_pos_buffer_);
    // Enable the "position" vertex attribute
    glEnableVertexAttribArray(0); // TODO: Not hardcoded
    // Specify the vertex format
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void PaintView::PrepareBrush(Brush& b) const {
    glEnable(GL_BLEND);
    // REQUIREMENT: Alpha Blend the RGB color for the Brush (don't modify the alpha channel)
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    // DPI projection
    glUseProgram(brush_shader_);
    GLint uniform_loc = glGetUniformLocation(brush_shader_, "projection_matrix");
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(dpi_proj_flipped_));

    glBindVertexArray(brush_vertex_array_);
    glBindBuffer(GL_ARRAY_BUFFER, brush_pos_buffer_);
    b.SetColorLocation(glGetUniformLocation(brush_shader_, "brush_color"));
}
