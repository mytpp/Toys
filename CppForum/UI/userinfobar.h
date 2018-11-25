#ifndef USERINFOBAR_H
#define USERINFOBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class UserInfoBar : public QWidget
{
    Q_OBJECT
public:
    explicit UserInfoBar(QWidget *parent = nullptr);

    void Refresh();

private:
    QLabel *userId;
    QLabel *userName;
    QLabel *userStatus;
    QLabel *postCount;

    QPushButton *assignModeratorButton;
    QPushButton *logoutButton;
};

#endif // USERINFOBAR_H
