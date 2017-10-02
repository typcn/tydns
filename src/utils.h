//
//  utils.h
//  tydns
//
//  Created by typcn on 17/10/2.
//

#ifndef utils_h
#define utils_h

#include <string>
#include <unistd.h>
#include <stdlib.h>
#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

inline static std::string ip2str(const sockaddr *addr){
    std::string str;
    switch(addr->sa_family) {
        case AF_INET: {
            struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
            char s[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr_in->sin_addr), s, INET_ADDRSTRLEN);
            str = s;
            str = str + ":" + std::to_string(ntohs(addr_in->sin_port));
            break;
        }
        case AF_INET6: {
            struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
            char s[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(addr_in6->sin6_addr), s, INET6_ADDRSTRLEN);
            str = s;
            str = str + ":" + std::to_string(ntohs(addr_in6->sin6_port));
            break;
        }
        default:
            break;
    }
    return str;
}

inline static int str2ip(std::string ip, int port, sockaddr **addr){
    if(ip.find(":") != std::string::npos){
        int af = AF_INET6;
        sockaddr_in6 *sa = (sockaddr_in6 *)malloc(sizeof(sockaddr_in6));
        memset((void *)sa, 0, sizeof(sockaddr_in6));
        int rv = inet_pton(af, ip.c_str(), &(sa->sin6_addr));
        if(rv != 1){
            free(sa);
            return -1;
        }
        sa->sin6_port = htons(port);
        sa->sin6_family = af;
        *addr = (sockaddr *)sa;
        return af;
    }else{
        int af = AF_INET;
        sockaddr_in *sa = (sockaddr_in *)malloc(sizeof(sockaddr_in));
        memset((void *)sa, 0, sizeof(sockaddr_in));
        int rv = inet_pton(af, ip.c_str(), &(sa->sin_addr));
        if(rv != 1){
            free(sa);
            return -1;
        }
        sa->sin_port = htons(port);
        sa->sin_family = af;
        *addr = (sockaddr *)sa;
        return af;
    }
}

inline static void set_env_var(const char *name, const char *value) {
#ifdef _WIN32
    SetEnvironmentVariableA(name, value);
#else
    setenv(name, value, 0);
#endif
}

inline static void print_bytes(int x, const uint8_t *buf){
    int i;
    for (i = 0; i < x; i++)
    {
        if (i > 0) printf(":");
        printf("%02X", buf[i]);
    }
    printf("\n");
}

inline static void set_non_blocking(int fd) {
#ifdef _WIN32
    u_long enable = 1;
    ioctlsocket(fd, FIONBIO, &enable);
#else
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
#endif
}

#endif /* utils_h */
