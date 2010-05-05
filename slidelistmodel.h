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
