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
    void saveCurrent(); //Saves current slide to list, not file
    void clearCurrent(); //Replaces edited data with data in list
    void clearAll(); //Empties the current script
    void saveFile(); //Saves the current file
    void saveAs(); //Saves with prompt for filename
    void loadFile(); //Loads a script from file


private:
    Ui::MainWindow *ui;
    QLabel connectionLabel;
    SlideListModel slides;
    int currentSlide;
    QString fileName;

    void loadSlide(int index);
};

#endif // MAINWINDOW_H
