#include "forum.h"
#include <algorithm>
#include <iterator>
#include <QUuid>
#include <QDate>
#include <QVector>
#include <QDebug>
#include "Storage/userinfostorage.h"


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
    SetBoards();
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
    boards = {
        {//Board 1
            "C++11/14",
            "111",
            {//Posts lists
                {//Post 1
                    QUuid::createUuid().toString(),
                    "333",
                    "lambda-calculus",
                    "Qt Creator's C++ compliler frontend doesn't support init capture",
                    QDate::currentDate().addDays(-5),
                    {}
                },
                {//Post 2
                    QUuid::createUuid().toString(),
                    "222",
                    "memory model",
                    "What's new about C++'s memory model in the C++14 standard?",
                    QDate::currentDate().addDays(-4),
                    {}
                },
                {//Post 3
                    QUuid::createUuid().toString(),
                    "111",
                    "std::packaged_task",
                    "The class template std::packaged_task wraps any Callable target (function, lambda expression, bind expression, or another function object) so that it can be invoked asynchronously. Its return value or exception thrown is stored in a shared state which can be accessed through std::future objects. ",
                    QDate::currentDate().addDays(-3),
                    {//comments
                        {//1st
                            "123",
                            "niubility!",
                            QDate::currentDate().addDays(-3)
                        },
                        {//2nd
                            "222",
                            "why do I need packaged_task when I have std::async?",
                            QDate::currentDate().addDays(-3)
                        }
                    }
                },

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
//    users.insert({"000", {infrastructure::ADMINISTRATOR, 0, "a_admin", "000"} });
//    users.insert({"111", {infrastructure::MODERATOR, 2, "a_user", "111"} });
//    users.insert({"222", {infrastructure::COMMON_USER, 1, "b_user", "222"} });
//    users.insert({"333", {infrastructure::COMMON_USER, 1, "c_user", "333"} });
//    users.insert({"444", {infrastructure::COMMON_USER, 1, "d_user", "444"} });
//    users.insert({"555", {infrastructure::ADMINISTRATOR, 0, "b_admin", "555"} });

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

