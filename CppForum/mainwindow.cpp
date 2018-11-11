#include "mainwindow.h"
#include "Forum/forum.h"
#include "UI/boardsarea.h"
#include "forumui.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, elementsExist(false)
{
    //show the main window (ie. the forum)
    connect(&Forum::Get(), &Forum::ReadyToDraw,
            this, &MainWindow::Initiate);

    //hide the forum (after a user logs out)
    connect(&Forum::Get(), &Forum::HideForum, [=]{
        ui::loginDialog->show();
        this->close();
        ui::mainWindow = new MainWindow();
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::Initiate() {
    container = new QWidget(this);qDebug()<<"0";
    this->setCentralWidget(container);;qDebug()<<"00";

    infoBar = new UserInfoBar(this);qDebug()<<"0";

    boardArea = new BoardsArea(this);qDebug()<<"2";

    //post area
    postArea = new PostsArea(this);  qDebug()<<"3";
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(postArea);
    scrollArea->setWidgetResizable(true);

    //main area
    mainArea = new QSplitter(this);  qDebug()<<"1";
    mainArea->addWidget(boardArea);  qDebug()<<"4";
    mainArea->addWidget(scrollArea);   qDebug()<<"5";
    mainArea->setStretchFactor(0, 1);
    mainArea->setStretchFactor(1, 3);

    //set layout of the main window
    vLayout = new QVBoxLayout(container); qDebug()<<"6";
    vLayout->addWidget(infoBar);
    vLayout->addWidget(mainArea);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->resize(800, 600);
    this->show();
}

void MainWindow::RefreshPostArea() {
    delete postArea;
    postArea = new PostsArea(this);
    scrollArea->setWidget(postArea);
    mainArea->addWidget(scrollArea);
    //mainArea->setStretchFactor(0, 1);
    mainArea->setStretchFactor(1, 3);
}
