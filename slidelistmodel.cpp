#include "slidelistmodel.h"

#include <iostream>
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
    return slides.size();
}

QVariant SlideListModel::data(const QModelIndex& index, int role) const{
    if(role != Qt::DisplayRole || index.row() >= slides.size())
        return QVariant();

    return (slides[index.row()])->title;
}

SlideListModel::~SlideListModel(){
    std::vector<slide*>::iterator i;
    for(i = slides.begin(); i != slides.end(); i++)
        delete (*i);
}
