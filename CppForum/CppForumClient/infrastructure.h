#ifndef INFRASTRUCTURE_H
#define INFRASTRUCTURE_H
#include <cstdint>
#include <QString>

namespace infrastructure {

    enum Status: uint8_t
    {
        ADMINISTRATOR = 0,
        COMMON_USER = 1,
        MODERATOR = 2,
        ANONYMOUS = 3
    };

    enum Error: uint8_t
    {
        SUCCESS = 0,
        ID_NOT_FOUNTD = 1,
        WRONG_PASSWORD = 2,
        ALREADY_ONLINE = 3,
        NETWORK_ERROR = 4,
        UNKNOWN_ERROR
    };

    struct Response final
    {
        Error  error;
        Status status;
        uint32_t postCount;
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
