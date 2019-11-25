#include "viewport.h"
#include <QOpenGLContext>
#include <fstream>
#include <QDir>
#include <GL/glu.h>
#include <QFileDialog>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QVector2D>

viewport::viewport(QWidget *parent)
    : QOpenGLWidget(parent), m_texture(0), m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    //defaultScene();
    nSca = 1;
    translateX = 0;
    translateY = 0;
    del = 0;
}

viewport::~viewport()
{
}
///Класс вьюпорта с попыткой рисовать все в шейдерах(их я вренул к стандартным и ничего не дописывал)
/// и костылем в виде текстуры geom чтобы проще все настроить
void viewport::initializeGL()
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    initShaders();

    QStringList pathToFiles = QFileDialog::getOpenFileNames(nullptr, "Open Dialog", "", "*.obj");
    QString path;
    foreach(QString str, pathToFiles ) path.append(str);
    QDir(path).absolutePath();
    obj.loadObj(path);

    initGeom();
}


void viewport::resizeGL(int w, int h)
{
    float aspect = w / (h ? (float)h : 1);

    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(45, aspect, 0.001f, 1000.0f);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

//    glViewport(0, 0, w, h);
//    glOrtho(-50, 50, -50, 50, -50, 50);
//    glViewport(0, 0, w, h);
//    glMatrixMode(GL_PROJECTION);
}

void viewport::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 modalViewMatrix;
    modalViewMatrix.setToIdentity();
    modalViewMatrix.translate(0, 0, -5.0f * nSca);
    //modalViewMatrix.translate(0, translateX, -5.0f * nSca);

    modalViewMatrix.rotate(m_rotationX);
    modalViewMatrix.rotate(m_rotationY);

    m_texture->bind(0);

    m_program.bind();
    m_program.setUniformValue("qt_ModelViewProjectionMatrix", m_projectionMatrix * modalViewMatrix);
    m_program.setUniformValue("qt_Texture0", 0);

    m_arrayBuffer.bind();

    int offset = 0;

    int vertLoc = m_program.attributeLocation("qt_Vertex");
    m_program.enableAttributeArray(vertLoc);
    m_program.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof (VertexData));

    offset += sizeof (QVector3D);

    int textLoc = m_program.attributeLocation("qt_MultiTexCoord0");
    m_program.enableAttributeArray(textLoc);
    m_program.setAttributeBuffer(textLoc, GL_FLOAT, offset, 2, sizeof (VertexData));

    m_indexBuffer.bind();

    //glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, (void*)0);

    if (obj.countFace == 3){
    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, (void*)0);
    } else if (obj.countFace == 4){
        glDrawElements(GL_QUADS, m_indexBuffer.size(), GL_UNSIGNED_INT, (void*)0);
    }
//    glPointSize(10);
//    glDrawElements(GL_POINTS, m_indexBuffer.size(), GL_UNSIGNED_INT, (void*)0);
//    GL_POINTS	 Рисование точек
//     GL_LINES	 Рисование линий по 2 точкам
//     GL_LINE_LOOP	 Рисование нескольких связанных линий 1 и последняя точка связываются в линию
//     GL_LINE_STRIP	 Рисование нескольких связанных линий
//     GL_TRIANGLES	 Рисование треугольников по 3 точкам
//     GL_TRIANGLE_STRIP	 Рисование нескольких связанных треугольников
//     GL_TRIANGLE_FAN	Первый соединяется с последним
//     GL_QUADS	 Рисование квадратов по 4 точкам
//     GL_QUAD_STRIP	 Рисование связанных квадратов
//     GL_POLYGON	 Рисование полигонов от 3 точек.

    m_arrayBuffer.release();
    m_indexBuffer.release();
    m_texture->release();
//    glRotatef(angle_x, 1,0,0);
//    glRotatef(angle_y, 0,1,0);
//    glScaled(nSca, nSca, nSca);
    //obj.DrawObject();
}

