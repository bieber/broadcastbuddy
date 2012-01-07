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

#ifndef SLIDELISTMODEL_H
#define SLIDELISTMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <vector>

class SlideListModel : public QAbstractListModel
{
public:
  //Simple struct to define a slide
  struct slide{
    QString title;
    QString fgColor;
    QString bgColor;
    QString content;

    slide(QString title, QString fg, QString bg, QString content):
        title(title), fgColor(fg), bgColor(bg), content(content){ }
  };


  SlideListModel();
  virtual ~SlideListModel();
  void addSlide(QString title, QString fgColor,
                QString bgColor, QString content);
  void removeSlide(int index);
  void modSlide(int index, QString title, QString fgColor,
                QString bgColor, QString content);
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  slide* getSlide(int index){ return slides[index]; }
  void clearSlides();
  void saveToFile(QString path);
  void loadFromFile(QString path);

private:
  std::vector<slide*> slides;
};

#endif // SLIDELISTMODEL_H
