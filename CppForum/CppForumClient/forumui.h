#ifndef FORUMUI_H
#define FORUMUI_H

#include "UI/logindialog.h"
#include "mainwindow.h"
#include <memory>

namespace ui {
//declaration
extern std::unique_ptr<LoginDialog> loginDialog;
extern std::unique_ptr<MainWindow> mainWindow;

}

#endif // FORUMUI_H
