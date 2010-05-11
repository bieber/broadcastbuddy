#ifndef SLIDEDISPLAY_H
#define SLIDEDISPLAY_H

#include <QtGui/QWidget>
#include <QShortcut>

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

private:
    Ui::SlideDisplay *ui;
    QShortcut escape;

};

#endif // SLIDEDISPLAY_H
