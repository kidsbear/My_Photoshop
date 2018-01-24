#ifndef PAINTVIEW_H
#define PAINTVIEW_H

#include <glinclude.h>
#include <vectors.h>
#include <unordered_map>
#include <memory>
#include <rgbabuffer.h>
#include <layer.h>

class Brush;

class PaintView : public QOpenGLWidget {
    Q_OBJECT
public:
    static const unsigned int BASE_LAYER;
    static const unsigned int OVERLAY_LAYER;
    static const glm::vec4 RGBA_WHITE;
    static const glm::vec4 RGBA_TRANSPARENT;

    // Before using an OpenGL calls, the makeCurrent should be called.
    PaintView(QWidget* parent = Q_NULLPTR);

    // Transfers the image onto the paint view. Flips the image vertically while doing so.
    void DrawImage(const unsigned char* image, unsigned int width, unsigned int height, bool flipped = false);

    // Returns a deep copy of an image of the current layer.
    // Note: This is an extremely expensive operation reading from GPU memory, use sparingly.
    std::unique_ptr<RGBABuffer> GetSnapshot();

    // Resets everything, clears all layers
    void Setup(unsigned int width, unsigned int height);

    // Creates a new layer cleared with a color
    void CreateLayer(unsigned int layer_num, glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Sets the current layer
    void SetCurrentLayer(unsigned int layer_num);

    // Clears the current layer
    void Clear(glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Size of the PaintView
    unsigned int GetWidth();
    unsigned int GetHeight();

    // Forward signals to MainWindow
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    // Draws with the brush on the current layer
    void DrawBegin(Brush& b, glm::vec2 pos);
    void DrawMove(Brush& b, glm::vec2 pos);
    void DrawEnd(Brush& b, glm::vec2 pos);

signals:
    void MouseMove(QMouseEvent* event);
    void MousePress(QMouseEvent* event);
    void MouseRelease(QMouseEvent* event);

protected:
    const std::string brush_vert_source_ =
        "#version 150\n"
        "in vec2 position;"
        "uniform mat4 projection_matrix;"
        "void main() {"
        "   gl_Position = projection_matrix * vec4(position, 0.0, 1.0);"
        "}";

    const std::string brush_frag_source_ =
        "#version 150\n"
        "out vec4 outColor;"
        "uniform vec4 brush_color;"
        "void main() {"
        "   outColor = brush_color;"
        "}";

    const std::string canvas_vert_source_ =
        "#version 150\n"
        "in vec2 position;"
        "in vec2 texcoord;"
        "out vec2 uv;"
        "uniform mat4 projection_matrix;"
        "void main() {"
        "   uv = texcoord;"
        "   gl_Position = projection_matrix * vec4(position, 0.0, 1.0);"
        "}";

    const std::string canvas_frag_source_ =
        "#version 150\n"
        "in vec2 uv;"
        "out vec4 outColor;"
        "uniform sampler2D canvas;"
        "void main() {"
        "   outColor = texture(canvas, uv);"
        "}";

    void makeCurrent();
    virtual void initializeGL() override;
    virtual void paintGL() override;
    // Single-shot GL initialization
    void SetupBrushShader();
    void SetupCanvasShader();
    void SetupFullscreenQuad();
    void ResizeFullscreenQuad();
    void SetupBrushes();
    // Called right before using the brush
    void PrepareBrush(Brush& b) const;

    // Layers are keyed by their layer number
    std::map<unsigned int, std::unique_ptr<Layer>> layers_;
    Layer* current_layer_;
    GLuint brush_vertex_array_;
    GLuint brush_pos_buffer_;
    GLuint canvas_vertex_array_;
    GLuint canvas_pos_buffer_;
    GLuint canvas_uv_buffer_;
    GLuint canvas_texture_;
    GLuint brush_shader_;
    GLuint canvas_shader_;
    unsigned int width_;
    unsigned int height_;

    glm::mat4 canvas_proj_;
    glm::mat4 dpi_proj_;
    glm::mat4 dpi_proj_flipped_;
};

#endif // PAINTVIEW_H
