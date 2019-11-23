#include "viewport.h"
#include <QOpenGLContext>
#include <fstream>
#include <QDir>
#include <GL/glu.h>
#include <QFileDialog>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMatrix4x4>

viewport::viewport(QWidget *parent)
    : QOpenGLWidget(parent), m_texture(0), m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    //defaultScene();
    nSca = 1;
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
    m_projectionMatrix.perspective(45, aspect, 0.1f, 10.0f);


//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();

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
    modalViewMatrix.translate(0.0, 0.0, -5.0);

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

    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);

    m_arrayBuffer.release();
    m_indexBuffer.release();
    m_texture->release();
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

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
    int k = 0;
    for (int i = 0; i < obj.list_vertices.size(); i++)
    {
        GLfloat xV = obj.list_vertices.at(i).x;
        GLfloat yV = obj.list_vertices.at(i).y;
        GLfloat zV = obj.list_vertices.at(i).z;
//        qDebug() << xV;
//        qDebug() << yV;
//        qDebug() << zV;
        coords.append(QVector3D(xV, yV, zV));
       // qDebug() << "Успех вершин";
    } int dfd = 0;
    for (int i = 0; i < obj.list_texture.size(); i++)
    {
        GLfloat uT = obj.list_texture.at(i).u;
        GLfloat vT = obj.list_texture.at(i).v;
        //qDebug() << uT;
        //qDebug() << vT;
        texCoords.append(QVector2D(0, 1));
        //qDebug() << "Успех текстур" << dfd;
        dfd++;
    }
    //qDebug() << "старт заполнения нормалей";
    for (int i = 0; i < obj.list_normal.size(); i++)
    {
        GLfloat xN = obj.list_normal.at(i).x;
        GLfloat yN = obj.list_normal.at(i).y;
        GLfloat zN = obj.list_normal.at(i).z;
        //qDebug() << xN;
        //qDebug() << yN;
        //qDebug() << zN;
        normal.append(QVector3D(xN, yN, zN));
        //qDebug() << "Успех и все";
        k++;
    }
    int count = 0;
    qDebug() << "Заполнение нормалей   не пошло по пизде";
    for (int i = 1; i < obj.face.size(); i++)
    {
        if (i % 4 != 0)
        {
            GLint cord = obj.face.at(i).id_vertices - 1;
            GLint tcord =  obj.face.at(i).id_textur_coordinat - 1;
            GLint norma = obj.face.at(i).id_normal - 1;

            vertexes.append(VertexData(coords[cord], texCoords[tcord], normal[norma]));
            indexes.append(indexes.size());
            qDebug() << cord << tcord << norma;
        }
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


void viewport::mousePressEvent(QMouseEvent* pe)
{
    if (pe->button() == Qt::LeftButton){
        x_pos = pe->x();
        y_pos = pe->y();
    }
}

void viewport::mouseMoveEvent(QMouseEvent *pe)
{
    angle_x = 180 * (GLfloat)(pe->y() - y_pos) / width();
    angle_y = 180 * (GLfloat)(pe->x() - x_pos) / height();
    del = (GLfloat)(pe->y() - translate) / width();
    update();
//    x_pos = pe->x();
//    y_pos = pe->y();
}

void viewport::wheelEvent(QWheelEvent *pe)
{
    if ((pe->delta()) > 0){
        scale_plus();
    }
    else if ((pe->delta() < 0)){
        scale_minus();
    }
    update();
}

void viewport::keyPressEvent(QKeyEvent *pe)
{
    switch (pe->key())
    {
    case Qt::Key_Plus:
        scale_plus();
        break;
    case Qt::Key_Minus:
        scale_minus();
        break;
    case Qt::Key_Up:
        rotate_up();
        break;
    case Qt::Key_Down:
        rotate_down();
        break;
    case Qt::Key_Left:
        rotate_left();
        break;
    case Qt::Key_Right:
        rotate_right();
        break;
    case Qt::Key_Z:
        translate_down();
        break;
    case Qt::Key_X:
        translate_up();
        break;
    case Qt::Key_Space:
        defaultScene();
        break;
    case Qt::Key_Escape:
        this->close();
        break;
    }

    update();
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
