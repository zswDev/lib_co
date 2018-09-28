#include "all.h"
using namespace std;

thread_local int i=0;
void run(int** ptr){
    *ptr=&i;
    sleep(2);
    ++i;
    cout<<i<<endl;
}

using Func = function<void(Message* msg)>;

struct Task {
    Func func;
    Message* msg;
};

vector<queue<Task>*> vtask; // 任务组
thread_local queue<Task> self_queue;
mutex look;
atomic<bool> off_on = {true};
void work(int idx){
    look.lock();
    vtask[idx] = &self_queue;
    look.unlock();
    while(off_on){
        // TODO 无任务尝试从 其他线程窃取，或从全局队列拿
        usleep(1000);
        while(!self_queue.empty()) {
            Task t1 = self_queue.front();
            printf("\nt-%i",idx);
            t1.func(t1.msg);
            self_queue.pop();  
        }
    }
}

vector<thread*> ts;
void start(int n=4){
    for(int i=0;i<n;i++){
        vtask.push_back(NULL);
        thread* t1 = new thread(work,i);
        t1->detach();
        ts.push_back(t1);
    }
    //vtask[n] = &self_queue;
}
void end(){
    off_on = false;
}

int jsq =0;
int  getid(int n=4){
    int val;
    srand((int)time(NULL)+ ++jsq);
    val = rand() % n;
    return val;
}

void add_task(Message* msg,Func f1) {
    Task t1;
    t1.func = f1;
    t1.msg = msg;
    int idx = getid();
    
    look.lock();
    vtask[idx]->push(t1);
    look.unlock();
}

/*
int main(){
    start();
    int data[] = {9,1,31,346,7,8,1,2,76,8,901};
    for(int i=0;i<10;i++){
       
        add_task((data+i),[](void* data){
            printf(", data:%i\n",*(int*)data);
        });
    }
    

    sleep(3);
    end();
    sleep(1);
    cout<<"over"<<endl;
}*/
