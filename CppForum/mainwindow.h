#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
#include <QScrollArea>
#include "UI/logindialog.h"
#include "UI/userinfobar.h"
#include "UI/postsarea.h"
#include "UI/boardsarea.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //This is for boardArea to refresh postArea (by signal).
    //Returning reference is necessary to make the change
    //visible to MainWindow
    PostsArea* & AccessPostArea() { return postArea; }

public slots:
    void RefreshPostArea();

private:
    LoginDialog *loginUI;

    //Since the QMainWindow class already has a layout,
    //we need to use a container(QWidget) to hold our own
    //layout (inside the QMainWindow's default layout)
    QWidget *container;
    QVBoxLayout *vLayout;
    UserInfoBar *infoBar;

    QSplitter *mainArea;
    BoardsArea *boardArea;
    QScrollArea *scrollArea;//contain postArea
    PostsArea *postArea;
};

#endif // MAINWINDOW_H
