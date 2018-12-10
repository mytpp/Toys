#include "forum.h"
#include <algorithm>
#include <iterator>
#include <QUuid>
#include <QDate>
#include <QVector>
#include <QDebug>
#include "Storage/userinfostorage.h"
#include "Storage/boardsstorage.h"
#include "Storage/postsstorage.h"
#include "Storage/commentsstorage.h"


namespace std {
    //make QString hashable
    template<> struct hash<QString> {
        std::size_t operator()(const QString& s) const {
            return qHash(s);
        }
    };
}

std::unordered_map<QString, Forum::UserInfo> Forum::users;

Forum::Forum()
{
    //they must appear in order
    SetBoards();
    SetPosts();
    SetComments();

    curBoard = boards.begin();
}

Forum::~Forum() {}

void Forum::Show() {
    SetCurBoard(0);   //navigate to the first board
    emit ReadyToDraw();
}

void Forum::Hide() {
    emit HideForum();
}

std::vector<QString> Forum::GetBoardNames() const {
    std::vector<QString> reVal;

    std::for_each(boards.begin(), boards.end(),
        [&reVal](const Board& board) {
        reVal.push_back(board.Name());
    });

    return reVal;
}

void Forum::SetCurBoard(size_t index) {
    curBoard = boards.begin();
    std::advance(curBoard, index);
}

bool Forum::AssignModerator(const QString& id) {
    auto it = users.find(id);
    if(it == users.end() ||
       it->second.status == infrastructure::ADMINISTRATOR)
        return false;
    it->second.status = infrastructure::MODERATOR;
    curBoard->SetModerator(id);
    return true;
}

bool Forum::DismissModerator() {
    auto id = curBoard->ModeratorId();
    if(id.isEmpty())
        return false;

    auto it = users.find(id);
    if(it == users.end()) //necessary?
        return false;
    it->second.status = infrastructure::COMMON_USER;
    curBoard->SetModerator(tr("")); //empty str means no moderator
    return true;
}

bool Forum::IncPostCountOf(const QString &id) {
    auto it = users.find(id);
    if(it == users.end())
        return false;
    it->second.postCount++;
    return true;
}

bool Forum::DecPostCountOf(const QString &id) {
    auto it = users.find(id);
    if(it == users.end())
        return false;
    it->second.postCount--;
    return true;
}


Forum& Forum::Get() {
    static Forum _forum;
    return _forum;
}

infrastructure::Response Forum::Verify(QString id, QString password){
    auto it = users.find(id);

    if(it == users.end()) {                       //id not found
        return { infrastructure::ID_NOT_FOUNTD };
    } else if(it->second.password != password) {  //wrong password
        return { infrastructure::WRONG_PASSWORD };
    } else {                                      //passed verification
        auto& info = it->second;
        return {
            infrastructure::SUCCESS,
            info.status,
            info.postCount,
            info.name
        };
    } //end else
}

QString Forum::SelectNameWhereIdEqualTo(QString id) {
    auto it = users.find(id);
    if(it == users.end()) {
        return "Unknown";
    } else {
        return it->second.name;
    }
}

bool Forum::SetBoards() {
    ForumStorage& storage = ForumStorage::GetStorage("boards");
    QVector<QString> record;
    while (storage>>record) {
        //QString name      = record[0];
        //QString moderator = record[1];
        boards.emplace_back(record[0], record[1]);
        record.clear();
    }
    return true;
}

bool Forum::SetPosts() {
    ForumStorage& storage = ForumStorage::GetStorage("posts");
    QVector<QString> record;
    while (storage>>record) {
//        QString id       = record[0];
//        QString board    = record[1];
//        QString author   = record[2];
//        QString authorId = record[3];
//        QString title    = record[4];
//        QString content  = record[5];
        QDate birthday = QDate::fromString(record[6]);
        auto p = std::find_if(boards.begin(), boards.end(),
                     [&val = record[1]](auto& board) {return val==board.Name();});
        if(p != boards.end())
            p->AddInitialPost(
                { record[0], record[2], record[3],
                  record[4], record[5], birthday }
            );
        else
            qDebug()<<"Adding initial post failed";

        record.clear();
    }
    return true;
}

bool Forum::SetComments() {
    ForumStorage& storage = ForumStorage::GetStorage("comments");
    QVector<QString> record;
    while (storage>>record) {
//        QString id       = record[0];
//        QString board    = record[1];
//        QString postId   = record[2];
//        QString author   = record[3];
//        QString authorId = record[4];
//        QString content  = record[5];
        QDate birthday = QDate::fromString(record[6]);
        auto p = std::find_if(boards.begin(), boards.end(),
                     [&val = record[1]](auto& board) {return val==board.Name();});
        if(p != boards.end())
            p->AddInitialComment(
                { record[3], record[4], record[5], birthday},
                record[2]
            );

        record.clear();
    }
    return true;
}

void Forum::SetExistUsers(){
    ForumStorage& storage = ForumStorage::GetStorage("userinfo");
    QVector<QString> record;
    while (storage>>record) {
        QString id = record[0];
        infrastructure::Status status;
        if (record[1] == "ADMINISTRATOR")
            status =infrastructure::ADMINISTRATOR;
        else if (record[1] == "MODERATOR")
            status = infrastructure::MODERATOR;
        else if (record[1] == "COMMON_USER")
            status = infrastructure::COMMON_USER;
        else
            status = infrastructure::ANONYMOUS;
        uint16_t postCount = record[2].toInt();
        QString  name      = record[3];
        QString  password  = record[4];

        users.insert({id, {status, postCount, name, password}});
        record.clear();
    }

}

