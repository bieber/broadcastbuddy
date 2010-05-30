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

#ifndef SLIDEDISPLAY_H
#define SLIDEDISPLAY_H

#include <QtGui/QWidget>
#include <QShortcut>
#include <QTimer>

namespace Ui {
    class SlideDisplay;
}

class SlideDisplay : public QWidget {
    Q_OBJECT
public:
    SlideDisplay(QWidget *parent = 0);
    ~SlideDisplay();

    void setColors(QString fg, QString bg);
    void setText(QString content);
    void emergency(QString text);

private slots:
    void blink();

private:
    Ui::SlideDisplay *ui;
    QShortcut escape;
    QTimer blinker;
    bool red;

};

#endif // SLIDEDISPLAY_H
