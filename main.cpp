#include "all.h"
#include "co.h"

void gen(){
    string name = "aa";
    cout<<name<<":"<<123<<endl;

    auto v = worker([&](void* data)->void{
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
    cout<<v<<endl;

    close();

    cout<<name<<":"<<111<<endl;
    aco_exit();
}


int main(){
    init();

    th t1(gen);
    loop();
}