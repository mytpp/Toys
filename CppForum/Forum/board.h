#ifndef BOARD_H
#define BOARD_H

#include "Forum/post.h"
#include <QDebug>

class Board final
{
public:
    Board(const QString& name, const std::list<Post>& posts);
    Board(const QString& name, const QString& moderatorId,
          const std::list<Post>& posts = {});
    void AddInitialPost(const Post& post) {posts.push_back(post);}
    void AddInitialComment(const Comment& comment, const QString& postId);

    const QString&   ModeratorId() const { return moderatorId; }
    const QString&   Name()        const { return name; }

    std::list<Post>& GetPosts()  { return posts; }

    Post& AddPost(const QString& id, const QString& title, const QString& content);
    bool DeletePost(size_t index);

    void SetModerator(const QString& id) { moderatorId = id; }

private:
    QString name;
    QString moderatorId;
    std::list<Post> posts;
};

#endif // BOARD_H
