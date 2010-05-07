#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), connectionLabel()
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
}

MainWindow::~MainWindow()
{
    delete ui;
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
