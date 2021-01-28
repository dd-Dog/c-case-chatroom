#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define PORT 10000 

int main(int argc, char** argv){
    int socketfd;
    int acceptfd;
    int recvlen;
    socklen_t socklen = 0;
    char buf[4096] = {0};
    struct sockaddr_in socketLocalAddr;
    struct sockaddr_in socketRemoteAddr;
    
    bzero(&socketLocalAddr, sizeof(socketLocalAddr));//初始化结构体
    bzero(&socketRemoteAddr, sizeof(socketRemoteAddr));

    socketLocalAddr.sin_family = AF_INET;//设置地址族，AF_INET表示IPV4
    socketLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);//设置IP,INADDR_ANY=0表示自动设置为本机IP
    socketLocalAddr.sin_port = htons(PORT);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);//IPV4, 流，TCP(0表示自动推演出使用哪种协议)
    if(socketfd < 0){
        printf("创建套接字失败!\n");
        return -1;
    }

    //绑定套接字地址到套接字句柄上
    if(0 > bind(socketfd, (void *)&socketLocalAddr, sizeof(socketLocalAddr))){
        printf("绑定地址失败!\n");
        return -1;
    }
    //开启监听，第二个参数是最大监听数量
    if(0 > listen(socketfd, 10)){
        printf("监听失败~！\n");
        return -1;
    }
    
    //阻塞方式
    //在这里阻塞知道接收到消息，参数分别是socket句柄，接收到的地址信息以及大小
    acceptfd = accept(socketfd, (void *)&socketRemoteAddr, &socklen);
    if(acceptfd < 0){
        printf("接收失败！\n");
        return 0;
    }else {
        printf("接收成功！\n");
        char *msg = "这是来自服务器接收成功后返回的消息"; 
        send(acceptfd, msg, strlen(msg), 0);
    }

    recvlen = recv(acceptfd, buf, sizeof(buf), 0);
    if(recvlen <= 0){
        printf("接收失败或者对端关闭连接！\n");
    }else {
        printf("接收消息：%s\n", buf);
    }

    close(acceptfd);
    close(socketfd);
    return 0;
}
