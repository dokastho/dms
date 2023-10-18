#include "dms.h"

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

    ds->msg_send(p);
}

void dms_server::msg_send(msg *m)
{
    delete m;
}

void dms_server::start()
{
    drpc_engine->start();
}
