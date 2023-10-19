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
    short port;
    int seed;
    char addr[16];
    char endpoint[16];
    char data[1024];
};

struct msg_reply
{
    int status;
    int backup_addr;
    short backup_port;
};

struct rpc_reply
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

// primary backup design
// 
// if error on rpc for primary, send rpc to backup
// if backup is not primary, reply with status code NOT_PRIMARY
// otherwise, serve RPC
// 
// send backup address and port in RPC replies
