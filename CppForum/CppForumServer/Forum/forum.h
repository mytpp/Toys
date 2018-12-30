#ifndef FORUM_H
#define FORUM_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <QString>
#include <optional>
#include <atomic>
#include "infrastructure.h"
#include "board.h"

//to decouple, forum doesn't reference to 'User' class
class Forum final
{
private:
    struct UserInfo
    {
        //as std::atomic<bool> is non-copyable, we need a constructor
        UserInfo(uint8_t status, uint32_t postCount,
                 QString name, QString password, bool online)
            :status(status)
            ,postCount(postCount)
            ,name(name)
            ,password(password)
            ,online(online)
        {}

        uint32_t postCount;
        QString name;
        QString password;         //need not posting back
        std::atomic<uint8_t> status;
        std::atomic<bool> online; //whether this user has loged in
    };

public:
    Forum();
    ~Forum();

    const std::vector<std::pair<QString, QString>> GetBoards() const;
    std::optional<std::reference_wrapper<Board>>
    GetBoardByName(const QString& name);


    bool AssignModerator(const QString& id, const QString& board);
    bool DismissModerator(const QString& board);

    bool IncPostCountOf(const QString& id);
    bool DecPostCountOf(const QString& id);

    bool DeletePost(const QString& id);

    std::optional<const std::list<Comment>>
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
    std::vector<std::unique_ptr<Board>> boards;

    static std::unordered_map<QString, UserInfo> users;
};

#endif // FORUM_H
