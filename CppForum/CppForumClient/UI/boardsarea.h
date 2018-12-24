#ifndef BOARDSAREA_H
#define BOARDSAREA_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QList>
#include <QPushButton>


class BoardsArea : public QWidget
{
    Q_OBJECT
public:
    explicit BoardsArea(QWidget *parent = nullptr);

private:
    QVBoxLayout *boardsLayout;
    QList<QPushButton*> buttonList;
};

#endif // BOARDSAREA_H
