一个C++11 协程事件循环库 和 nodejs 的 co 框架类似，不过有工作线程池

TODO:
    
    Timer function



example:
```bash
    ./run main.cpp
```
New writing method;
```c++
    co([]()->void{
        auto v = yield [&]()->void* {
            // TODO Asynchronous task, Run in thread pool
            return null;
        }
        aco_exit();
    })
    loop();

```


a c++11 continue library and event loop library