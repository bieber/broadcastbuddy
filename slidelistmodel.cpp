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

#include "slidelistmodel.h"

#include <string>

#include <QFile>
#include <QTextStream>

SlideListModel::SlideListModel()
{
}

void SlideListModel::addSlide(QString title, QString fgColor,
                              QString bgColor, QString content){

    emit beginInsertRows(QModelIndex(), slides.size(), slides.size());

    slides.push_back(new slide(title, fgColor, bgColor, content));

    emit endInsertRows();
}

void SlideListModel::removeSlide(int index){
    emit beginRemoveRows(QModelIndex(), index, index);
    delete slides[index];
    slides.erase(slides.begin() + index);
    emit endRemoveRows();
}

void SlideListModel::modSlide(int index, QString title, QString fgColor,
                              QString bgColor, QString content){
    slide* edit = slides[index];
    edit->title = title;
    edit->fgColor = fgColor;
    edit->bgColor = bgColor;
    edit->title = title;
    edit->content = content;

    emit dataChanged(this->index(index, 0, QModelIndex()),
                     this->index(index, 0, QModelIndex()));
}

int SlideListModel::rowCount(const QModelIndex& parent) const{
    QModelIndex x = parent; // A hack to shut g++ up
    return slides.size();
}

QVariant SlideListModel::data(const QModelIndex& index, int role) const{
    if(role != Qt::DisplayRole || index.row() >= (int)slides.size())
        return QVariant();

    return (slides[index.row()])->title;
}

void SlideListModel::saveToFile(QString path){
    QFile file(path);

    //Opening the file
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream fout(&file);

    //Writing the file contents
    std::vector<slide*>::iterator i;
    for(i = slides.begin(); i != slides.end(); i++){
        QString buffer;

        fout << "[{";

        //Buffering and escaping the title
        buffer = "";
        for(int j = 0; j < (*i)->title.length(); j++){
            if((*i)->title[j] == '}'){
                buffer.push_back('~');
                buffer.push_back('}');
            }else if((*i)->content[j] == '~'){
                buffer.push_back('~');
                buffer.push_back('~');
            }else{
                buffer.push_back((*i)->title[j]);
            }
        }

        fout << buffer;
        fout << "}\n {" << (*i)->fgColor << "}\n {" << (*i)->bgColor << "}\n {";

        //Buffering and escaping the text
        buffer = "";
        for(int j = 0; j < (*i)->content.length(); j++){
            if((*i)->content[j] == '}'){
                buffer.push_back('~');
                buffer.push_back('}');
            }else if((*i)->content[j] == '~'){
                buffer.push_back('~');
                buffer.push_back('~');
            }else{
                buffer.push_back((*i)->content[j]);
            }
        }

        fout << buffer << "}]\n\n";
    }

    file.close();
}

void SlideListModel::loadFromFile(QString path){
    QString title;
    QString fgColor;
    QString bgColor;
    QString content;
    char in;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    //Clearing the list
    emit beginRemoveRows(QModelIndex(), 0, (int)(slides.size()) - 1);
    while(slides.size() > 0)
        removeSlide(0);
    emit endRemoveRows();

    //Parsing the individual slides
    while(!file.atEnd()){
        title = ""; fgColor = ""; bgColor = ""; content = "";

        /* It's vitally important that any functional code in this function
         * gets nested within a while(file.getChar(&in)) block, so that if we
         * hit file end prematurely nothing else gets executed
         */

        while(file.getChar(&in) && in != '[');

        while(file.getChar(&in) && in != '{');

        while(file.getChar(&in)){
            if(in == '~'){
                file.getChar(&in);
                title.push_back(in);
            }else if(in == '}'){
                break;
            }else{
                title.push_back(in);
            }
        }

        while(file.getChar(&in) && in != '{');

        while(file.getChar(&in)){
            if(in == '}')
                break;
            else
                fgColor.push_back(in);
        }

        while(file.getChar(&in) && in != '{');

        while(file.getChar(&in)){
            if(in == '}')
                break;
            else
                bgColor.push_back(in);
        }

        while(file.getChar(&in) && in != '{');

        while(file.getChar(&in)){
            if(in == '}'){
                break;
            }else if(in == '~'){
                file.getChar(&in);
                content.push_back(in);
            }else{
                content.push_back(in);
            }
        }

        while(file.getChar(&in) && in != ']');

        if(title != "" && content != "")
            slides.push_back(new slide(title, fgColor, bgColor, content));
    }

    file.close();

    //Notifying viewers that data has been added
    emit beginInsertRows(QModelIndex(), 0, (int)(slides.size()) - 1);
    emit endInsertRows();
}


SlideListModel::~SlideListModel(){
    std::vector<slide*>::iterator i;
    for(i = slides.begin(); i != slides.end(); i++)
        delete (*i);
}