void viewport::initGeom()
{
    QVector<QVector3D> coords;
    QVector<QVector2D> texCoords;
    QVector<QVector3D> normal;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;

    for (int i = 0; i < obj.list_vertices.size(); i++)
    {
        GLfloat xV = obj.list_vertices.at(i).x;
        GLfloat yV = obj.list_vertices.at(i).y;
        GLfloat zV = obj.list_vertices.at(i).z;
//        qDebug() << "вершины" << xV << yV << zV;
        coords.append(QVector3D(xV, yV, zV));
       // qDebug() << "Успех вершин";
    }
    if (!obj.m_texture_coord)
    {
        for (int i = 0; i < obj.count; i++)
        {
            GLfloat uT = 0;
            GLfloat vT = 0;
//            qDebug() << "текстуры" << uT << vT;
            texCoords.append(QVector2D(0, 1));
        }
    } else if (obj.m_texture_coord){
        for (int i = 0; i < obj.list_texture.size(); i++)
        {
            GLfloat uT = obj.list_texture.at(i).u;
            GLfloat vT = obj.list_texture.at(i).v;
//            qDebug() << "текстуры" << uT << vT;
            texCoords.append(QVector2D(0, 1));
        }
    }
    //qDebug() << "старт заполнения нормалей";
    if (!obj.m_normal)
    {
        for (int i = 0; i < obj.count; i++)
        {
            GLfloat xN = 0;
            GLfloat yN = 0;
            GLfloat zN = 0;
//            qDebug() << "нормали" << xN << yN << zN;
            normal.append(QVector3D(xN, yN, zN));
        }
    } else if (obj.m_normal)
    {
        for (int i = 0; i < obj.list_normal.size(); i++)
        {
            GLfloat xN = obj.list_normal.at(i).x;
            GLfloat yN = obj.list_normal.at(i).y;
            GLfloat zN = obj.list_normal.at(i).z;
            //qDebug() << "нормали" << xN << yN << zN;
            normal.append(QVector3D(xN, yN, zN));
        }
    }
    GLint cord;
    qDebug() << "Заполнение нормалей   не пошло по пизде";
    for (int i = 0; i < obj.face.size(); i++)
    {

        GLint cord = obj.face.at(i).id_vertices;
        GLint tcord = obj.face.at(i).id_textur_coordinat;
        GLint norma = obj.face.at(i).id_normal;
        //qDebug() << cord << tcord << norma;
        vertexes.append(VertexData(coords[cord], texCoords[tcord], normal[norma]));
        indexes.append(indexes.size());
    }

    m_arrayBuffer.create();
    m_arrayBuffer.bind();
    m_arrayBuffer.allocate(vertexes.constData(), vertexes.size() * sizeof (VertexData));
    m_arrayBuffer.release();
    //qDebug() << "бинд буферов - в";
    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.allocate(indexes.constData(), indexes.size() * sizeof (GLint));
    m_indexBuffer.release();
    //qDebug() << "бинд буферов - и";

    m_texture = new QOpenGLTexture(QImage(":/cubbbbbbb.jpg").mirrored());

    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

    m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void viewport::initShaders()
{
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshaders.vsh"))
    {   qDebug() << "Херня в шейдерах - вершинный";
        close();}

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshaders.fsh"))
    {
        qDebug() << "Херня в шейдерах - фрагментный";
        close();}

    if (!m_program.link())
    {
        qDebug() << "Херня в шейдерах - линкуется";
        close();}
}


void viewport::mousePressEvent(QMouseEvent *pe)
{
    if (pe->button() == Qt::LeftButton){
        m_mousePosition = QVector2D(pe->localPos());
        pe->accept();
    } else if (pe->button() == Qt::RightButton){
        m_mousePositionT = QVector2D(pe->localPos());
        pe->accept();
    }
}

void viewport::mouseMoveEvent(QMouseEvent *pe)
{
    QVector2D diff = QVector2D(pe->localPos()) - m_mousePosition;
    m_mousePosition = QVector2D(pe->localPos());

//    float angle = diff.length() / 2;

//    QVector3D axis = QVector3D(diff.y(), diff.x(), 0.0);

//    m_rotation = QQuaternion::fromAxisAndAngle(axis, angle) * m_rotation;

    float angleX = diff.y() / 2.0f;
    float angleY = diff.x() / 2.0f;

    m_rotationX = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, angleX) * m_rotationX;
    m_rotationY = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, angleY) * m_rotationY;

//    QVector2D diffT = QVector2D(pe->localPos()) - m_mousePositionT;
//    m_mousePositionT = QVector2D(pe->localPos());

//    translateX += diffT.y() / 50.0f;
//    translateY += diffT.x() / 50.0f;

    update();
//    angle_x = 180 * (GLfloat)(pe->y() - y_pos) / width();
//    angle_y = 180 * (GLfloat)(pe->x() - x_pos) / height();
//    del = (GLfloat)(pe->y() - translate) / width();
//    update();
////    x_pos = pe->x();
////    y_pos = pe->y();
}

void viewport::wheelEvent(QWheelEvent *pe)
{
    if ((pe->delta()) > 0){
        scale_minus();
    }
    else if ((pe->delta() < 0)){
        scale_plus();
    }
    update();
}

void viewport::keyPressEvent(QKeyEvent *pe)
{
//    switch (pe->key())
//    {
//    case Qt::Key_6:
//        translateX += 1;
//        break;
//    case Qt::Key_4:
//        translateX -= 1;
//        break;
//    case Qt::Key_8:
//        translateY += 1;
//        break;
//    case Qt::Key_2:
//        translateY -= 1;
//        break;
//    case Qt::Key_Plus:
//        scale_plus();
//        break;
//    case Qt::Key_Minus:
//        scale_minus();
//        break;
//    case Qt::Key_Up:
//        rotate_up();
//        break;
//    case Qt::Key_Down:
//        rotate_down();
//        break;
//    case Qt::Key_Left:
//        rotate_left();
//        break;
//    case Qt::Key_Right:
//        rotate_right();
//        break;
//    case Qt::Key_Z:
//        translate_down();
//        break;
//    case Qt::Key_X:
//        translate_up();
//        break;
//    case Qt::Key_Space:
//        defaultScene();
//        break;
//    case Qt::Key_Escape:
//        this->close();
//        break;
//    }

//    update();
}


void viewport::scale_plus()
{
    nSca = nSca * 1.1f;
}

void viewport::scale_minus()
{
    nSca = nSca / 1.1f;
}

void viewport::rotate_up()
{
    xRot += 1.0f;
}

void viewport::rotate_down()
{
    xRot -= 1.0f;
}

void viewport::rotate_left()
{
    zRot += 1.0f;
}

void viewport::rotate_right()
{
    zRot -= 1.0f;
}

void viewport::translate_down()
{
    zTra -= 0.05f;
}

void viewport::translate_up()
{
    zTra += 0.05f;
}

void viewport::defaultScene()
{
    xRot=-90; yRot = 0; zRot = 0; zTra = 0; nSca = 1;
}
