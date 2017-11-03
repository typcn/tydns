#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "logging.h"
#include "config.h"
#include "server.h"
#include "utils.h"
#include "packet.h"

void *server_loop_main(void *wid){
    int worker_id = *(int *)wid;
    LOG(INFO) << "Worker #" << worker_id << " starting";
    
    int fd = ::socket(AF_INET6, SOCK_DGRAM, 0);
    if (fd < 0) LOG(FATAL) << "socket() failed with err " << strerror(errno);
    
    int enable = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *) (void *) &enable, sizeof(int)) < 0){
        LOG(ERROR) << "Failed to set SO_REUSEADDR, " << strerror(errno);
    }
    int disable = 0;
    if (::setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char *) (void *) &disable, sizeof(int)) < 0){
        LOG(ERROR) << "Failed to disable IPV6_V6ONLY, " << strerror(errno);
    }
#ifdef SO_REUSEPORT
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const char *) (void *) &enable, sizeof(int)) < 0){
        LOG(ERROR) << "Failed to set SO_REUSEPORT, " << strerror(errno);
    }
#endif

    struct sockaddr_in6 addr;
    addr.sin6_family      = AF_INET6;
    addr.sin6_port        = htons(53);
    addr.sin6_addr = in6addr_any;
    
    socklen_t cached_addrlen = sizeof(addr);
    
    if (::bind(fd, (sockaddr *)&addr, cached_addrlen) != 0) {
        LOG(FATAL) << "Failed to bind, " << strerror(errno);
    }
    
    struct sockaddr *addrptr = (struct sockaddr *)&addr;
    
    /* Query Buffers */
    uint8_t buf[INCOMING_PACKET_SIZE];
    uintptr_t question_count;
    uintptr_t response_count;
    uintptr_t position;
    char *question_hostname;
    struct dns_query *query_attr;
    
    while (1) {
        int rsize = ::recvfrom(fd, buf, INCOMING_PACKET_SIZE, 0, addrptr, &cached_addrlen);
        if(rsize < 0){
            if(errno == ETIMEDOUT || errno == EAGAIN){
                continue;
            }else if(errno == EBADF){
                LOG(FATAL) << "*** FD INVALID ***";
            }
            LOG(ERROR) << "recvfrom() failed, " << strerror(errno);
        }
        
        struct dns_packet *packet = DNS_PACKET(buf);
        
        question_count = ntohs(packet->question_count);
        response_count = 0;

#ifdef DEBUG
        LOG(VERBOSE) << "Received a packet from " << ip2str(addrptr);
        print_bytes(rsize, buf);
        LOG(VERBOSE) << "TxID:" << packet->id << " Flags:" << packet->flags << " QCount:" << ntohs(packet->question_count);
#endif

        for (int i = 0; i < question_count; i++) {
            DNS_NEXT_QUESTION(packet->dns_data,position,question_hostname,query_attr);

            LOG(VERBOSE) << "Question #" << i << " " << question_hostname << " Type: " << ntohs(query_attr->type);

            //response_count++;
        }
        
        if(!response_count){
            packet->flags = DNS_FLAGS_NXDOMAIN;
        }else{
            packet->flags = DNS_FLAGS_OK;
        }
        
        
        ::sendto(fd, buf, rsize, 0, addrptr, cached_addrlen);
        
        LOG(VERBOSE) << "====================";
        
    }
    
    return NULL;
}



