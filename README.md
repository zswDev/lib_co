一个C++11 协程事件循环库 和 nodejs 的 co 框架类似，不过有工作线程池

TODO:
    
    Timer function



example:
```bash
    ./run main.cpp
```
New writing method;
```c++11
    Chan ch;
    co([]()->void{
        auto v = yield [&]()->void* {
            c_write(ch, 123); // golang_channel
            // TODO Asynchronous task, Run in thread pool
            return null;
        }
        cout<<v<<endl;
    })
    co([]()->void{
        auto v = yield [&]()->void* {
            auto v = c_read(ch)
            cout<<v<<endl;
            // TODO Asynchronous task, Run in thread pool
            return null;
        }
        cout<<v<<endl;
    })
    loop();

```


a c++11 continue library and event loop library