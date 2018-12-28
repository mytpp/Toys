#include <QGridLayout>
#include <QMessageBox>
#include "logindialog.h"
#include "User/user.h"
#include "Forum/forum.h"
#include "forumui.h"
#include <QDebug>


LoginDialog::LoginDialog(QWidget *parent)
    :QDialog(parent)
{
    idLabel = new QLabel(this);
    idLabel->setText(tr("ID"));
    idEdit = new QLineEdit(this);

    passwordLabel = new QLabel(this);
    passwordLabel->setText(tr("PassWord"));
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton(this);
    loginButton->setText("LogIn");

    anonymousLoginButton = new QPushButton(this);
    anonymousLoginButton->setText("Anonymous Login");

    QGridLayout *loginLayout = new QGridLayout(this);
    loginLayout->addWidget(idLabel, 0, 0);
    loginLayout->addWidget(idEdit, 0, 1);
    loginLayout->addWidget(passwordLabel, 1, 0);
    loginLayout->addWidget(passwordEdit, 1, 1);
    loginLayout->addWidget(loginButton, 2, 0);
    loginLayout->addWidget(anonymousLoginButton, 2, 1);
    loginLayout->setContentsMargins(50, 50, 50, 50);

    connect(loginButton, &QPushButton::clicked,
            [idEdit = idEdit, passwordEdit = passwordEdit, this]{
        if(idEdit->text().isEmpty() || passwordEdit->text().isEmpty()){
            QMessageBox::warning(nullptr, tr("Error!"),
                                 tr("Id and Password are required!"),
                                 QMessageBox::Ok);
            return;
        }

        auto result = User::TryLogin(idEdit->text(), passwordEdit->text());
        if(result == infrastructure::SUCCESS) {
            ui::mainWindow.reset(new MainWindow);
            if (!User::Get()->LogIn()) {
                QMessageBox::warning(nullptr, tr("Error!"),
                             tr("LOAD FORUM FAILED"), QMessageBox::Ok);
                return;
            }
            idEdit->clear();
            passwordEdit->clear();
            this->hide();
        } else if (result == infrastructure::ID_NOT_FOUNTD) {
            QMessageBox::warning(nullptr, tr("Error!"),
                         tr("ID NOT FOUNTD"), QMessageBox::Ok);
        } else if (result == infrastructure::WRONG_PASSWORD) {
            QMessageBox::warning(nullptr, tr("Error!"),
                         tr("WRONG PASSWORD"), QMessageBox::Ok);
        } else if (result == infrastructure::NETWORK_ERROR) {
            QMessageBox::warning(nullptr, tr("Error!"),
                         tr("NETWORK ERROR"), QMessageBox::Ok);
        } else if (result == infrastructure::ALREADY_ONLINE) {
            QMessageBox::warning(nullptr, tr("Error!"),
                         tr("REPEATEDLY LOGIN"), QMessageBox::Ok);
        }
    });

    connect(anonymousLoginButton, &QPushButton::clicked, [=] {
        auto result = User::TryLogin("anonymous", "anonymous");
        if(result == infrastructure::SUCCESS) {
            ui::mainWindow.reset(new MainWindow);
            if (!User::Get()->LogIn()) {
                QMessageBox::warning(nullptr, tr("Error!"),
                             tr("LOAD FORUM FAILED"), QMessageBox::Ok);
                return;
            }
            idEdit->clear();
            passwordEdit->clear();
            this->hide();
        } else {
            QMessageBox::warning(nullptr, tr("Error!"),
                                 tr("Loging in Failed"), QMessageBox::Ok);
        }
    });
}

