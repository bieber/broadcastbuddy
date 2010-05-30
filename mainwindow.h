 /*
  * Copyright 2010, Robert Bieber
  *
  * This file is part of Broadcast Buddy.
  * Broadcast Buddy is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * Broadcast Buddy is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Broadcast Buddy.  If not, see <http://www.gnu.org/licenses/>.
  *
  */

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
    void next();
    void previous();
    void emergencyBroadcast();

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
