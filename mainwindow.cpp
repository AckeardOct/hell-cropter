#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    form = (Form*)ui->widget;

    connect(ui->actionOpen, SIGNAL(triggered(bool)), SLOT(OpenSlot()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), SLOT(SaveSlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "./", tr("Image Files (*.png *.jpg *.bmp)"));

    if(!fileName.isEmpty())
        form->OpenImage(fileName);
}

void MainWindow::SaveSlot()
{
    QString saveDir = QFileDialog::getSaveFileName(this);

    if(!saveDir.isEmpty())
        form->SaveRects(saveDir);
}
