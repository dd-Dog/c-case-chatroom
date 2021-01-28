#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define PORT 10000			//目标地址端口号
#define ADDR "127.0.0.1" 

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
	socketRemoteAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, ADDR, &remoteAddr);
	socketRemoteAddr.sin_addr.s_addr = remoteAddr;

	if(0 > connect(socketfd, (void *)&socketRemoteAddr, sizeof(socketRemoteAddr))){
		printf("连接失败\n");
	}else {
		printf("连接成功\n");
        while(true){
		    recv(socketfd, buf, sizeof(buf), 0);
		    printf("收到消息：%s\n", buf);
        }
	}
	close(socketfd);
	return 0;
}
