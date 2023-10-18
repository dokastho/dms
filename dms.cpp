#include "dms.h"

#include <thread>

dms_server::dms_server(drpc_host &host_in)
{
    drpc_engine = new drpc_server(host_in, this);
    drpc_engine->publish_endpoint("recv", (void *)dms_server::msg_receive);
}

dms_server::~dms_server()
{
    delete drpc_engine;
}

void dms_server::msg_receive(dms_server *ds, drpc_msg &m)
{
    msg *p = (msg *)m.req->args;
    // add msg to the buffer
    ds->buffer.add(*p, p->rank);
}

void dms_server::msg_send(msg &m)
{
    m.rank = 0;
}

void dms_server::send_worker()
{
    while (true)
    {
        msg m = buffer.get();
        msg_send(m);
    }
}

void dms_server::start()
{
    drpc_engine->start();
    std::thread t(&dms_server::send_worker, this);
    t.detach();
}
