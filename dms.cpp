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
    msg_reply *r = (msg_reply *)m.rep->args;

    ds->sync.lock();

    if (ds->completed.find(p->seed) != ds->completed.end())
    {
        r->status = ds->completed[p->seed].first;
        ds->sync.unlock();
        return;
    }

    ds->completed[p->seed].first = PENDING;
    
    // add msg to the buffer
    ds->buffer.add(*p, p->rank);
    ds->sync.unlock();

    ds->completed[p->seed].second.get();

    ds->sync.lock();
    ds->completed[p->seed].first = DONE;
    r->status = DONE;

    // free the channel
    Channel<bool> *ch = &(ds->completed[p->seed].second);
    ch->~Channel();
    ds->sync.unlock();
}

void dms_server::msg_send(msg &m)
{
    completed[m.seed].second.add(true);
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
