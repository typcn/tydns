//
//  packet.h
//  tydnsd
//
//  Created by typcn on 17/10/9.
//

#ifndef packet_h
#define packet_h

#include <stdio.h>
#include <stdint.h>

struct dns_query {
    //char *name;
    uint16_t type;
    uint16_t dns_class;
} __attribute__((__packed__));

struct dns_record {
    //char *name;
    uint16_t type;
    uint16_t dns_class;
    uint32_t ttl;
    uint16_t length;
    uint8_t *data;
} __attribute__((__packed__));


struct dns_packet {
    /* Header */
    uint16_t id;
    uint16_t flags;
    uint16_t question_count;
    uint16_t answer_rr;
    uint16_t authority_rr;
    uint16_t additional_rr;
    /* Data */
    uint8_t dns_data[0];
} __attribute__((__packed__));


#define DNS_PACKET(x) (struct dns_packet *)x

#define DNS_NEXT_QUESTION(x,pos,hostname,attr) \
    hostname = (char *)x;\
    uintptr_t __u = (uintptr_t)x + strlen(hostname); \
    pos = __u + sizeof(dns_query); \
    attr = (struct dns_query *)__u;


#define DNS_FLAGS_OK 0x8081;
#define DNS_FLAGS_NXDOMAIN 0x8381;

#endif /* packet_h */
