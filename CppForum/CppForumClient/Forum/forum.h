#ifndef FORUM_H
#define FORUM_H

#include <memory>
#include <vector>
#include <QString>
#include "infrastructure.h"
#include "board.h"
#include <QDebug>

//to decouple, forum doesn't reference to 'User' class
class Forum final: public QObject
{
    Q_OBJECT
private:
    struct UserInfo
    {
        infrastructure::Status status;
        uint16_t postCount;
        QString name;
        QString password; //need not posting back
    };

public:
    Forum();
    ~Forum();

    //draw the framework of forum, including boards list
    void Show();
    //close the "forum UI"
    void Hide();

    const std::vector<QString>& GetBoardNames() const
        { return boardNames; }

    void SetCurBoard(const QString& name);
    Board& GetCurBoard() { return *curBoard; }

    bool AssignModerator(const QString& id);
    bool DismissModerator();


    /*-----------------------Static Method------------------------*/
    static Forum& Get();
    static infrastructure::Response Verify(QString id, QString password);
    static bool LogOut();

signals:
    void ReadyToDraw();
    void HideForum();

private:
    std::unique_ptr<Board> curBoard;
    std::vector<QString> boardNames;
};

#endif // FORUM_H
