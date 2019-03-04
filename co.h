#include "all.h"
#include "aco.h"
#include "evt.h"
#include "util.h"
#include "pool.h"

#define yield  gt +

atomic<bool> isInit = {false};

struct  Message {
    long tid;
    void* data;
};

unordered_map<long,aco_t*> tid_um;
aco_t* main_co = NULL;
aco_share_stack_t* sstk = NULL;
atomic_long l = ATOMIC_FLAG_INIT;
long get_tid (){
    return ++l;
}
void init(){
    aco_thread_init(NULL);
    main_co = aco_create(NULL,NULL,0,NULL,NULL);
    sstk = aco_share_stack_new(0);

    evt_on("golang_channel", [](void* data){
        aco_t* co1 = (aco_t*)data;    
        cout<<co1<<endl;
        add_task(NULL,[co1](void* data)->void{
            aco_resume(co1);
        });
    });

    start();
}


void close(){
    evt_close();
    end();
}

void box(){
    Message* msg = (Message*)aco_get_arg();
    aco_cofuncp_t run = (aco_cofuncp_t) msg->data;
    run();
    aco_exit();
}

class th{
private:
    Message msg;
    aco_t* co = NULL;
public:
    th(aco_cofuncp_t fp){
        if (!isInit) {
            init();
            isInit = true;
        }
        // 第一次传递的 是运行函数，
        msg.data = (void*)fp;
        co = aco_create(main_co, sstk,0,box,&msg); 
        msg.tid = get_tid();
        tid_um[msg.tid] = co;
        aco_resume(co);
    }
};

void co(aco_cofuncp_t fp) {
    th* t = new th(fp);
}

void* worker(function<void(void* data)> cb){
    Message* self = (Message*)aco_get_arg();
    add_task(self,cb); 
    aco_yield();
    return self->data;
}

void send(void* msg, void* data){
    Message* mess = (Message*) msg;
    mess->data = data;
    aco_resume(tid_um[mess->tid]); // 直接在线程中 恢复 协程
}

class Gen{
public:
    void* data;
    void* operator+(function<void* ()> func){
        void* result = worker([&](void* data)->void{
            void* val = func();
            send(data,val);
        });
        return result;
    }
};
Gen gt;