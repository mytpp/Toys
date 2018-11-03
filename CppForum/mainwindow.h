#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
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

    QVBoxLayout *vLayout;
    UserInfoBar *infoBar;
    QSplitter *mainArea;
    BoardsArea *boardArea;
    PostsArea *postArea;
};

#endif // MAINWINDOW_H
