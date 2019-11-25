#ifndef MODEL_H
#define MODEL_H
#include <QFile>
#include <QString>
#include <QStringList>
#include <QtOpenGL/QtOpenGL>
#include <QVector>


//struct VertexData //Структура с информацией о вершинах
//{
//    VertexData() //Конструктор по умолчанию не принимающий аргументы
//    {
//    }
//    VertexData(QVector3D p, QVector2D t, QVector3D n):
//        position(p), textCoord(t), normal(n)
//    {//Конструктор с списком инициализации
//    }
//    QVector3D position; //позиция
//    QVector2D textCoord; //текстурные координаты
//    QVector3D normal; //нормали
//};

struct lVertices
{
    float x;
    float y;
    float z;
};

struct TexturCord
{
    float u = 0;
    float v = 0;
};

struct Normal
{
    float x = 0;
    float y = 0;
    float z = 0;
};

struct SpaceV
{
    float u;
    float v = 0;
    float w = 0;
};


struct Face
{
      int id_vertices;
      int id_textur_coordinat = 0;
      int id_normal = 0;
};

class model
{
public:
    model();
    //~model();
    void loadObj(QString filename);
    void DrawObject();

    QVector<Face> getFace() const;
    void setFace(const QVector<Face> &value);

    lVertices vert;
    TexturCord tc;
    Normal norm;
    Face ind;

    GLint count = 0;

    bool m_texture_coord = false; // is vt?
    bool m_normal = false; // is vn?

    QVector<lVertices> list_vertices; //v
    QVector<TexturCord> list_texture; //vt
    QVector<Normal> list_normal; //vn
    QVector<SpaceV> space_vert; //vp
    QVector<Face> face; //f
    QVector<QString> mtl;                   // mtlib
    QVector<QString> usemtl;
    int countFace = 0;

private:
    void record(const QString &line);
    void readVerticex(const QString &line);
    void readTextureCord(const QString &line);
    void readNormal(const QString &line);
    void readSpaceVert(const QString &line);
    void readFace(const QString &line);
    void readMtl(const QString &line);
    void readUseMtl(const QString &line);
    QString get_corret_line(const QString &line);
    bool isblank(const QChar &ch);
    int get_normal_index(int index, QString name);

};
#endif // MODEL_H
