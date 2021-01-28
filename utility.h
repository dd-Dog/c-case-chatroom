#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define EPOLL_SIZE 5000
#define BUF_SIZE 0XFFFF
#define SERVER_WELCOME "欢迎来到二狗子们的聊天室，你的ID号为：二狗#%d"
#define SERVER_MESSAGE "二狗子 %d 说 >> %s"

#define EXIT "EXIT"
#define CAUTION "这是只能说数字的地方！"

/*定义一个列表*/
//保存所有的客户端连接
struct plist {
	int data[100];
	int size; //当前有多少个
} clients_list;

void list_init(struct plist *list);
void list_add(struct plist *list, int value);
void list_remove(struct plist *list, int value);
void list_show(struct plist *list);

void list_init(struct plist *list) {
	list->size = 0;
}

void list_add(struct plist *list, int value) {
	list->data[list->size++] = value;
}

void list_remove(struct plist *list, int value) {
	//删除需要遍历数组,并进行移位,因为并没有顺序要求
	if (list == NULL || list->size <= 0) {
		printf("list为空，删除失败");
		return;
	}
	for (int i = 0; i < list->size; i++) {
		if (value == list->data[i]) {
			list ->data[i] = list->data[list->size - 1];//把最后一位移动要删除的位置
			list->size = list->size - 1;//数量减1
		}
	}
}

void list_show(struct plist *list) {
	if (list == NULL) {
		printf("list为空");
		return;
	}
	printf("[");
	for (int i = 0; i < list->size; i++) {
		printf("%d ", list->data[i]);
	}
	printf("]\n");
}

/*设置socket为非阻塞式*/
int setnonblocking(int sockfd){
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)| O_NONBLOCK);
	return 0;
}

void addfd(int epollfd, int fd, int enable_et){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if(enable_et){
		ev.events = EPOLLIN |EPOLLET;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	setnonblocking(fd);
	printf("fd=%d added to epoll!\n\n",fd);
}

int sendBroadcastMessage(struct plist *list, int clientfd){
	char buf[BUF_SIZE] , message[BUF_SIZE];
	bzero(buf, BUF_SIZE);
	bzero(message, BUF_SIZE);

	printf("从客户端(Clientfd=%d)读取\n", clientfd);
	int len = recv(clientfd, buf, BUF_SIZE, 0);
	if(len == 0){
		//客户端关闭了连接
		close(clientfd);
		list_remove(list, clientfd);
		printf("移除了客户端(Clientfd=%d),当前还有%d个人在聊天室\n", clientfd,(int)list->size);
	} else {
		//发送消息广播
		if(list->size == 1){
			send(clientfd, CAUTION, strlen(CAUTION), 0);
			return len;
		}
		//消息格式化
		sprintf(message, SERVER_MESSAGE, clientfd, buf);
		int tempfd = -1;
		for(int i=0; i < list->size; i++){
			tempfd = list->data[i];
			if(tempfd != -1){
				if(send(tempfd, message, BUF_SIZE, 0) < 0){
					perror("error");
					exit(-1);
				}
			}
		}
	}
	return len;
}
#endif //UTILITY_H_INCLUDED
