#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    // Represents the format of a renderable surface
    // NOTE: If not set as default, Mac renders as black screen
    QSurfaceFormat glFormat;
    glFormat.setRenderableType( QSurfaceFormat::OpenGL );
    glFormat.setMajorVersion( 4 );
    glFormat.setMinorVersion( 1 );
    glFormat.setProfile( QSurfaceFormat::CoreProfile ); // Functionality deprecated in OpenGL 3.0 is not available.
    glFormat.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
//    glFormat.setStencilBufferSize( 8 );
//    glFormat.setSamples(4);
    QSurfaceFormat::setDefaultFormat(glFormat);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    // High DPI Support
//    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
