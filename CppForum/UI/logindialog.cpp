#include <QGridLayout>
#include <QMessageBox>
#include "logindialog.h"
#include "User/user.h"
#include "Forum/forum.h"
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
//    loginLayout->addWidget(loginButton, 2, 0, 1, 2, Qt::AlignCenter);
//    loginLayout->addWidget(anonymousLoginButton, 3, 0, 1, 2, Qt::AlignCenter);
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

        if(result == infrastructure::ID_NOT_FOUNTD) {
            QMessageBox::warning(nullptr, tr("Error!"),
                                 tr("ID NOT FOUNTD"), QMessageBox::Ok);
        } else if (result == infrastructure::WRONG_PASSWORD) {
            QMessageBox::warning(nullptr, tr("Error!"),
                                 tr("WRONG PASSWORD"), QMessageBox::Ok);
        } else {
            idEdit->clear();
            passwordEdit->clear();
            User::Get()->LogIn();
            this->hide();
        }
    });

    connect(anonymousLoginButton, &QPushButton::clicked, [=] {
        auto result = User::TryLogin("anonymous", "anonymous");
        if(result == infrastructure::SUCCESS) {
            idEdit->clear();
            passwordEdit->clear();
            User::Get()->LogIn();
            this->hide();
        } else {
            QMessageBox::warning(nullptr, tr("Error!"),
                                 tr("Loging in Failed"), QMessageBox::Ok);
        }
    });
}

