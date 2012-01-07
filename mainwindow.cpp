/*
  * Copyright 2010-2012, Robert Bieber
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QtNetwork>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QSettings>

#include <iostream>
#include <cstdio>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow), connectionLabel(this),
  server(this), clientConnection(this), blockSize(0)
{
  ui->setupUi(this);

  // Loading window position and size
  QSettings settings;

  settings.beginGroup("BroadcastBuddy");

  QPoint pos = settings.value("position").toPoint();
  QSize size = settings.value("size").toSize();

  settings.endGroup();

  if(!(pos.isNull() || size.isNull()))
  {
    resize(size);
    move(pos);
  }

  //Setting the connection status
  connectionLabel.setText("Disconnected");
  ui->statusBar->addPermanentWidget(&connectionLabel);

  //Connecting the slide view to its model and signals
  QObject::connect(ui->slideList, SIGNAL(pressed(QModelIndex)),
                   this, SLOT(slideSelected(QModelIndex)));
  ui->slideList->setModel(&slides);

  //Connecting buttons to main window slots
  QObject::connect(ui->deleteSlideButton, SIGNAL(pressed()),
                   this, SLOT(deleteCurrent()));
  QObject::connect(ui->newSlideButton, SIGNAL(pressed()),
                   this, SLOT(newSlide()));
  QObject::connect(ui->broadcastButton, SIGNAL(pressed()),
                   this, SLOT(broadcast()));
  QObject::connect(ui->fgPick, SIGNAL(pressed()),
                   this, SLOT(fgPick()));
  QObject::connect(ui->bgPick, SIGNAL(pressed()),
                   this, SLOT(bgPick()));

  //Connecting edit fields to the saveCurrent signal
  QObject::connect(ui->titleInput, SIGNAL(textEdited(QString)),
                   this, SLOT(saveCurrent()));
  QObject::connect(ui->fgInput, SIGNAL(textEdited(QString)),
                   this, SLOT(saveCurrent()));
  QObject::connect(ui->bgInput, SIGNAL(textEdited(QString)),
                   this, SLOT(saveCurrent()));
  QObject::connect(ui->textInput, SIGNAL(textChanged()),
                   this, SLOT(saveCurrent()));

  // Connecting color picker dialogs
  QObject::connect(&fgDialog, SIGNAL(currentColorChanged(QColor)),
                   this, SLOT(fgChange(QColor)));
  QObject::connect(&bgDialog, SIGNAL(currentColorChanged(QColor)),
                   this, SLOT(bgChange(QColor)));
  QObject::connect(&fgDialog, SIGNAL(currentColorChanged(QColor)),
                   this, SLOT(saveCurrent()));
  QObject::connect(&bgDialog, SIGNAL(currentColorChanged(QColor)),
                   this, SLOT(saveCurrent()));

  //Connecting menu items
  QObject::connect(ui->actionNew_Script, SIGNAL(triggered()),
                   this, SLOT(clearAll()));
  QObject::connect(ui->actionLoad_Script, SIGNAL(triggered()),
                   this, SLOT(loadFile()));
  QObject::connect(ui->actionSave_Script, SIGNAL(triggered()),
                   this, SLOT(saveFile()));
  QObject::connect(ui->actionSave_Script_As, SIGNAL(triggered()),
                   this, SLOT(saveAs()));
  QObject::connect(ui->actionDisconnect, SIGNAL(triggered()),
                   this, SLOT(disconnect()));
  QObject::connect(ui->actionConnect_to_Server, SIGNAL(triggered()),
                   this, SLOT(connect()));
  QObject::connect(ui->actionBroadcast_Current, SIGNAL(triggered()),
                   this, SLOT(broadcast()));
  QObject::connect(ui->actionNext, SIGNAL(triggered()),
                   this, SLOT(next()));
  QObject::connect(ui->actionPrevious, SIGNAL(triggered()),
                   this, SLOT(previous()));
  QObject::connect(ui->actionEmergency, SIGNAL(triggered()),
                   this, SLOT(emergencyBroadcast()));

  //Setting up the client
  QObject::connect(&clientConnection,
                   SIGNAL(error(QAbstractSocket::SocketError)),
                   this,
                   SLOT(networkError(QAbstractSocket::SocketError)));
  QObject::connect(&clientConnection, SIGNAL(readyRead()),
                   this, SLOT(receiveData()));

  //Setting up the server
  if(!server.listen(QHostAddress::Any, port))
    QMessageBox::information(this, "Network Error", "Couldn't open server");
  QObject::connect(&server, SIGNAL(newConnection()),
                   this, SLOT(newConnection()));


  //Setting up the viewer
  view = new SlideDisplay();
  QObject::connect(this, SIGNAL(destroyed()),
                   view, SLOT(close()));

  writeStatus();
}

MainWindow::~MainWindow()
{
  // Storing window position and size
  QSettings settings;

  settings.beginGroup("BroadcastBuddy");

  settings.setValue("size", size());
  settings.setValue("position", geometry().topLeft());

  settings.endGroup();

  delete ui;
  delete view;
  disconnect();
}

void MainWindow::slideSelected(QModelIndex index){
  currentSlide = index.row();
  loadSlide(index.row());
}

void MainWindow::deleteCurrent(){
  slides.removeSlide(currentSlide);
  while(currentSlide >= slides.rowCount())
    currentSlide--;
  loadSlide(currentSlide);
}

void MainWindow::newSlide(){

  slides.addSlide("New Slide", "000000", "ffffff", "Slide text here");
  currentSlide = slides.rowCount() - 1;
  QItemSelectionModel *m = ui->slideList->selectionModel();
  m->select(slides.index(slides.rowCount() - 1),
            QItemSelectionModel::ClearAndSelect);
  loadSlide(currentSlide);
}

void MainWindow::saveCurrent(){

  slides.modSlide(currentSlide, ui->titleInput->text(),
                  ui->fgInput->text(), ui->bgInput->text(),
                  ui->textInput->toPlainText());
}

void MainWindow::clearCurrent(){
  loadSlide(currentSlide);
}

void MainWindow::clearAll(){
  fileName = "";
  while(slides.rowCount() > 0)
    slides.removeSlide(0);
  loadSlide(-1);
}

void MainWindow::saveFile(){
  QSettings settings;
  settings.beginGroup("BroadcastBuddy");

  QString lastDir = settings.value("lastdir", "").toString();

  if(fileName == "")
    fileName = QFileDialog::getSaveFileName(this, "Save Script As", lastDir,
                                            "Broadcast Buddy Script Files "
                                            "(*.bbs);;All Files (*.*)");
  if(fileName == "")
    return;

  settings.setValue("lastdir", QFileInfo(fileName).dir().path());
  settings.endGroup();

  slides.saveToFile(fileName);
  ui->statusBar->showMessage("Script Saved", 3000);
}

void MainWindow::saveAs(){

  QSettings settings;
  settings.beginGroup("BroadcastBuddy");

  QString lastDir = settings.value("lastdir", "").toString();

  fileName = QFileDialog::getSaveFileName(this, "Save Script As", lastDir,
                                          "Broadcast Buddy Script Files "
                                          "(*.bbs);;All Files (*.*)");

  if(fileName == "")
    return;

  settings.setValue("lastdir", QFileInfo(fileName).dir().path());
  settings.endGroup();

  slides.saveToFile(fileName);
  ui->statusBar->showMessage("Script Saved", 3000);
}

void MainWindow::loadFile(){
  QSettings settings;
  settings.beginGroup("BroadcastBuddy");

  QString lastDir = settings.value("lastdir", "").toString();

  fileName = QFileDialog::getOpenFileName(this, "Open Script", lastDir,
                                          "Broadcast Buddy Script Files "
                                          "(*.bbs);;All Files (*.*)");
  if(!QFile::exists(fileName))
    return;

  settings.setValue("lastdir", QFileInfo(fileName).dir().path());
  settings.endGroup();

  slides.loadFromFile(fileName);
  ui->statusBar->showMessage("Script Loaded", 3000);
}


void MainWindow::loadSlide(int index){

  if(index < 0){
    //Clear out everything
    ui->titleInput->setEnabled(false);
    ui->fgInput->setEnabled(false);
    ui->bgInput->setEnabled(false);
    ui->fgPick->setEnabled(false);
    ui->bgPick->setEnabled(false);
    ui->textInput->setEnabled(false);
    ui->deleteSlideButton->setEnabled(false);
  }else{
    //Loading the selected slide
    SlideListModel::slide* slide = slides.getSlide(index);
    ui->titleInput->setText(slide->title);
    ui->fgInput->setText(slide->fgColor);
    ui->bgInput->setText(slide->bgColor);
    fgDialog.setCurrentColor(QColor("#" + slide->fgColor));
    bgDialog.setCurrentColor(QColor("#" + slide->bgColor));
    ui->textInput->setText(slide->content);

    ui->titleInput->setEnabled(true);
    ui->fgInput->setEnabled(true);
    ui->bgInput->setEnabled(true);
    ui->fgPick->setEnabled(true);
    ui->bgPick->setEnabled(true);
    ui->textInput->setEnabled(true);
    ui->deleteSlideButton->setEnabled(true);

  }
}

void MainWindow::newConnection(){
  while(server.hasPendingConnections()){
    QTcpSocket* conn = server.nextPendingConnection();
    QObject::connect(conn, SIGNAL(disconnected()),
                     this, SLOT(lostConnection()));
    QObject::connect(conn, SIGNAL(disconnected()),
                     conn, SLOT(deleteLater()));
    connections.push_back(conn);
  }

  writeStatus();
}

void MainWindow::lostConnection(){

  for(int i = 0; i < connections.size(); i++){
    if(connections[i] == QObject::sender()){
      connections.removeAt(i);
      break;
    }
  }

  writeStatus();
}

void MainWindow::disconnect(){
  if(connections.size() > 0){
    while(connections.size() > 0){
      connections[0]->close();
      connections.removeAt(0);
    }
  }
  server.close();
  server.listen(QHostAddress::Any, port);
  clientConnection.close();

  writeStatus();
}

void MainWindow::connect(){
  bool ok;
  QString address = QInputDialog::getText(this, "Connect to Server",
                                          "Hostname: ", QLineEdit::Normal,
                                          "localhost", &ok);
  if(ok){
    //Just in case
    server.close();

    if(clientConnection.isOpen())
      clientConnection.close();

    clientConnection.connectToHost(address, port, QTcpSocket::ReadOnly);

    writeStatus();
  }
}

void MainWindow::receiveData(){

  QDataStream nin(&clientConnection);
  nin.setVersion(QDataStream::Qt_4_0);

  if(blockSize == 0){
    if(clientConnection.bytesAvailable() < (int)sizeof(quint16))
      return;

    nin >> blockSize;
  }

  if(clientConnection.bytesAvailable() < blockSize)
    return;

  //We'll only get this far if we've got a complete block ready
  QString fgColor;
  QString bgColor;
  QString content;
  nin >> fgColor;

  //Checking for the emergency signal
  if(fgColor == "~E"){

    nin >> content;
    view->setColors("#000000", "#ff0000");
    view->setText(content);
    view->emergency(content);
    view->showFullScreen();
    blockSize = 0;
    return;

  }

  nin >> bgColor;
  nin >> content;

  view->setColors(fgColor, bgColor);
  view->setText(content);
  view->showFullScreen();

  blockSize = 0;
}

void MainWindow::networkError(QAbstractSocket::SocketError error){
  if(error == QAbstractSocket::RemoteHostClosedError){
    QTimer::singleShot(0, this, SLOT(disconnect()));
    writeStatus();
    QTimer::singleShot(0, view, SLOT(close()));
    return;
  }

  QMessageBox::information(this, "Network Error",
                           "Couldn't connect to server");
  clientConnection.close();
  writeStatus();
}

void MainWindow::writeStatus(){

  if(clientConnection.isOpen()){
    connectionLabel.setText("Connected to master");
  }else if(connections.size() > 0){
    QString connString = "Connected to ";
    char buffer[50];
    std::sprintf(buffer, "%d", connections.size());
    connString.append(buffer);
    connString.append(" clients");
    connectionLabel.setText(connString);

  }else if(server.isListening()){

    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QList<QHostAddress>::iterator i;

    QList<QString> ips;
    for(i = addresses.begin(); i != addresses.end(); i++)
      if(i->toIPv4Address() != 0 && *i != QHostAddress::LocalHost)
        ips.push_back(i->toString());

    QString listening = "Listening on ";
    QList<QString>::iterator j;
    for(j = ips.begin(); j != ips.end(); j++){
      listening += *j;
      if(!ips.endsWith(*j))
        listening += ", ";
    }

    connectionLabel.setText(listening);

  }else{

    connectionLabel.setText("Network error");

  }

}

void MainWindow::writeToAll(QByteArray& data){

  QList<QTcpSocket*>::iterator i;
  for(i = connections.begin(); i != connections.end(); i++)
    (*i)->write(data);
}

void MainWindow::broadcast(){

  if(currentSlide < 0 || currentSlide >= slides.rowCount())
    return;

  //Starting an empty array of bytes
  QByteArray data;
  QDataStream nout(&data, QIODevice::WriteOnly);
  nout.setVersion(QDataStream::Qt_4_0);
  nout << (quint16)0;

  //Adding some data
  SlideListModel::slide* ref = slides.getSlide(currentSlide);

  //Making sure the colors are valid hex strings, or else everything will go
  //wrong on the other end
  if(ref->fgColor.length() != 6){
    QMessageBox::information(this, "Format Error", "Foreground color must "
                             "be a six digit hexadecimal color");
    return;
  }

  if(ref->bgColor.length() != 6){
    QMessageBox::information(this, "Format Error", "Background color must "
                             "be a six digit hexadecimal color");
    return;
  }

  nout << ref->fgColor;
  nout << ref->bgColor;
  nout << ref->content;

  //And then going back and adding the size
  nout.device()->seek(0);
  nout << (quint16)(data.size() - (int)sizeof(quint16));
  writeToAll(data);
}

void MainWindow::next(){
  if(currentSlide < slides.rowCount() - 1)
    currentSlide++;
  QTimer::singleShot(0, this, SLOT(broadcast()));
}

void MainWindow::previous(){
  if(currentSlide > 0)
    currentSlide--;
  QTimer::singleShot(0, this, SLOT(broadcast()));
}

void MainWindow::emergencyBroadcast(){

  bool ok;
  QString text = QInputDialog::getText(this, "Emergency Broadcast",
                                       "Emergency broadcast text: ",
                                       QLineEdit::Normal, "", &ok);
  if(!ok)
    return;

  // Assembling the emergency message
  QByteArray data;
  QDataStream nout(&data, QIODevice::WriteOnly);
  nout.setVersion(QDataStream::Qt_4_0);
  nout << (quint16)0; //Place holder for the block size

  nout << QString("~E");
  nout << text;
  nout.device()->seek(0);
  nout << (quint16)(data.size() - sizeof(quint16));

  writeToAll(data);
}

void MainWindow::fgPick()
{
  fgDialog.show();
}

void MainWindow::bgPick()
{
  bgDialog.show();
}

void MainWindow::fgChange(QColor color)
{
  ui->fgInput->setText(color.name().right(6));
}

void MainWindow::bgChange(QColor color)
{
  ui->bgInput->setText(color.name().right(6));
}
