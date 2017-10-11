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
    char *name;
    uint16_t type;
    uint16_t dns_class;
};

struct dns_response {
    char *name;
    uint16_t type;
    uint16_t dns_class;
    uint32_t ttl;
    uint16_t length;
    uint8_t *data;
};

struct dns_packet{
    uint16_t id;
    uint16_t flags;
    uint16_t question_count;
    uint16_t answer_rr;
    uint16_t authority_rr;
    uint16_t additional_rr;
    struct dns_query **queries;
    struct dns_response **responses;
};

#endif /* packet_h */
