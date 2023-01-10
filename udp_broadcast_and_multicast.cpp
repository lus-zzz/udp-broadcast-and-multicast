#include "udp_broadcast_and_multicast.h"

udp_broadcast::udp_broadcast()
{

}

udp_broadcast::~udp_broadcast()
{

}

bool udp_broadcast::receive_broadcast(int port)
{
    setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
 
	// 绑定地址
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_ANY);
	addrto.sin_port = htons(port);
	
	// 接收广播地址
	struct sockaddr_in from;
	bzero(&from, sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	from.sin_port = htons(port);
	
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		cout<<"socket error"<<endl;	
		return false;
	}   
 

	int nb = 0;

	struct timeval timeout = {3,0}; 
	nb = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval));
	if(nb == -1)
	{
		cout<<"set socket error..."<<endl;
		return false;
	}

	//设置该套接字为广播类型
	const int opt = 1;
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		cout<<"set socket error..."<<endl;
		return false;
	}
 
	if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1) 
	{   
		cout<<"bind error..."<<endl;
		return false;
	}
 
	int len = sizeof(sockaddr_in);
	char smsg[BUFFER_SIZE] = {0};
 
	while(!exitProgram)
	{
		//从广播地址接受消息
		int ret=recvfrom(sock, smsg, BUFFER_SIZE, 0, (struct sockaddr*)&from,(socklen_t*)&len);
		if (ret > 0)
		{		
			_on_read(smsg, ret, (struct sockaddr*)&from, len);
		}
	}
    return true;
}

void udp_broadcast::setOnReceive(onReadCB cb)
{
	if (cb) {
        _on_read = std::move(cb);
    } else {
        _on_read = [](const char* buffer, int buffer_len, struct sockaddr *addr, int addr_len) {
            cout << "Socket not set read callback, data ignored: " << buffer_len << endl;
        };
    }
}

bool udp_broadcast::send_broadcast(const char* smsg, int smsg_len, int port)
{
    setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
 
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		cout<<"socket error"<<endl;	
		return false;
	}   
	
	const int opt = 1;
	//设置该套接字为广播类型，
	int nb = 0;
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		cout<<"set socket error..."<<endl;
		return false;
	}
 
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family=AF_INET;
	addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
	addrto.sin_port=htons(port);
	int nlen=sizeof(addrto);
 
	int ret=sendto(sock, smsg, smsg_len, 0, (sockaddr*)&addrto, nlen);
	if(ret<0)
	{
		cout<<"send error: "<<  strerror(ret)<<ret<<endl;
	}
    return true;
}


udp_multicast::udp_multicast()
{

}

udp_multicast::~udp_multicast()
{

}

bool udp_multicast::send_multicast(const char* smsg, int smsg_len, const char *group, int port)
{
    setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
 
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		cout<<"socket error"<<endl;	
		return false;
	}   
 
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family=AF_INET;
	addrto.sin_addr.s_addr=inet_addr(group);
	addrto.sin_port=htons(port);
	int nlen=sizeof(addrto);
 
	//从组播地址发送消息
	int ret=sendto(sock, smsg, strlen(smsg), 0, (sockaddr*)&addrto, nlen);
	if(ret<0)
	{
		cout<<"send error : "<<  strerror(ret) <<endl;
	}
    return true;
}

bool udp_multicast::receive_multicast(const char *group, int port)
{

    setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
 
	// 绑定地址
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_ANY);
	addrto.sin_port = htons(port);
	
	struct sockaddr_in from;
	bzero(&from, sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	from.sin_port = htons(port);
	
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		cout<<"socket error"<<endl;	
		return false;
	}   

	int nb = 0;

	struct timeval timeout = {3,0}; 
	nb = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval));
	if(nb == -1)
	{
		cout<<"set socket error..."<<endl;
		return false;
	}
	 
	//设置该套接字为组播类型
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	nb = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
	if(nb == -1)
	{
		cout<<"set socket error..."<<endl;
		return false;
	}

    
 
	if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1) 
	{   
		cout<<"bind error..."<<endl;
		return false;
	}
 
	int len = sizeof(sockaddr_in);
	char smsg[100] = {0};
 
	while(!exitProgram)
	{
		//从组播地址接受消息
		int ret=recvfrom(sock, smsg, 100, 0, (struct sockaddr*)&from,(socklen_t*)&len);
		if (ret > 0)
		{		
			_on_read(smsg, ret, (struct sockaddr*)&from, len);
		}
	}
    return true;
}

void udp_multicast::setOnReceive(onReadCB cb)
{
	if (cb) {
        _on_read = std::move(cb);
    } else {
        _on_read = [](const char* buffer, int buffer_len, struct sockaddr *addr, int addr_len) {
            cout << "Socket not set read callback, data ignored: " << buffer_len << endl;
        };
    }
}
