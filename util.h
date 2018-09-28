#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

struct timeval tv;
long cache = 0;
long getNow() {
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}
void now(){
    if (cache == 0) {
        cache = getNow();
    } else {
        printf("\ndiff:%ld\n",getNow()-cache);
        cache = 0;
    }   
}