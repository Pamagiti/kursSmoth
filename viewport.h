#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QMatrix4x4>
#include <GL/glu.h>
#include <QOpenGLWidget>
#include <QVector2D>
#include <QPoint>
#include <QString>
#include <QMouseEvent>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "model.h"

struct VertexData
{
    VertexData() //Конструктор по умолчанию не принимающий аргументы
    {
    }
    VertexData(QVector3D p, QVector2D t, QVector3D n):
        position(p), textCoord(t), normal(n)
    {//Конструктор с списком инициализации
    }
    QVector3D position; //позиция
    QVector2D textCoord; //текстурные координаты
    QVector3D normal; //нормали
};

class viewport : public QOpenGLWidget
{
    Q_OBJECT

public:
    viewport(QWidget *parent = nullptr);
    ~viewport();

    void mousePressEvent(QMouseEvent *pe);
    void mouseMoveEvent(QMouseEvent *pe);
    //void mouseMoveEvent(QMouseEvent *pe);
    void wheelEvent(QWheelEvent* pe);
    void keyPressEvent(QKeyEvent* pe);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void initGeom();

    void initShaders();

private:
    GLfloat xRot;
    GLfloat yRot;
    GLfloat zRot;
    GLfloat zTra;
    GLfloat nSca;

    GLfloat x_pos;
    GLfloat y_pos;
    GLfloat angle_x;
    GLfloat angle_y;
    GLfloat translate;
    GLfloat del;
    GLfloat translateX;
    GLfloat translateY;

    QPoint ptrMousePosition;

    void scale_plus();
    void scale_minus();
    void rotate_up();
    void rotate_down();
    void rotate_left();
    void rotate_right();
    void translate_down();
    void translate_up();
    void defaultScene();

    model obj;

    QVector2D m_mousePosition;
    QVector2D m_mousePositionT;
    QQuaternion m_rotationX;
    QQuaternion m_rotationY;

    ///
    QMatrix4x4 m_projectionMatrix;
    QOpenGLShaderProgram m_program;
    QOpenGLTexture *m_texture;
    QOpenGLBuffer m_arrayBuffer;
    QOpenGLBuffer m_indexBuffer;
};

#endif // VIEWPORT_H
