#include "all.h"
#include "co.h"

class V {
public:
    ~V(){
        cout<<"exit"<<endl;
    }
};

atomic_int id=ATOMIC_FLAG_INIT;

void gen(){
    // TODO 改进偶然性 错误
    int value = 123;
    evt_emit("data",&value); // 监听事件都会 在主线程执行!!

    string name = "aa";
    yield [&]()->void*{
        sleep(1);
        ++id;
        cout<<name<<this_thread::get_id()<<","<<id<<endl;
    };

    auto val = yield [&]()->void*{ // 这里是个异步任务，主线程不阻塞，这些任务可并行执行, 会发生并发
        sleep(1);
        ++id;
        cout<<name<<this_thread::get_id()<<","<<id<<endl;
        int* ii = new int(123);
        return (void*)ii;
    };
    cout<<*(int*)val<<","<<this_thread::get_id()<<endl;
    close();
}

int main(){
  co(gen);
  co(gen);

  evt_on("data", [](void* data){
      int* v = (int*)data;
      cout<<"evt,"<<*v<<endl;
  });

  loop();
}