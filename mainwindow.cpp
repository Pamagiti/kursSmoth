#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector2D>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    viewport3d = new viewport();
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete viewport3d;
}

void MainWindow::on_actionLoad_triggered()
{
    QVBoxLayout *layout = new QVBoxLayout();
//    QGLFormat format(QGL::SampleBuffers);
//    format.setSamples(8);
//    viewport3d->setFormat(format);
    layout->setMargin(0);
    layout->addWidget(viewport3d);
    ui->openGLWidget->setLayout(layout);
}

