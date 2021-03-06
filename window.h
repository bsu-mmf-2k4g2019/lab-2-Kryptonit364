#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSlider;
class QPushButton;
QT_END_NAMESPACE
class GLWidget;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void rotationTextChanger();

private:
    GLWidget *glWidget;
    QPushButton *rotationChanger;
};

#endif
