#ifndef INFRASTRUCTURE_H
#define INFRASTRUCTURE_H
#include <cstdint>
#include <QString>

namespace infrastructure {

    enum Status: uint8_t
    {
        COMMON_USER,
        ADMINISTRATOR,
        MODERATOR,
        ANONYMOUS
    };

    enum Error: uint8_t
    {
        SUCCESS = 0,
        ID_NOT_FOUNTD = 1,
        WRONG_PASSWORD = 2
    };

    struct Response final
    {
        Error  error;
        Status status;
        uint16_t postCount;
        QString name;  //what about transmitting it in network?
    };

    struct Profile final
    {
        QString id;
        QString name;
        Status status;
        size_t postCount;
    };


}

#endif // INFRASTRUCTURE_H
