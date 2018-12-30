#ifndef BOARD_H
#define BOARD_H

#include "Forum/post.h"
#include <mutex>
#include <map>
#include <QDebug>

class Board final
{
public:
    Board(const QString& name, const QString& moderatorId);
    void AddInitialPost(const Post& post) {posts.emplace(post.Id(), post);}
    void AddInitialComment(const Comment& comment, const QString& postId);

    const QString& ModeratorId() const { return moderatorId; }
    const QString& Name()        const { return name; }

    std::map<QString, Post> GetPosts() const;

    std::pair<bool, QString> AddPost(const QString& title,
                                     const QString& content,
                                     const QString& author,
                                     const QString& authorId);
    bool DeletePost(const QString& id);
    bool AddComment(const QString& postId,
                    const QString& content,
                    const QString& author,
                    const QString& authorId);

    void SetModerator(const QString& id) { moderatorId = id; }

private:
    mutable std::mutex postsMutex;
    QString name;
    QString moderatorId;
    std::map<QString, Post> posts;
};

#endif // BOARD_H
