#include "all.h"
#include "co.h"

void gen(){

    string name = "aa";

    auto v = yield [&]()->string{
        sleep(1);
        cout<<name<<endl;
        return "this is worker 1";
    };
    cout<<v.data<<endl;

    v = yield [&]()->string{
        sleep(1);
        //cout<<name<<endl;
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
    //close();

    aco_exit();
}


int main(){
    co(gen);
    co(gen);
    loop();
}