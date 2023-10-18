#ifndef DMS_H
#define DMS_H

#include <map>
#include <mutex>

#include "Channel.h"
#include "drpc.h"

#define DONE 0
#define PENDING 1

struct msg
{
    uint8_t rank;
    int seed;
    int addr;
    char data[1024];
};

struct msg_reply
{
    int status;
};

class dms_server
{
private:
    drpc_server *drpc_engine;
    priority_channel<msg> buffer;
    std::map<int, Channel<bool>> completed_chn;
    std::map<int, uint8_t> completed_int;
    std::mutex sync;

    void send_worker();

public:
    dms_server(drpc_host &);

    ~dms_server();

    void start();

    static void msg_receive(dms_server *, drpc_msg &);

    void msg_send(msg &);
};

#endif