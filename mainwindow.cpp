#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QtNetwork>
#include <QInputDialog>

#include <iostream>
#include <cstdio>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), connectionLabel(this),
    server(this), clientConnection(this), blockSize(0)
{
    ui->setupUi(this);

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

    //Connecting edit fields to the saveCurrent signal
    QObject::connect(ui->titleInput, SIGNAL(textEdited(QString)),
                     this, SLOT(saveCurrent()));
    QObject::connect(ui->fgInput, SIGNAL(textEdited(QString)),
                     this, SLOT(saveCurrent()));
    QObject::connect(ui->bgInput, SIGNAL(textEdited(QString)),
                     this, SLOT(saveCurrent()));
    QObject::connect(ui->textInput, SIGNAL(textChanged()),
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
    if(fileName == "")
        fileName = QFileDialog::getSaveFileName(this, "Save Script As", "",
                                                "Broadcast Buddy Script Files "
                                                "(*.bbs);;All Files (*.*)");
    if(fileName == "")
        return;

    slides.saveToFile(fileName);
    ui->statusBar->showMessage("Script Saved", 3000);
}

void MainWindow::saveAs(){

    fileName = QFileDialog::getSaveFileName(this, "Save Script As", "",
                                            "Broadcast Buddy Script Files "
                                            "(*.bbs);;All Files (*.*)");

    if(fileName == "")
        return;

    slides.saveToFile(fileName);
    ui->statusBar->showMessage("Script Saved", 3000);
}

void MainWindow::loadFile(){
    fileName = QFileDialog::getOpenFileName(this, "Open Script", "",
                                            "Broadcast Buddy Script Files "
                                            "(*.bbs);;All Files (*.*)");
    if(!QFile::exists(fileName))
        return;

    slides.loadFromFile(fileName);
    ui->statusBar->showMessage("Script Loaded", 3000);
}


void MainWindow::loadSlide(int index){

    if(index < 0){
        //Clear out everything
        ui->titleInput->setEnabled(false);
        ui->fgInput->setEnabled(false);
        ui->bgInput->setEnabled(false);
        ui->textInput->setEnabled(false);
        ui->deleteSlideButton->setEnabled(false);
    }else{
        //Loading the selected slide
        SlideListModel::slide* slide = slides.getSlide(index);
        ui->titleInput->setText(slide->title);
        ui->fgInput->setText(slide->fgColor);
        ui->bgInput->setText(slide->bgColor);
        ui->textInput->setText(slide->content);

        ui->titleInput->setEnabled(true);
        ui->fgInput->setEnabled(true);
        ui->bgInput->setEnabled(true);
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
    nin >> bgColor;
    nin >> content;

    view->setColors(fgColor, bgColor);
    view->setText(content);
    view->showFullScreen();

    blockSize = 0;
}

void MainWindow::networkError(QAbstractSocket::SocketError error){
    if(error == QAbstractSocket::RemoteHostClosedError){
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
