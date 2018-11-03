#include "mainwindow.h"
#include "Forum/forum.h"
#include "UI/boardsarea.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    loginUI = new LoginDialog(this);
    loginUI->show();

    connect(&Forum::Get(), &Forum::ReadyToDraw, [=]{
        qDebug()<<"in";
        infoBar = new UserInfoBar(this);qDebug()<<"0";

        //main area
        mainArea = new QSplitter(this);  qDebug()<<"1";
        boardArea = new BoardsArea(this);qDebug()<<"2";
        postArea = new PostsArea(this);  qDebug()<<"3";
        mainArea->addWidget(boardArea);  qDebug()<<"4";
        mainArea->addWidget(postArea);   qDebug()<<"5";

        //set layout
        vLayout = new QVBoxLayout(this); qDebug()<<"6";
        vLayout->addWidget(infoBar);
        vLayout->addWidget(mainArea);

        this->show();
    });
}

MainWindow::~MainWindow()
{

}

void MainWindow::RefreshPostArea() {
    delete postArea;
    postArea = new PostsArea(this);
}
