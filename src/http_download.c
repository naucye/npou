#include <sys/ioctl.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include "http_download.h"

void print_error(const char *msg) {
    perror(msg);
    exit(1);
}

//获取服务端地址和别名
int get_http_info(const char *host_name,HTTP_INFO *http_info) {
    //判断是否为IP地址
    int a,b,c,d;
    if (sscanf(host_name,"%d.%d.%d.%d",&a,&b,&c,&d) == 4){
        if (a >= 0x0 && a <= 0xff && b >= 0x0 && b <= 0xff && c >=0 && c <= 0xff && d >=0 && d <= 0xff){
            strcpy(http_info->ip_addr,host_name);
            strcpy(http_info->aliases,"no aliases");
            return 0;
        }
    }
    struct hostent *phost = gethostbyname(host_name);
    if (!phost)
        return -1;

    inet_ntop(AF_INET,phost->h_addr_list[0],http_info->ip_addr,sizeof(http_info->ip_addr));
    if (*phost->h_aliases != NULL)
        strcpy(http_info->aliases,phost->h_aliases[0]);
    else
        strcpy(http_info->aliases,"no aliases");
    return 0;
}

//发送http报文请求
int send_http_header(int sfd,HOST_INFO host_info) {
    char http_header[BUFSIZ];
    bzero(http_header,sizeof(http_header));
    sprintf(http_header,\
			"GET %s HTTP/1.1\r\n"\
			"Host: %s\r\n"\
			"Connection: Keep-Alive\r\n"\
			"Content-Type: application/octet-stream\r\n"\
			"\r\n",host_info.file_path,host_info.host_name);
    return write(sfd,http_header,strlen(http_header));
}



//解析http报文头
int parse_http_header(int sfd,HTTP_INFO *http_info) {
    char buffer[BUFSIZ],temp[BUFSIZ],*ptr;
    bzero(buffer,sizeof(buffer));
    bzero(temp,sizeof(temp));
    int len,n = 0;
    while((len = read(sfd,buffer,1)) != 0){
        temp[n] = *buffer;
        if (*buffer == '\n'){
            ptr = strstr(temp,"HTTP/");
            if (ptr != NULL){
                sscanf(ptr, "%*s %d", &http_info->status_code);
            }
            ptr = strstr(temp,"Content-Length:");
            if (ptr != NULL){
                ptr = strchr(ptr,':');
                ptr++;
                http_info->file_size = strtoul(ptr,NULL,10);
            }
            ptr = strstr(temp,"Content-Type:");
            if (ptr != NULL){
                ptr = strchr(ptr,':');
                ptr++;
                strcpy(http_info->content_type,ptr);
                http_info->content_type[strlen(ptr) - 1] = '\0';
            }
            //printf("%s",temp);
            if (temp[0] == '\r' && temp[1] == '\n')
                break;
            bzero(temp,sizeof(temp));
            n = -1;
        }
        n++;
    }
    return sfd;
}

