#include "glwidget.h"

#include <QImage>
#include <QTime>
//#include <iostream>

static const char *vertexShaderSource_container =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCol;\n"
    "layout (location = 2) in vec2 aTex;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "    ourColor = aCol;\n"
    "    TexCoord = aTex;\n"
    "}\n\0";

static const char *vertexShaderSource_pyramid =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCol;\n"
    "layout (location = 2) in vec2 aTex;\n"
    "layout (location = 3) in float tType;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "out float TexType;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "    ourColor = aCol;\n"
    "    TexCoord = aTex;\n"
    "    TexType = tType;\n"
    "}\n\0";

static const char *fragmentShaderSource_container =
    "#version 330 core\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D mTexture;\n"
    "void main()\n"
    "{\n"
    "    FragColor = mix(texture(mTexture, TexCoord), vec4(ourColor, 1.0), 0.35);\n"
    "}\n\0";

static const char *fragmentShaderSource_pyramid =
    "#version 330 core\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "in float TexType;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D mTexture_c;\n"
    "uniform sampler2D mTexture_t;\n"
    "void main()\n"
    "{\n"
    "   if (TexType == 1.0f)\n"
    "       FragColor = texture(mTexture_c, TexCoord);\n"
    "   else\n"
    "       if (TexType == 0.0f)\n"
    "           FragColor = texture(mTexture_t, TexCoord);\n"
    //"       else FragColor = vec4(ourColor, 1.0);\n"
    "}\n\0";

static const char *fragmentShaderSource_tower =
    "#version 330 core\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D mTexture;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(mTexture, TexCoord);\n"
    "}\n\0";

static QVector3D cubePositions[] = {
  QVector3D( 0.0f,  0.0f,  0.0f),
  QVector3D( 2.0f,  5.0f, -15.0f),
  QVector3D(-1.5f, -2.2f, -2.5f),
  QVector3D(-3.8f, -2.0f, -12.3f),
  QVector3D( 2.4f, -0.4f, -3.5f),
  QVector3D(-1.7f,  3.0f, -7.5f),
};
static QVector3D pyramidPositions[] = {
   QVector3D( 1.5f,  0.2f, -1.5f),
   QVector3D(-1.3f,  1.0f, -1.5f)
};
static QVector3D towerPositions[] = {
    QVector3D( 1.3f, -2.0f, -2.5f),
    QVector3D( 1.5f,  2.0f, -2.5f),
};

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), camera_up(0.0f, 1.0f, 0.0f), camera_front(0.0f, 0.0f, -1.0f) {
    autoRotate = true;
    t_x = t_y = t_z = 0;
    timerId = this->startTimer(0);
}
GLWidget::~GLWidget()
{}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}
QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::timerEvent(QTimerEvent *event)
{
    update();
    event->accept();
}
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    float cameraSpeed = 0.30f; // adjust accordingly
    if (event->key() == Qt::Key_W || event->text() == "ц" || event->text() == "Ц")
        camera_pos += cameraSpeed * camera_up;
    if (event->key() == Qt::Key_S || event->text() == "ы" || event->text() == "Ы")
        camera_pos -= cameraSpeed * camera_up;
    if (event->key() == Qt::Key_A || event->text() == "ф" || event->text() == "Ф")
        camera_pos -= QVector3D::crossProduct(camera_front, camera_up).normalized() * cameraSpeed;
    if (event->key() == Qt::Key_D || event->text() == "в" || event->text() == "В")
        camera_pos += QVector3D::crossProduct(camera_front, camera_up).normalized() * cameraSpeed;
    update();
}
void GLWidget::wheelEvent(QWheelEvent* event) {
    QPoint numDegrees = event->angleDelta() / 8;
    float cameraSpeed = 0.30f;
    if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        QVector3D numSteps3D(numSteps.x(), 0, numSteps.y());
        camera_pos += (-1.0f) * numSteps3D * cameraSpeed;
    }
    update();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}
