#include "forum.h"
#include <algorithm>
#include <iterator>
#include <QUuid>
#include <QDate>
#include <QDebug>

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
    GetBoards();
    curBoard = boards.begin();
}

Forum::~Forum() {}

void Forum::Show() {
    SetCurBoard(0);   //navigate to the first board
    qDebug()<<"11";
    emit ReadyToDraw();
    qDebug()<<"22";
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

bool Forum::GetBoards() {
    boards = {
        {//Board 1
            "C++11/14",
            {//Posts lists
                {//Post 1
                    QUuid::createUuid().toString(),
                    "333",
                    "lambda-calculus",
                    "Qt Creator's C++ compliler frontend doesn't support init capture",
                    QDate::currentDate().addDays(-2),
                    {}
                },
                {//Post 2
                    QUuid::createUuid().toString(),
                    "222",
                    "memory model",
                    "What's new about C++'s memory model in the C++14 standard?",
                    QDate::currentDate().addDays(-1),
                    {}
                }
            }
        },
        {//Board 2
            "C++17",
            {//Posts lists
                {//Post 1
                    QUuid::createUuid().toString(),
                    "111",
                    "structured binding",
                    "I used structured binding in my C lexical analyzer!",
                    QDate::currentDate().addMonths(-1),
                    {//comments
                        {//1st
                            "123",
                            "Awesome!",
                            QDate::currentDate().addDays(-15)
                        },
                        {//2nd
                            "222",
                            "Why my MSVC doesn's compile the C++17 code?",
                            QDate::currentDate().addDays(-10)
                        }
                    }
                },
                {//Post 2
                    QUuid::createUuid().toString(),
                    "444",
                    "file system",
                    "Can I use C++'s filesystem to implement a FUSE supported network storage?",
                    QDate::currentDate().addDays(-7),
                    {//comments
                        {//1st
                            "111",
                            "I also want to know the answer...",
                            QDate::currentDate().addDays(-2)
                        }
                    }//end comments
                }//end post 2
            }//end posts list
        }//end board 2
    };
    return true;
}

void Forum::SetExistUsers(){
    users.insert({"123", {infrastructure::COMMON_USER, 0, "pp", "456" } });
    users.insert({"000", {infrastructure::ADMINISTRATOR, 0, "a_admin", "000"} });
    users.insert({"111", {infrastructure::MODERATOR, 1, "a_user", "111"} });
    users.insert({"222", {infrastructure::COMMON_USER, 1, "b_user", "222"} });
    users.insert({"333", {infrastructure::COMMON_USER, 1, "c_user", "333"} });
    users.insert({"444", {infrastructure::COMMON_USER, 1, "c_user", "444"} });

}

