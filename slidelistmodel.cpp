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
    return slides.size();
}

QVariant SlideListModel::data(const QModelIndex& index, int role) const{
    if(role != Qt::DisplayRole || index.row() >= slides.size())
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
        fout << "\n {" << (*i)->fgColor << "}\n {" << (*i)->bgColor << "}\n {";

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
    QChar in;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while(!file.atEnd()){
        title = ""; fgColor = ""; bgColor = ""; content = "";
        //Looking for the opening square bracket
        in = file.getChar();

    }

    file.close();
}


SlideListModel::~SlideListModel(){
    std::vector<slide*>::iterator i;
    for(i = slides.begin(); i != slides.end(); i++)
        delete (*i);
}
