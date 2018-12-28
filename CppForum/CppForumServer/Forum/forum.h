#ifndef FORUM_H
#define FORUM_H

#include <vector>
#include <unordered_map>
#include <QString>
#include <optional>
#include "infrastructure.h"
#include "board.h"

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
        bool online;      //record whether this user has loged in at client side
    };

public:
    Forum();
    ~Forum();

    const std::vector<Board>& GetBoards() const { return boards; }
    std::optional<std::reference_wrapper<Board>>
    GetBoardByName(const QString& name);


    bool AssignModerator(const QString& id, const QString& board);
    bool DismissModerator(const QString& board);

    bool IncPostCountOf(const QString& id);
    bool DecPostCountOf(const QString& id);

    bool DeletePost(const QString& id);

    std::optional<std::reference_wrapper<const std::list<Comment>>>
    GetComments(const QString& id);


    /*-----------------------Static Method------------------------*/
    static Forum& Get();
    static infrastructure::Response Verify(QString id, QString password);
    static bool LogOut(QString id);
    static QString SelectNameWhereIdEqualTo(QString id);
    static void SetExistUsers();

private:
    bool SetBoards();
    bool SetPosts();
    bool SetComments();

private:
    std::vector<Board> boards;

    static std::unordered_map<QString, UserInfo> users;
};

#endif // FORUM_H
