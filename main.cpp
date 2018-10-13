#include "all.h"
#include "co.h"

void gen(){
    // TODO 解决出错问题

    string name = "aa";
    cout<<name<<":"<<123<<endl;

    auto v = yield [&]()->string{
        sleep(1);
        cout<<name<<endl;
        return "this is worker 1";
    };
    cout<<v.data<<endl;

    v = yield [&]()->string{
        sleep(1);
        cout<<name<<endl;
        return "this is worker 2";
    };
    cout<<v.data<<endl;

    /*auto v = worker([&](void* data)->void{
        sleep(1);
        cout<<name<<endl;
        send(data,"abcdfasdfasdf");
        sleep(1);
    });
    cout<<v<<endl;

    v = worker([&](void* data)->void{
        sleep(1);
        cout<<name<<endl;
        send(data,"1111111111111");
        sleep(1);
    });
    cout<<v<<endl; */

    close();

    cout<<name<<":"<<111<<endl;
    aco_exit();
}


int main(){
    init();

    th t1(gen);
    loop();
}