#ifndef BOARD_H
#define BOARD_H

#include "Forum/post.h"
#include <QDebug>

class Board final
{
public:
    Board(const QString& name, const std::list<Post>& posts);
    Board(const QString& name, const QString& moderatorId,
          const std::list<Post>& posts);

    const QString&         ModeratorId() const { return moderatorId; }
    const QString&         Name()        const { return name; }
    const std::list<Post>& GetPosts()    const { return posts; }

    bool AddPost(const QString& guid, const QString& title, const QString& content);
    bool DeletePost(size_t index);

    void SetModerator(const QString& id) { moderatorId = id; }

private:
    QString name;
    QString moderatorId;
    std::list<Post> posts;
};

#endif // BOARD_H