void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        //emit xRotationChanged(angle);
        update();
    }
}
void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        //emit yRotationChanged(angle);
        update();
    }
}
void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        //emit zRotationChanged(angle);
        update();
    }
}
void GLWidget::setRotationType(){
    if (autoRotate) {
        autoRotate = 0;
        if (timerId > 0)
            this->killTimer(timerId);
    }
    else {
        autoRotate = 1;
        timerId = this->startTimer(0);
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    // Create VAO & VBO, bind it to current GL_ARRAY_BUFFER and load vertices into it
    GLfloat vertices_pyramid[] = {
        // positions          // colors           // texture coords & texture type (1 - cube, 0 - triangle)
        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,    1.0f,   // top right
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,    1.0f,   // bottom right
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,    1.0f,   // bottom left
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,    1.0f,   // top left

        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,    0.0f,   //1-2
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,    0.0f,
        0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,   0.5f, 1.0f,    0.0f,

        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,    0.0f,   //2-3
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,    0.0f,
        0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,   0.5f, 1.0f,    0.0f,

       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,    0.0f,   //3-4
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,    0.0f,
        0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,   0.5f, 1.0f,    0.0f,

       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,    0.0f,   //4-1
        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,    0.0f,
        0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,   0.5f, 1.0f,    0.0f,
    };
    GLuint indices_pyramid[] = {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        7, 8, 9,
        10, 11, 12,
        13, 14, 15
    };

    GLfloat vertices_container[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   // top left

       -0.5f,  0.5f,-0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 1.0f,   // top right
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f,-0.5f,   1.0f, 0.0f, 0.5f,   0.0f, 1.0f,   // top left

       -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
       -0.5f,  0.5f,-0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
        0.5f,  0.5f,-0.5f,   1.0f, 0.0f, 0.5f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   // top left

        0.5f,  0.5f,-0.5f,   1.0f, 0.0f, 0.5f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   // top left

       -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f,-0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 1.0f,   // top left
    };
    GLuint indices_container[] = {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23,
    };

    GLfloat vertices_tower[] = {
         0.5f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, //Нижнее основание
        -0.5f, -1.0f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.5f, -1.0f, 0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,

         0.5f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f, //Верхнее основание
        -0.5f, 1.0f,-0.5f,    0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
        -0.5f, 1.0f, 0.5f,    1.0f, 0.0f, 1.0f,   0.0f, 0.0f,

         0.5f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, //1-2
        -0.5f, -1.0f,-0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, 1.0f,-0.5f,    0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
         0.5f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

        -0.5f, -1.0f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f, //2-3
        -0.5f, -1.0f, 0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, 1.0f, 0.5f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, 1.0f,-0.5f,    0.0f, 1.0f, 1.0f,   1.0f, 1.0f,

         0.5f, 1.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, //1-3
        -0.5f, 1.0f, 0.5f,    1.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f, -1.0f, 0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
         0.5f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
    };
    GLuint indices_tower[] = {
        0, 1, 2,

        3, 4, 5,

        6, 7, 8,
        6, 8, 9,

        10, 11, 12,
        10, 12, 13,

        14, 15, 16,
        14, 16, 17,
    };

    GLuint VBO;
    GLuint EBO;

    // Create VAO ids
    glGenVertexArrays(1, &m_vao_container_id);
    glGenVertexArrays(1, &m_vao_pyramid_id);
    glGenVertexArrays(1, &m_vao_tower_id);

    // Fill data for the container
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind container VAO to store all buffer settings related to container object
    glBindVertexArray(m_vao_container_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_container), vertices_container, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_container), indices_container, GL_STATIC_DRAW);
    // Configure how OpenGL will interpret the VBO data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind current VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind current EBO

    // Fill data for the container
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Pyramide
    glBindVertexArray(m_vao_pyramid_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_pyramid), vertices_pyramid, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_pyramid), indices_pyramid, GL_STATIC_DRAW);
    // Configure how OpenGL will interpret the VBO data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind current VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind current EBO

    // Fill data for the container
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Tower
    glBindVertexArray(m_vao_tower_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_tower), vertices_tower, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_tower), indices_tower, GL_STATIC_DRAW);
    // Configure how OpenGL will interpret the VBO data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind current VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind current EBO

    // Prepare textures
    QImage cube(":/img/cube.jpg");
    cube = cube.convertToFormat(QImage::Format_RGB888);
    glGenTextures(1, &m_texture_cube_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_cube_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cube.width(), cube.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, cube.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    QImage triangle(":/img/triangle.jpg");
    triangle = triangle.convertToFormat(QImage::Format_RGB888).mirrored(false, true);
    glGenTextures(1, &m_texture_triangle_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_triangle_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, triangle.width(), triangle.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, triangle.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    QImage grass(":/img/grass.jpg");
    grass = grass.convertToFormat(QImage::Format_RGB888);
    glGenTextures(1, &m_texture_grass_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_grass_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grass.width(), grass.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, grass.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Prepare shader programms
    m_prog_container.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource_container);
    m_prog_container.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource_container);
    m_prog_container.link();

    m_prog_pyramid.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource_pyramid);
    m_prog_pyramid.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource_pyramid);
    m_prog_pyramid.link();

    m_prog_tower.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource_container);
    m_prog_tower.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource_tower);
    m_prog_tower.link();
}
void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::noTime(int &t_x, int &t_y, int &t_z){
    QTime cur_t = QTime::currentTime();
    int r, temp = (int) ((cur_t.second() * 1000 + cur_t.msec()));
    if (temp < 4000)
        r = (int) (0.04 * temp);
    else
        if (temp >= 4000 && temp < 17000)
            r = (int) (0.006 * temp);
        else
            if (temp >= 17000 && temp < 26500)
                r = (int) (0.003 * temp);
            else
                r = (int) (0.001 * temp);
    //std::cout << cur_t.second() << " : " << cur_t.msec() << " - " << temp << "\n";
    t_x += r; t_y += r; t_z += r;
}
void GLWidget::paintGL()
{
    noTime(t_x, t_y, t_z);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_cube_id);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture_triangle_id);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture_grass_id);

    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;

    view.lookAt(camera_pos, camera_pos + camera_front, camera_up);
    projection.perspective(45.0f, width() / height(), 0.1f, 100.0f);

    glBindVertexArray(m_vao_container_id);
    m_prog_container.bind();
    m_prog_container.setUniformValue("mTexture", 0);
    m_prog_container.setUniformValue("view", view);
    m_prog_container.setUniformValue("projection", projection);
    for (size_t i = 0; i < 6; i++) {
        model.setToIdentity();
        model.translate(cubePositions[i]);
        if (autoRotate){
            model.rotate(180.0f - (t_x / 16.0f), 1.0f, 0.0f, 0.0f);
            model.rotate(t_y / 16.0f, 0.0f, 1.0f, 0.0f);
            model.rotate(t_z / 16.0f, 0.0f, 0.0f, 1.0f);
            m_xRot = t_x; m_yRot = t_y; m_zRot = t_z;
        }
        else {
            model.rotate(180.0f - (m_xRot / 16.0f), 1.0f, 0.0f, 0.0f);
            model.rotate(m_yRot / 16.0f, 0.0f, 1.0f, 0.0f);
            model.rotate(m_zRot / 16.0f, 0.0f, 0.0f, 1.0f);
            t_x = m_xRot; t_y = m_yRot; t_z = m_zRot;
        }
        m_prog_container.setUniformValue("model", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(m_vao_tower_id);
    m_prog_tower.bind();
    m_prog_tower.setUniformValue("mTexture", 2);
    m_prog_tower.setUniformValue("view", view);
    m_prog_tower.setUniformValue("projection", projection);
    for (size_t i = 0; i < 2; i++) {
        model.setToIdentity();
        model.translate(towerPositions[i]);
        if (autoRotate){
            model.rotate(180.0f - (t_x / 16.0f), 1.0f, 0.0f, 0.0f);
            model.rotate(t_y / 16.0f, 0.0f, 1.0f, 0.0f);
            model.rotate(t_z / 16.0f, 0.0f, 0.0f, 1.0f);
            m_xRot = t_x; m_yRot = t_y; m_zRot = t_z;
        }
        else {
            model.rotate(180.0f - (m_xRot / 16.0f), 1.0f, 0.0f, 0.0f);
            model.rotate(m_yRot / 16.0f, 0.0f, 1.0f, 0.0f);
            model.rotate(m_zRot / 16.0f, 0.0f, 0.0f, 1.0f);
            t_x = m_xRot; t_y = m_yRot; t_z = m_zRot;
        }
        m_prog_tower.setUniformValue("model", model);
        glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(m_vao_pyramid_id);
    m_prog_pyramid.bind();
    m_prog_pyramid.setUniformValue("mTexture_c", 0);
    m_prog_pyramid.setUniformValue("mTexture_t", 1);
    m_prog_pyramid.setUniformValue("view", view);
    m_prog_pyramid.setUniformValue("projection", projection);
    for (size_t i = 0; i < 2; i++) {
        model.setToIdentity();
        model.translate(pyramidPositions[i]);
        if (autoRotate){
            model.rotate(180.0f - (t_x / 16.0f), 1.0f, 0.0f, 0.0f);
            model.rotate(t_y / 16.0f, 0.0f, 1.0f, 0.0f);
            model.rotate(t_z / 16.0f, 0.0f, 0.0f, 1.0f);
            m_xRot = t_x; m_yRot = t_y; m_zRot = t_z;
        }
        else {
            model.rotate(180.0f - (m_xRot / 16.0f), 1.0f, 0.0f, 0.0f);
            model.rotate(m_yRot / 16.0f, 0.0f, 1.0f, 0.0f);
            model.rotate(m_zRot / 16.0f, 0.0f, 0.0f, 1.0f);
            t_x = m_xRot; t_y = m_yRot; t_z = m_zRot;
        }
        m_prog_pyramid.setUniformValue("model", model);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr);
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (autoRotate) return;
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    }
    else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}