int connect_server(const char *ip_addr,unsigned short port) {
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if (sfd == -1)
        print_error("socket");

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;

    //默认80端口
    serv_addr.sin_port = htons(port);

    //根据域名获取到ip地址
    int ret = inet_pton(AF_INET,ip_addr,&serv_addr.sin_addr.s_addr);
    if (ret == -1)
        print_error("inet_pton");

    int flags = fcntl(sfd,F_GETFL,0);

    flags |= O_NONBLOCK;
    fcntl(sfd,F_SETFL,flags);

    //连接服务器
    ret = connect(sfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    if (ret != 0 && errno == EINPROGRESS){
        struct pollfd pfd;
        socklen_t optlen;
        int optval,timeout = 10 *1000;
        pfd.fd = sfd;
        pfd.events = POLLOUT;

        if ((ret = poll(&pfd,1,timeout)) == 1){
            //建立连接成功
            optlen = sizeof(optval);
            ret = getsockopt(sfd,SOL_SOCKET,SO_ERROR,&optval,&optlen);
            errno = optval;
            ret = optval == 0 ? 0 : -1;
        }
        else if (!ret){
            //连接超时
            errno = ETIMEDOUT;
            ret = -1;
        }
        else
            print_error("poll failed");
    }
    flags &= ~O_NONBLOCK;
    fcntl(sfd,F_SETFL,flags);
    return ret == -1 ? -1 : sfd;
}

//打印进度条
void print_progress_bar(const char *file_name,float sum,float file_size) {
    float percent = (sum / file_size) * 100;
    char *sign = "#";
    if ((int)percent != 0){
        sign = (char *)malloc((int)percent + 1);
        strncpy(sign,"###############################",(int) percent);
    }
    printf("%s %7.2f%% [%-*.*s] %.2f/%.2f mb\r",file_name,percent,30,(int)percent / 2,sign,sum / 1024.0 / 1024.0,file_size / 1024.0 / 1024.0);
    if ((int)percent != 0)
        free(sign);
    fflush(stdout);
}

unsigned long download(int sfd,HOST_INFO host_info,HTTP_INFO http_info) {
    umask(0111);
    int len;
    unsigned long sum = 0;
    char buffer[BUFSIZ] = { 0 };
    int fd = open(host_info.new_name,O_CREAT | O_WRONLY | O_TRUNC,0777);
    if (fd == -1)
        print_error("open");
    while((len = read(sfd,buffer,sizeof(buffer))) > 0){
        write(fd,buffer,len);
        sum+=len;
        print_progress_bar(host_info.new_name,(float)sum,(float)http_info.file_size);
        if (http_info.file_size == sum){
            printf("\n");
            break;
        }
    }
    close(fd);
    close(sfd);
    return sum;
}

//解析url
void parse_http_url(char *url,HOST_INFO *host_info) {
    char *protocol[] = {"http://","https://",NULL};
    int i,len = 0;

    //删除http/https协议头
    for (i = 0;protocol[i] != NULL;i++)
        if (!strncmp(url,protocol[i],strlen(protocol[i]))){
            len = strlen(protocol[i]);
            break;
        }


    char *ptr = strchr(url + len,':');
    if (ptr != NULL){
        //提取域名
        strncpy(host_info->host_name,url + len,strlen(url + len) - strlen(ptr));
        //提取端口
        sscanf(++ptr,"%5hu",&host_info->port);
        //判断端口是否为有效端口
        if (host_info->port > 65535){
            printf("invalid port\n");
            exit(1);
        }
        //提取文件路径
        while(*ptr != '/')
            ptr++;
        strcpy(host_info->file_path,ptr);
        //提取文件名
        ptr = strrchr(host_info->file_path,'/');
        ptr++;
        strcpy(host_info->new_name,ptr);
    }
    else{
        ptr = strchr(url + len,'/');
        //提取域名
        strncpy(host_info->host_name,url + len,strlen(url + len) - strlen(ptr));
        //设置为默认端口
        host_info->port = 80;
        //提取文件路径
        strcpy(host_info->file_path,ptr);
        //提取文件名
        ptr = strrchr(host_info->file_path,'/');
        ptr++;
        strcpy(host_info->new_name,ptr);
    }
}

//获取下载的文件大小
unsigned long get_file_size(const char *file_name) {
    struct stat buf;
    if (stat(file_name,&buf) == -1)
        return 0;
    else
        return buf.st_size;
}

//get请求
int http_get(char *url,char *new_name) {
    bzero(&host_info,sizeof(host_info));
    bzero(&http_info,sizeof(http_info));

    //解析url
    parse_http_url(url,&host_info);

    if (new_name != NULL)
        strcpy(host_info.new_name,new_name);

    //根据域名获取IP地址
    if (get_http_info(host_info.host_name,&http_info) == -1)
        print_error("gethostbyname");

    //连接服务器
    int sfd = connect_server(http_info.ip_addr,host_info.port);
    if (sfd == -1)
        print_error("connect failed");
    //发送http报文
    int	ret = send_http_header(sfd,host_info);
    if (ret == -1)
        print_error("write");

    //解析http报头
    sfd = parse_http_header(sfd,&http_info);

    if (http_info.status_code != 200){
        printf("Error: The file could not be downloaded, the remote host returned: %d\n",http_info.status_code);
        exit(-1);
    }

    //下载文件
    unsigned long download_size = download(sfd,host_info,http_info);
    unsigned long file_size = get_file_size(host_info.new_name);
    if (download_size != file_size){
        printf("download %s failure\n",host_info.new_name);
        remove(host_info.new_name);
    }
    return 0;
}

//如果按下了ctrl+c
void sig_handler(int num){
    if (num == SIGINT){
        printf("\ndownload %s abort\n",host_info.new_name);
        if (!access(host_info.new_name,F_OK))
            remove(host_info.new_name);
        putc('\n',stdout);
        exit(0);
    }
}