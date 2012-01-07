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

#include "SlideDisplay.h"
#include "ui_SlideDisplay.h"

SlideDisplay::SlideDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SlideDisplay), escape(this), blinker(this)
{
  ui->setupUi(this);
  escape.setKey(Qt::Key_Escape);
  QObject::connect(&escape, SIGNAL(activated()),
                   this, SLOT(close()));
  QObject::connect(&blinker, SIGNAL(timeout()),
                   this, SLOT(blink()));

  blinker.setInterval(500);
}

SlideDisplay::~SlideDisplay()
{
  delete ui;
}

void SlideDisplay::setColors(QString fg, QString bg){

  blinker.stop();

  QPalette newPal;
  newPal.setColor(QPalette::Background, QString("#") + fg);
  newPal.setColor(QPalette::Foreground, QString("#") + bg);
  setPalette(newPal);
}


void SlideDisplay::setText(QString content){
  ui->output->setText(content);
}

void SlideDisplay::emergency(QString text){
  ui->output->setText(text);
  blinker.start();
}

void SlideDisplay::blink(){
  QPalette newPal;
  if(red){
    newPal.setColor(QPalette::Background, QColor(255, 0, 0));
    newPal.setColor(QPalette::Foreground, QColor(0, 0, 0));
    red = false;
  }else{
    newPal.setColor(QPalette::Background, QColor(0, 0, 0));
    newPal.setColor(QPalette::Foreground, QColor(255, 255, 255));
    red = true;
  }
  setPalette(newPal);
}
