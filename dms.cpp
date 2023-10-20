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

    if (ds->completed_int.find(p->seed) != ds->completed_int.end())
    {
        r->status = ds->completed_int[p->seed];
        ds->sync.unlock();
        return;
    }

    ds->completed_int[p->seed] = PENDING;
    ds->completed_chn[p->seed] = Channel<bool>();

    // add msg to the buffer
    ds->buffer.add(*p, p->rank);
    ds->sync.unlock();

    // wait for msg to forward successfully
    bool success = ds->completed_chn[p->seed].get();

    ds->sync.lock();
    if (success)
    {
        ds->completed_int[p->seed] = DONE;
        r->status = DONE;
    }
    else
    {
        ds->completed_int[p->seed] = FAILED;
        r->status = FAILED;
    }

    // free the channel
    ds->completed_chn.erase(p->seed);
    ds->sync.unlock();
}

void dms_server::msg_send(msg &m)
{
    drpc_client c;
    drpc_host dest{m.addr, m.port};

    rpc_reply r{PENDING};

    rpc_arg_wrapper req{m.data, sizeof(m.data)};
    rpc_arg_wrapper rep{&r, sizeof(rpc_reply)};

    int status = DONE;
    int timeout = 16;
    do
    {
        status = DONE;
        r.status = PENDING;
        if (status != DONE)
        {
            if (timeout > 1000)
            {
                completed_chn[m.seed].add(false);
                return;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
            timeout *= 2;
        }
        
        status = c.Call(dest, m.endpoint, &req, &rep);
    } while (status != DONE && r.status != DONE);
    completed_chn[m.seed].add(true);
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
