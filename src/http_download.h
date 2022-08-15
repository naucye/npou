//
// Created by naucye on 2022/8/15.
//

#ifndef NPOU_HTTP_DOWNLOAD_H
#define NPOU_HTTP_DOWNLOAD_H
#include <netinet/in.h>

typedef struct HTTP_INFO{
    unsigned long file_size;
    char content_type[256];
    char file_name[256];
    char ip_addr[INET_ADDRSTRLEN];
    char aliases[256];
    int status_code;
}HTTP_INFO;

typedef struct HOST_INFO{
    char host_name[256];
    char file_path[1024];
    unsigned short port;
    char new_name[256];
}HOST_INFO;

HOST_INFO host_info;
HTTP_INFO http_info;

int http_get(char *url,char *new_name);

unsigned long download(int sfd,HOST_INFO host_info,HTTP_INFO http_info);

#endif //NPOU_HTTP_DOWNLOAD_H
