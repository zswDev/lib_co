#include "all.h"

struct List {
    function<void(void*)> self;
    List* next;
};
struct Node{
    string evt_name;
    void* data; 
};
unordered_map<string, List*> on_user;  // 监听者
queue<Node> task;   // 消息队列

void evt_on(string evt_name,function<void(void*)> cb){
    List* li = (List*)malloc(sizeof(List));
    li->self = cb;
    li->next = NULL;
    if (on_user[evt_name] == NULL) {
        on_user[evt_name] = li;
    } else {
        li->next = on_user[evt_name];
        on_user[evt_name]= li;
    }
}

mutex look;

void evt_emit(string evt_name,void* data){
    look.lock();
    Node n1;
    n1.evt_name = evt_name;
    n1.data = data;
    task.push(n1);
    look.unlock();
}

atomic<bool> isExit = {true};
void evt_close(){
    if (isExit) {
        isExit = false;
        cout<<"event loop close"<<endl;
    }
}


atomic_long idx = ATOMIC_FLAG_INIT;

class Chan {
public:
    long id=0;
    int data=0;
    aco_t* write=NULL;
    aco_t* read=NULL;
    Chan() {
        id = ++idx;
    }
};
Chan c;
queue<Chan*> msg;
void c_write(Chan& c, int data){
    // 暂停当前协程，在队列里等待写入到
    msg.push(&c);
    c.data = data;
    c.write = aco_get_co();
    aco_yield();
}
int c_read(Chan& c) {
    // 暂停当前协程，在队列里等待读取到
    c.read = aco_get_co();
    aco_yield();
    // 开一个线程恢复 write
    return c.data;
}

void loop(){
    Node n1;
    Chan* c1;
    aco_t* ch_read;
    aco_t* ch_write;

    while(isExit) {
        usleep(1000);
        look.unlock();
        while(!task.empty()){
            n1 = task.front();
            List* li = on_user[n1.evt_name];       
            while(li != NULL) {
                li->self(n1.data);
                li = li->next;
            }
            task.pop();
        }
        while(!msg.empty()){
            c1 = msg.front();
            ch_read = c1->read;
            ch_write = c1->write;
            if(ch_read != NULL && ch_write != NULL) {
                // 在线程中恢复协程
                evt_emit("golang_channel", ch_read);
                evt_emit("golang_channel", ch_write);
                msg.pop();
            }
        }
        
        look.unlock();
    }
}