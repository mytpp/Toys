#include "mainwindow.h"
#include "Forum/forum.h"
#include "UI/boardsarea.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    loginUI = new LoginDialog(this);
    loginUI->show();

    container = new QWidget(this);
    this->setCentralWidget(container);

    connect(&Forum::Get(), &Forum::ReadyToDraw, [=]{
        infoBar = new UserInfoBar(this);qDebug()<<"0";

        //main area
        mainArea = new QSplitter(this);  qDebug()<<"1";
        boardArea = new BoardsArea(this);qDebug()<<"2";
        postArea = new PostsArea(this);  qDebug()<<"3";
        mainArea->addWidget(boardArea);  qDebug()<<"4";
        mainArea->addWidget(postArea);   qDebug()<<"5";
        mainArea->setStretchFactor(0, 1);
        mainArea->setStretchFactor(1, 3);

        //set layout of the main window
        vLayout = new QVBoxLayout(container); qDebug()<<"6";
        vLayout->addWidget(infoBar);
        vLayout->addWidget(mainArea);

        this->resize(800, 600);
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
