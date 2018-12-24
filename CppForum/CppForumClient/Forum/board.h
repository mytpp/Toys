#ifndef BOARD_H
#define BOARD_H

#include "Forum/post.h"
#include <map>
#include <QDebug>

class Board final
{
public:
    Board(const QString& name, const QString& moderatorId);
    void AddInitialPost(const Post& post) {posts.emplace(post.Id(), post);}

    const QString&   ModeratorId() const { return moderatorId; }
    const QString&   Name()        const { return name; }

    std::map<QString, Post>& GetPosts()  { return posts; }

    Post& AddPost(const QString& title, const QString& content);
    bool DeletePost(const QString id);

    void SetModerator(const QString& id) { moderatorId = id; }

private:
    QString name;
    QString moderatorId;
    std::map<QString, Post> posts;
};

#endif // BOARD_H
