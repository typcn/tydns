#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "config.h"
#include "logging.h"
#include "server.h"

CREATE_LOGGER

bool port_reuse_supported();


int main(){
    INIT_LOGGER
    LOG(INFO) << "TYPCN DNS Server v" VERSION;
    int thread_num = sysconf(_SC_NPROCESSORS_ONLN);

    if(!port_reuse_supported()){
        LOG(WARNING) << "Your kernel is too f**king old , upgrade to use multithread";
        thread_num = 1;
    }
    
    for (int i = 0; i < thread_num; i++) {
        pthread_t t;
        int wid = i+1;
        pthread_create(&t, NULL, server_loop_main, &wid);
    }
    
    LOG(INFO) << "Running with " << thread_num << " threads";
    
    while (1) {
        sched_yield();
        sleep(1000);
    }
    
    return 0;
}

bool port_reuse_supported(){
#ifdef SO_REUSEPORT
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) {
        if (errno == EINVAL || errno == ENOPROTOOPT) {
            close(sock);
            return false;
        } else {
            LOG(FATAL) << "What's wrong with your kernel?";
        }
    } else {
        close(sock);
        return true;
    }
#else
    return false;
#endif
}
