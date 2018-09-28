#include "all.h"
#include <mutex>

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
        on_user[evt_name]->next = li;
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
    isExit = false;
    cout<<"event loop close"<<endl;
}
void loop(){
    Node n1;
    while(isExit) {
        usleep(1000);
        while(!task.empty()){
            n1 = task.front();
            List* li = on_user[n1.evt_name];       
            while(li != NULL) {
                li->self(n1.data);
                li = li->next;
            }
            task.pop();
        }
    }
}