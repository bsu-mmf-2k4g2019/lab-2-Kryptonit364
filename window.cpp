#include "glwidget.h"
#include "window.h"
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QMessageBox>

Window::Window(QWidget *parent) : QWidget(parent)
{
    glWidget = new GLWidget;
    rotationChanger = new QPushButton("Ручное вращение", this);

    connect(rotationChanger, SIGNAL(clicked()), glWidget, SLOT(setRotationType()));
    connect(rotationChanger, SIGNAL(clicked()), this, SLOT(rotationTextChanger()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *container = new QHBoxLayout; //Окошко ГЛ, и функционал справа
    QVBoxLayout *additional = new QVBoxLayout; //Пара тестовых кнопок
    additional->setSizeConstraint(QLayout::SetFixedSize);
    container->addWidget(glWidget);

    QWidget *w = new QWidget;
    w->setLayout(container);
    mainLayout->addWidget(w);
    mainLayout->addWidget(rotationChanger);

    setLayout(mainLayout);

    setWindowTitle("3D ObJects");
    QMessageBox::information(nullptr, "Небольшой гайд",
                             "<html><h2>Внимание! </h2>\n"
                             "<html><u>WASD</u> - для перемещения камеры в плоскости параллельной объектам.<br>"
                             "<html><u>Mouse scroll</u> - для перемещения камеры от / к пользователю.<br><br>"
                             "<html><u>ПКМ / ЛКМ</u> - для вращения объектов в ручном режиме.<br><br>"
                             "Для переключения режима вращения используйте <html><u>кнопку</u> под GLWidget'ом.<br>"
                             "<html><u>Esc</u> - для выхода из программы.");
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    else
        glWidget->keyPressEvent(event);
}
void Window::rotationTextChanger() {
    if (glWidget->autoRotate) {
        rotationChanger->setText("Ручное вращение");
    }
    else {
        rotationChanger->setText("Автоматическое вращение");
    }
}
QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}
