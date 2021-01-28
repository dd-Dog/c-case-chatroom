#include "utility.h"


int main(){

	int socketfd = 0;
	unsigned int remoteAddr = 0;
	struct sockaddr_in socketRemoteAddr = {0};
	socklen_t socklen = 0;
	char buf[4096] = {0};

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > socketfd){
		printf("创建Socket失败！\n");
		return -1;
	}
	
	socketRemoteAddr.sin_family = AF_INET;
	socketRemoteAddr.sin_port = htons(SERVER_PORT);
	socketRemoteAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if(0 > connect(socketfd, (void *)&socketRemoteAddr, sizeof(socketRemoteAddr))){
		printf("连接失败\n");
	}else {
		printf("连接成功\n");
        //创建管道，其中fd[0]用于父进程读，fd[1]用于子进程写
        int pipe_fd[2];
        if(pipe(pipe_fd) < 0){
            perror("pipe error");
            exit(-1);
        }
        //创建epoll
        int epfd = epoll_create(EPOLL_SIZE);
        if(epfd < 0){
            perror("epoll create error");
            exit(-1);
        }

        static struct epoll_event events[2];
        //将socket和管道描述符都添加到内核事件表中
        addfd(epfd, socketfd, 1);
        addfd(epfd, pipe_fd[0], 1);

        int isClientWork = 1;//表示 客户端是否正常工作
        char message[BUF_SIZE];

        //创建子进程
        int pid = fork();
        if(pid < 0){
            perror("forl error");
            exit(-1);
        }else if(pid == 0){
            //子进程，pid=0表示子进程
            //子进程负责写入管道，因此先关闭读端
            close(pipe_fd[0]); 
            printf("Please input 'exit' to exit the chat room\n");

            while(isClientWork){
                bzero(&message, BUF_SIZE);
                fgets(message, BUF_SIZE, stdin);//从控制台读取输入内容

                //判断是否是'exit'
                if(strncmp(message, EXIT, strlen(EXIT)) == 0){
                    isClientWork = 0;
                } else {
                    //将消息写入管道
                    if(write(pipe_fd[1], message, strlen(message) - 1) < 0) {
                        perror("pipe write error");
                        exit(-1);
                    }
                }
            }
        }else{
            //父进程负责读管道数据，因此先关闭写端
            close(pipe_fd[1]);

            while(isClientWork) {
                int epoll_events_count = epoll_wait(epfd, events, 2, -1 );
                //处理就绪事件
                for(int i = 0; i < epoll_events_count ; ++i){
                    bzero(&message, BUF_SIZE);
                    if(events[i].data.fd == socketfd){
                       //服务端发来消息 
                       int ret = recv(socketfd, message, BUF_SIZE, 0);
                       if(ret == 0){
                            printf("服务端关闭了连接，fd=%d\n", socketfd);
                            close(socketfd);
                            isClientWork = 0; 
                            exit(0);
                       }else {
                            printf("接收消息：%s\n", message);
                       }
                    }else if(events[i].data.fd == pipe_fd[0]) {
                       //子进程写入事件发生，父进程处理并发送服务端
                       int ret = read(events[i].data.fd, message, BUF_SIZE);
                       if(ret == 0){
                            isClientWork = 0;
                       }else {//将消息发送给服务端
                            send(socketfd, message, BUF_SIZE, 0);
                       }
                       
                    }
                }
            }
        }
        
        if(pid){
            //关闭父进程和socket
            close(pipe_fd[0]);
            close(socketfd);
        }else {
            //关闭子进程
            close(pipe_fd[1]);
        }
	}
	return 0;
}
