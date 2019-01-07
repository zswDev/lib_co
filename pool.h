#include "all.h"
using namespace std;

using Func = function<void(void* data)>;
struct Task {
    Func func;
    void* data;
};
struct Self_Task{
    mutex self_look;
    queue<Task>* task;
};

vector<Self_Task*> vtask = {}; // 线程队列
thread_local queue<Task> self_queue; // 线程本地任务队列
mutex t_look;
atomic<bool> off_on = {true};
void work(int idx){
    
    t_look.lock();

    Self_Task st;
    st.task = &self_queue;
    vtask[idx] = &st;

    t_look.unlock();

    while(off_on){
        // TODO 无任务尝试从 其他线程窃取，或从全局队列拿
        // TODO 无任务则处于 阻塞或休眠状态，外部进行唤醒
        usleep(1000);
        st.self_look.lock();
        while(!self_queue.empty()) { 
            Task t1 = self_queue.front();
            printf("t-%i:\n",idx);
            t1.func(t1.data);
            self_queue.pop();  
        }
        st.self_look.unlock();
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
    if (off_on) {
        off_on = false;
        cout<<"thread pool close"<<endl;
    }
}

atomic_int jsq = ATOMIC_FLAG_INIT;
int self_jsq=0;
int  getid(int n=4){
    self_jsq = jsq;
    if (self_jsq >= n) {
        jsq = 0;
    }
    ++jsq;
    return self_jsq;
}


Self_Task* vt = NULL;

void add_task(void* data,Func f1) {
    Task t1;
    t1.func = f1;
    t1.data = data;
    int idx = getid();
    
    vt = vtask[idx];
    vt->self_look.lock();
    vt->task->push(t1);
    vt->self_look.unlock();
}
void wait(){
    for(;;){
        usleep(1000);
    }
}

/*
   metux vtask： 线程添加队列 ，
   工作线程间并发 ，独立的锁
   
   metux vtask[tid]： 线程 本地 任务队列，
   主线程和工作线程并发， 一线程一锁，提高并发
*/

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
