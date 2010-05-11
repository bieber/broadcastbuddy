#include "SlideDisplay.h"
#include "ui_SlideDisplay.h"

SlideDisplay::SlideDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlideDisplay), escape(this)
{
    ui->setupUi(this);
    escape.setKey(Qt::Key_Escape);
    QObject::connect(&escape, SIGNAL(activated()),
                     this, SLOT(close()));
}

SlideDisplay::~SlideDisplay()
{
    delete ui;
}

void SlideDisplay::setColors(QString fg, QString bg){
    QPalette newPal;
    newPal.setColor(QPalette::Background, QString("#") + fg);
    newPal.setColor(QPalette::Foreground, QString("#") + bg);
    setPalette(newPal);
}


void SlideDisplay::setText(QString content){
    ui->output->setText(content);
}
