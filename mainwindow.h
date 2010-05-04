#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include <QStringListModel>

#include "slidelistmodel.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slideSelected(QModelIndex index);
    void deleteCurrent();
    void newSlide();
    void saveCurrent();
    void clearCurrent();


private:
    Ui::MainWindow *ui;
    QLabel connectionLabel;
    SlideListModel slides;
    int currentSlide;
    QString fileName;

    void loadSlide(int index);
};

#endif // MAINWINDOW_H
