#include "utility.h"

int main(int argc, char** argv){
    int socketfd;
    int acceptfd;
    int recvlen;
    socklen_t socklen = 0;
    char buf[4096] = {0};
    struct sockaddr_in socketLocalAddr;
    struct sockaddr_in socketRemoteAddr;

    struct plist *clients_list = (struct plist*)malloc(sizeof(struct plist));
    
    bzero(&socketLocalAddr, sizeof(socketLocalAddr));//初始化结构体
    bzero(&socketRemoteAddr, sizeof(socketRemoteAddr));

    socketLocalAddr.sin_family = AF_INET;//设置地址族，AF_INET表示IPV4
    //socketLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);//设置IP,INADDR_ANY=0表示自动设置为本机IP
    socketLocalAddr.sin_addr.s_addr = inet_addr(SERVER_IP);//设置IP,INADDR_ANY=0表示自动设置为本机IP
    socketLocalAddr.sin_port = htons(SERVER_PORT);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);//IPV4, 流，TCP(0表示自动推演出使用哪种协议)
    if(socketfd < 0){
        printf("创建套接字失败!\n");
        return -1;
    }
    printf("创建套接字成功！\n");
    //绑定套接字地址到套接字句柄上
    if(0 > bind(socketfd, (void *)&socketLocalAddr, sizeof(socketLocalAddr))){
        printf("绑定地址失败!\n");
        return -1;
    }
    printf("绑定地址成功！\n");
    //开启监听，第二个参数是最大监听数量
    if(0 > listen(socketfd, 10)){
        printf("监听失败~！\n");
        return -1;
    }
    printf("开始监听客户端连接...\n");

    //在内存中创建事件表
    int epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0){
        perror("创建epoll失败");
        exit(-1);
    }
    static struct epoll_event events[EPOLL_SIZE];
    //往内核事件表中添加事件,并使用边沿触发
    addfd(epfd, socketfd, 1);

    //主循环
    while(1){
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if(epoll_events_count < 0){
            perror("epoll failure");
            break;
        }
        printf("epoll_events_count =%d\n", epoll_events_count);
        //处理就绪事件
        for(int i=0; i<epoll_events_count; i++){
            int sockfd = events[i].data.fd;
            //新用户连接
            if(sockfd == socketfd){
                struct sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                int clientfd = accept(socketfd, (struct sockaddr*)&client_address, &client_addrLength);
                if(clientfd == -1){
                    printf("接受客户端连接失败：Error no.%d: %s\n", errno, strerror(errno));
                    break;
                }
                printf("客户连接，IP:%s:%d,clientfd=%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), clientfd);

                addfd(epfd, clientfd, 1);//注册客户端连接到事件表
                list_add(clients_list, clientfd);
                printf("添加客户端 clientfd=%d 到 epoll\n", clientfd);
                printf("现在聊天室内有%d只二狗子\n", (int)clients_list->size);

                //服务端发送欢迎信息
                printf("Welcome message\n");
                char message[BUF_SIZE];
                bzero(&message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SIZE, 0);
                if(ret < 0){
                    perror("send error");
                    exit(-1);
                }
            }
            //处理用户发来的消息，并广播使用其它用户收到消息
            else {
                int ret = sendBroadcastMessage(clients_list, sockfd);
                if(ret < 0) {
                    perror("recv error");
                    exit(-1);
                }
            }
        }
    }

    close(acceptfd);
    close(socketfd);
    return 0;
}
