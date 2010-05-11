#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include <QTcpSocket>
#include <QTcpServer>
#include <QList>

#include "SlideDisplay.h"
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

    static const quint16 port = 2682;

private slots:
    void slideSelected(QModelIndex index);
    void deleteCurrent();
    void newSlide();
    void saveCurrent(); //Saves current slide to list, not file
    void clearCurrent(); //Replaces edited data with data in list
    void clearAll(); //Empties the current script
    void saveFile(); //Saves the current file
    void saveAs(); //Saves with prompt for filename
    void loadFile(); //Loads a script from file
    void disconnect(); //Closes all connections
    void connect(); //Attempts to connect to a server
    void newConnection();
    void lostConnection();
    void receiveData();
    void networkError(QAbstractSocket::SocketError error);
    void broadcast();

private:
    Ui::MainWindow *ui;
    QLabel connectionLabel;
    SlideListModel slides;
    int currentSlide;
    QString fileName;
    QTcpServer server;
    QList<QTcpSocket*> connections;
    QTcpSocket clientConnection;
    quint16 blockSize;
    SlideDisplay* view;

    void loadSlide(int index);
    void writeStatus();
    void writeToAll(QByteArray& data);
};

#endif // MAINWINDOW_H
