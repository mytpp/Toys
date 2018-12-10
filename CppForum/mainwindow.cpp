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
        ui::loginDialog->show();qDebug()<<"a";
//        this->close();
        delete ui::mainWindow;qDebug()<<"b";
        ui::mainWindow = new MainWindow();qDebug()<<"c";
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::Initiate() {
    container = new QWidget(this);
    this->setCentralWidget(container);

    infoBar = new UserInfoBar(this);
    boardArea = new BoardsArea(this);

    //post area
    postArea = new PostsArea(this);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(postArea);
    scrollArea->setWidgetResizable(true);

    //main area
    mainArea = new QSplitter(this);
    mainArea->addWidget(boardArea);
    mainArea->addWidget(scrollArea);
    mainArea->setStretchFactor(0, 1);
    mainArea->setStretchFactor(1, 3);

    //set layout of the main window
    vLayout = new QVBoxLayout(container);
    vLayout->addWidget(infoBar);
    vLayout->addWidget(mainArea);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->resize(800, 600);
    this->show();
}

void MainWindow::RefreshUserInfoBar() {
    infoBar->Refresh();
}

void MainWindow::RefreshPostArea() {
    delete postArea;
    postArea = new PostsArea(this);
    scrollArea->setWidget(postArea);
    mainArea->addWidget(scrollArea);
    //mainArea->setStretchFactor(0, 1);
    mainArea->setStretchFactor(1, 3);
}
