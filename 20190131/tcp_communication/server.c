#include <func.h>

int main(int argc,char **argv)
{
    args_check(argc,3);
    struct sockaddr_in my_sockaddr;
    memset(&my_sockaddr,0,sizeof(struct sockaddr_in));
    my_sockaddr.sin_family=AF_INET;
    my_sockaddr.sin_addr.s_addr=inet_addr(argv[1]);
    my_sockaddr.sin_port=htons(atoi(argv[2]));
    int sfd = socket(AF_INET,SOCK_STREAM,0);//套接字描述符(sfd,new_fd)拥有两个缓冲区，发送和接收缓冲区
    //sfd缓冲区用于存储本次通信的ip,端口等信息，即结构体sockaddr_in，new_fd用于存储通信的内容
    if(-1==sfd)
    {
        printf("socket");
        return -1;
    }
    //setsockopt 设置sfd的属性，端口可重用
    int reuse = 1;
    int ret=setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    if(-1==ret)
    {
        printf("setsockopt");
        return -1;
    }
    // printf("the port=%x,the ip=%x\n",my_sockaddr.sin_port,my_sockaddr.sin_addr.s_addr);
    // 绑定服务器的ip和port
    ret= bind(sfd,(struct sockaddr*)&my_sockaddr,sizeof(struct sockaddr_in));
    if(-1==ret)
    {
        printf("bind");
        return -1;
    }
    //监听端口，最多允许10个连接
    listen(sfd,12);
    int new_fd;
    char buf[128]={0};
    //设置结构体，接收客服端的ip和port
    fd_set rdset,currdset;
        FD_ZERO(&currdset);
        FD_SET(sfd,&currdset);
        FD_SET(STDIN_FILENO,&currdset);
    while(1)
    {
        memcpy(&rdset,&currdset,sizeof(fd_set));
        ret = select(15,&rdset,NULL,NULL,NULL);
        if(ret>0)
        {
            if(FD_ISSET(sfd,&rdset))
            {
               struct sockaddr_in client;
               memset(&client,0,sizeof(struct sockaddr_in));
               socklen_t addrlen=sizeof(struct sockaddr_in);
               new_fd=accept(sfd,(struct sockaddr*)&my_sockaddr,&addrlen);
               if(-1==new_fd)
               {
                   printf("accpt");
                   return -1;
               }
              FD_SET(new_fd,&currdset);
            }
            if(FD_ISSET(STDIN_FILENO,&rdset))
            {
                memset(buf,0,sizeof(buf));
                ret=read(STDIN_FILENO,buf,sizeof(buf)-1);
                if(-1==new_fd)
                {
                    printf("accpt");
                    return -1;
                }
                send(new_fd,buf,strlen(buf),0);
            }
            if(FD_ISSET(new_fd,&rdset))
            {
                memset(buf,0,sizeof(buf));
                ret= recv(new_fd,buf,sizeof(buf)-1,0);
                if(0==ret)
                {
                    FD_CLR(new_fd,&currdset);
                    close(new_fd);
                }
                printf("%s\n",buf);
            }
        }
    }
    close(sfd);//结束监听，关闭服务器
    return 0;
}

