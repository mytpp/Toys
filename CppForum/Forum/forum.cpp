#include "forum.h"
#include <algorithm>
#include <iterator>

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
    //read data from file
}

Forum::~Forum() {}

void Forum::Show() {
    emit ReadyToDraw();
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

Forum& Forum::Get() {
    qDebug()<<"get";
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

void Forum::SetExistUsers(){
    users.insert({"123", {infrastructure::COMMON_USER, 0, "pp", "456" } });
    users.insert({"1", {infrastructure::COMMON_USER, 0, "a_user", "1"} });
}

