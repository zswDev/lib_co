#include "all.h"
#include "aco.h"
#include "evt.h"
#include "util.h"
#include "pool.h"

#define yield  c +


struct  Message {
    long tid;
    string data;
};

unordered_map<long,aco_t*> tid_um;
aco_t* main_co = NULL;
aco_share_stack_t* sstk = NULL;
long l = 0;
long get_tid (){
    return getNow() + ++l;
}
void init(){
    aco_thread_init(NULL);
    main_co = aco_create(NULL,NULL,0,NULL,NULL);
    sstk = aco_share_stack_new(0);

    evt_on("run", [](void* data){
        Message* self = (Message*)data;
        aco_resume(tid_um[self->tid]);
    });

    start();
}

void close(){
    evt_close();
    end();
}
class th{
private:
    Message msg;
    aco_t* co = NULL;
public:
    th(aco_cofuncp_t fp){
        co = aco_create(main_co, sstk,0,fp,&msg); 
       msg.tid = get_tid();
        tid_um[msg.tid] = co;
        aco_resume(co);
    }
};

string worker(function<void(void* data)> cb){
    Message* self = (Message*)aco_get_arg();
   // thread th1(cb,self);
    //th1.detach();
    add_task(self,cb); 
    aco_yield();
    return self->data;
}

void send(void* msg, string data){
    Message* mess = (Message*) msg;
    mess->data = data;
    evt_emit("run",msg);
}

class Co{
public:
    string data;
    Co operator+(function<string()> func){
        string data = worker([&](void* data)->void {
            string val = func();
            send(data,val);
        });
        Co c1;
        c1.data = data;
        return c1;
    }
};
Co c;