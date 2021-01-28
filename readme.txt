1.网络字节序转换

htonl()--"Host to Network Long"
ntohl()--"Network to Host Long"
htons()--"Host to Network Short"
ntohs()--"Network to Host Short"   
之所以需要这些函数是因为计算机数据表示存在两种字节顺序：NBO与HBO
网络字节顺序NBO（Network Byte Order）：
      按从高到低的顺序存储，在网络上使用统一的网络字节顺序，可以避免兼容性问题。
主机字节顺序（HBO，Host Byte Order）：
      不同的机器HBO不相同，与CPU设计有关，数据的顺序是由cpu决定的,而与操作系统无关。 
如 Intelx86结构下,short型数0x1234表示为34 12, int型数0x12345678表示为78 56 34 12如IBM power PC结构下,short型数0x1234表示为12 34, int型数0x12345678表示为12   34 56 78
    由于这个原因不同体系结构的机器之间无法通信,所以要转换成一种约定的数序,也就是网络字节顺序,其实就是如同powerpc那样的顺序 。在PC开发中有ntohl和htonl函数可以用来进行网络字节和主机字节的转换。

2.sockaddr和sockaddr_in
二者长度一样，都是16个字节，即占用的内存大小是一致的，因此可以互相转化。二者是并列结构，指向sockaddr_in结构的指针也可以指向sockaddr。
sockaddr常用于bind、connect、recvfrom、sendto等函数的参数，指明地址信息，是一种通用的套接字地址。 
sockaddr_in 是internet环境下套接字的地址形式。所以在网络编程中我们会对sockaddr_in结构体进行操作，使用sockaddr_in来建立所需的信息，最后使用类型转化就可以了。一般先把sockaddr_in变量赋值后，强制类型转换后传入用sockaddr做参数的函数：sockaddr_in用于socket定义和赋值；sockaddr用于函数参数。

3.fcntl
fcntl系统调用可以用来对已打开的文件描述符进行各种控制操作以改变已打开文件的的各种属性

4.epoll_ctl
epoll是linux2.6内核的一个新的系统调用，epoll在设计之初，就是为了替代select, poll线性复杂度的模型，epoll的时间复杂度为O(1), 也就意味着，epoll在高并发场景，随着文件描述符的增长，有良好的可扩展性

这个系统调用用于操作epoll函数所生成的实例（该实例由epfd指向），向fd实施op操作
EPOLL_CTL_ADD    //注册新的fd到epfd中；
EPOLL_CTL_MOD    //修改已经注册的fd的监听事件；
EPOLL_CTL_DEL    //从epfd中删除一个fd；
events成员变量：
可以是以下几个宏的集合：
EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
EPOLLOUT：表示对应的文件描述符可以写；
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
EPOLLERR：表示对应的文件描述符发生错误；
EPOLLHUP：表示对应的文件描述符被挂断；
EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里。

5.fork()
首先了解 fork 的功能：
fork调用的一个奇妙之处就是它仅仅被调用一次，却能够返回两次，它可能有三种不同的返回值：
1）在父进程中，fork返回新创建子进程的进程ID；
2）在子进程中，fork返回0；
3）如果出现错误，fork返回一个负值
在执行函数fork()时，创建了一个子进程，此时是两个进程同时运行。fork()返回两次，子进程返回值为0

6.pipe()
函数原型 int pipe(int fd[2]) 
函数传入值 fd[2]:管道的两个文件描述符，之后就是可以直接操作者两个文件描述符 
管道两端可 分别用描述字fd[0]以及fd[1]来描述，需要注意的是，管道的两端是固定了任务的。即一端只能用于读，由描述字fd[0]表示，称其为管道读端；另 一端则只能用于写，由描述字fd[1]来表示，称其为管道写端。如果试图从管道写端读取数据，或者向管道读端写入数据都将导致错误发生。一般文件的I/O 函数都可以用于管道，如close、read、write等等
能用于父子进程或者兄弟进程之间（具有亲缘关系的进程）； 单独构成一种独立的文件系统：管道对于管道两端的进程而言，就是一个文件，但它不是普通的文件，它不属于某种文件系统，而是自立门户，单独构成一种文件系 统，并且只存在与内存中
