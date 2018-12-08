#include "form.h"
#include "ui_form.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEngine>
#include <QMouseEvent>
#include <QDir>
#include <QMessageBox>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    imageView = ui->imageView;

    connect(ui->sb_zoom, SIGNAL(valueChanged(double)), SLOT(UpdateOptionsSlot()));
    connect(ui->sb_rows, SIGNAL(valueChanged(int)), SLOT(UpdateOptionsSlot()));
    connect(ui->sb_cols, SIGNAL(valueChanged(int)), SLOT(UpdateOptionsSlot()));
    connect(ui->sb_offsetX, SIGNAL(valueChanged(int)), SLOT(UpdateOptionsSlot()));
    connect(ui->sb_offsetY, SIGNAL(valueChanged(int)), SLOT(UpdateOptionsSlot()));

    connect(ui->pb_selectAll, SIGNAL(clicked(bool)), imageView, SLOT(SelectAllSlot()));
}


Form::~Form()
{
    delete ui;
}


void Form::OpenImage(QString fileName)
{
    imageView->OpenImage(fileName);
}

void Form::SaveRects(QString dirPath) const
{
    imageView->SaveRects(dirPath);
}


void Form::UpdateOptionsSlot()
{
    CropOptions o;

    o.zoom    = ui->sb_zoom->value();
    o.rows    = ui->sb_rows->value();
    o.cols    = ui->sb_cols->value();
    o.offsetX = ui->sb_offsetX->value();
    o.offsetY = ui->sb_offsetY->value();

    { // normalize
        if(o.rows == 0) o.rows = 1;
        if(o.cols == 0) o.cols = 1;
    }

    imageView->UpdateOptions(o);
}


// ==============================================

ImageView::ImageView(QWidget *parent) :
    QWidget(parent)
{

}


void ImageView::OpenImage(QString fileName)
{
    imagePath = fileName;

    delete pixmap;
    pixmap = new QPixmap(imagePath);
    pixWidth  = pixmap->width();
    pixHeight = pixmap->height();

    Update();
}

void ImageView::SaveRects(QString dirPath)
{
    if(pixmap == NULL)
        return;

    QDir dir(dirPath);
    QString fileName = dir.dirName();
    QString ext = QFileInfo(fileName).suffix();
    fileName = fileName.left(fileName.size() - ext.size() - 1);
    dirPath = dirPath.left(dirPath.size() - ext.size() - 1);
    if(ext.isEmpty())
        ext = "png";

    qDebug() << fileName << ext << dirPath;

    if(!dir.exists() && !dir.mkdir(dirPath)) {
        qDebug() << "Can't make dir";
        QMessageBox::warning(this, "ERROR", "Can't mkdir: " + dirPath);
        return;
    }

    int counter = 0;
    for(int i = 0; i < grid.size(); i++) {
        if (!selectedRects.contains(i))
            continue;

        QPixmap pix = pixmap->copy(grid.at(i));
        QString name = fileName + "-" + QString::number(counter) + "." + ext;
        pix.save(dirPath + "/" + name);
        counter++;
    }
}

void ImageView::UpdateOptions(CropOptions &options)
{
    qDebug() << __FUNCTION__;
    cropOptions = options;
    Update();
}

void ImageView::Update()
{
    ReCalc();
    repaint();
}

void ImageView::ReCalc()
{
    const CropOptions& o = cropOptions;

    // calc grid
    grid.clear();
    const int widthDelta = pixWidth / o.cols;
    const int heightDelta = pixHeight / o.rows;
    for(int i = 0; i < o.cols; i++) {
        for(int j = 0; j < o.rows; j++) {
            grid.append(QRect(i * widthDelta + o.offsetX, j * heightDelta + o.offsetY, widthDelta, heightDelta));
        }
    }

}


void ImageView::paintEvent(QPaintEvent *e)
{
    if(pixmap == NULL)
        return;

    qDebug() << __FUNCTION__;
    const CropOptions& o = cropOptions;
    QPainter painter(this);

    // draw image
    painter.drawPixmap(0, 0, pixWidth * o.zoom, pixHeight * o.zoom, *pixmap);

    { // draw grid
        painter.setPen(QPen(QColor("red")));
        for(int i = 0; i < grid.size(); i++) {
            int opacity = 0;
            bool selected = selectedRects.contains(i);
            if(selected)
                opacity = 100;

            painter.setBrush(QBrush(QColor(255, 0, 0, opacity)));
            QRect rect = grid.at(i);
            painter.drawRect(rect.x() * o.zoom ,rect.y() * o.zoom
                             , rect.width() * o.zoom, rect.height() * o.zoom );
        }
    }
}


void ImageView::mousePressEvent(QMouseEvent *event)
{
    const CropOptions& o = cropOptions;

    const int mouseX = o.offsetX + event->x() / o.zoom;
    const int mouseY = o.offsetY + event->y() / o.zoom;

    qDebug() << event->x() << "  " << event->y() << mouseX << mouseY;

    // find rect
    for(int i = 0; i < grid.size(); i++) {
        if(grid.at(i).contains(mouseX, mouseY, true)) {
            if(selectedRects.contains(i)) {
                qDebug() << "remove " << grid.at(i).x() << grid.at(i).y();
                selectedRects.remove(i);
            } else {
                qDebug() << "add " << grid.at(i).x() << grid.at(i).y();
                selectedRects.insert(i);
            }
        }
    }

    Update();
}


void ImageView::SelectAllSlot()
{
    if (selectedRects.empty()) {
        for(int i = 0; i < grid.size(); i++) {
            selectedRects.insert(i);
        }
    } else {
        selectedRects.clear();
    }

    Update();
}



