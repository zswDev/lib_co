#include "all.h"
#include "co.h"

class V {
public:
    ~V(){
        cout<<"exit"<<endl;
    }
};

atomic_int id=ATOMIC_FLAG_INIT;

Chan ch;

void gen(){
    // TODO 改进偶然性 错误
    c_write(ch,99999);
    
    
   // int value = 123;
   // evt_emit("data",&value); // 监听事件都会 在主线程执行!!

    string name = "aa";
    yield [&]()->void*{
        sleep(1);
        ++id;
        cout<<name<<this_thread::get_id()<<","<<id<<endl;
    };
    cout<<"main"<<this_thread::get_id()<<endl;
    /*

    auto val = yield [&]()->void*{ // 这里是个异步任务，主线程不阻塞，这些任务不同协程可并行执行, 会发生并发
        sleep(1);
        ++id;
        cout<<name<<this_thread::get_id()<<","<<id<<endl;
        int* ii = new int(123);
        return (void*)ii;
    };
    cout<<*(int*)val<<","<<this_thread::get_id()<<endl;
    */
    close();
}
void gen1() {
    int v = c_read(ch);
    cout<<"get"<<v<<endl;

    // string name = "aa";
    // yield [&]()->void*{
    //     sleep(1);
    //     ++id;
    //     cout<<name<<this_thread::get_id()<<","<<id<<endl;
    // };
    // cout<<"main"<<this_thread::get_id()<<endl;
}

// TODO channel
// write, 暂停当前协程,切换到目标协程，等待恢复
// read, 暂停当前协程, 读取消息，恢复发信方，
// 通信标识
int main(){
  co(gen);
  co(gen1);

  evt_on("data", [](void* data){
      int* v = (int*)data;
      cout<<"evt,"<<*v<<endl;
  });

  loop();
  /*
  调用逻辑

  协程把任务抛给 任务队列,
  主线程分配任务，
  工作线程池消费队列，
  线程中恢复协程，不可并行恢复（TODO，待外部中断某些长时间线程）
  协程继续运行
  并行的，不在主线程运行，通过任务切换当前运行线程
  通信可使用 event,
  */
}