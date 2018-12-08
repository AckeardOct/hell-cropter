#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QSet>

namespace Ui {
class Form;
}

class ImageView;

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form ();

    void OpenImage(QString fileName);
    void SaveRects(QString dirPath) const;

private slots:
    void UpdateOptionsSlot();

private:
    Ui::Form *ui;
    ImageView* imageView { NULL };

};


struct CropOptions
{
    float zoom  { 1. };
    int rows    { 1 };
    int cols    { 1 };
    int offsetX { 0 };
    int offsetY { 0 };
};


class ImageView : public QWidget
{
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent = 0);

    void OpenImage(QString fileName);
    void SaveRects(QString dirPath);
    void UpdateOptions(CropOptions& options);
    void Update();
    void ReCalc();

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent* event);

public slots:
    void SelectAllSlot();

private:
    QString imagePath;
    QPixmap* pixmap { NULL };
    int pixWidth    { 0 };
    int pixHeight   { 0 };
    CropOptions cropOptions;
    QVector<QRect> grid;
    QSet<int>   selectedRects;
};

#endif // FORM_H
