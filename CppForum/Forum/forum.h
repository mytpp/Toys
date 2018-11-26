#ifndef FORUM_H
#define FORUM_H

#include <memory>
#include <vector>
#include <unordered_map>
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

    std::vector<QString> GetBoardNames() const;

    void SetCurBoard(size_t index);
    Board& GetCurBoard() { return *curBoard; }

    bool AssignModerator(const QString& id);
    bool DismissModerator();


    /*-----------------------Static Method------------------------*/
    static Forum& Get();
    static infrastructure::Response Verify(QString id, QString password);
    static QString SelectNameWhereIdEqualTo(QString id);
    static void SetExistUsers();
    static void SetBoards();

signals:
    void ReadyToDraw();
    void HideForum();

private:
    bool GetBoards();

private:
    std::vector<Board> boards;
    std::vector<Board>::iterator curBoard;

    static std::unordered_map<QString, UserInfo> users;
};

#endif // FORUM_H
