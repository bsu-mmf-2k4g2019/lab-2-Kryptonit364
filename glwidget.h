#ifndef WIDGET_H
#define WIDGET_H

#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void keyPressEvent(QKeyEvent *event) override; //Перемещён в public, т. к. вызывается из window
    bool autoRotate;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setRotationType();

/*signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);*/

protected:
    void timerEvent(QTimerEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

private:    
    //For rotation using mouse
    int m_xRot;
    int m_yRot;
    int m_zRot;
    QPoint m_lastPos;
    int timerId;

    // Shader programms
    QOpenGLShaderProgram m_prog_container;
    QOpenGLShaderProgram m_prog_pyramid;

    // VAOs
    GLuint m_vao_container_id;
    GLuint m_vao_pyramid_id;
    GLuint m_vao_tower_id;

    // Textures
    GLuint m_texture_cube_id;
    GLuint m_texture_triangle_id;
    GLuint m_texture_grass_id;
    GLuint m_texture_dirt_id;

    // Uniforms
    //GLint  m_triangle_color_id;

    QVector3D camera_pos;
    QVector3D camera_up;
    QVector3D camera_front;

    QTime frame_delta_time;
    QTime last_frame_time;

    //Instead of time in paintGL
    void noTime(int&, int&, int&);
    int t_x;
    int t_y;
    int t_z;
};

#endif // WIDGET_H
