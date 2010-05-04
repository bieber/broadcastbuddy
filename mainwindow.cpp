#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>

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

    //Connecting buttons to main window signals
    QObject::connect(ui->deleteSlideButton, SIGNAL(clicked()),
                     this, SLOT(deleteCurrent()));
    QObject::connect(ui->newSlideButton, SIGNAL(clicked()),
                     this, SLOT(newSlide()));
    QObject::connect(ui->confirmEdit, SIGNAL(accepted()),
                     this, SLOT(saveCurrent()));
    QObject::connect(ui->confirmEdit, SIGNAL(rejected()),
                     this, SLOT(clearCurrent()));

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
    loadSlide(currentSlide); //Clears out the interface
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

void MainWindow::loadSlide(int index){

    if(index < 0){
        //Clear out everything
        ui->titleInput->setEnabled(false);
        ui->fgInput->setEnabled(false);
        ui->bgInput->setEnabled(false);
        ui->textInput->setEnabled(false);
        ui->deleteSlideButton->setEnabled(false);
        ui->confirmEdit->setEnabled(false);
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
        ui->confirmEdit->setEnabled(true);

    }
}
