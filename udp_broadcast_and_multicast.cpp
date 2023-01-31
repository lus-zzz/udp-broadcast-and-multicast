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
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
	{   
		cout<<"socket error"<<endl;	
		return false;
	}   
 

	int nb = 0;
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

	startTP = std::chrono::system_clock::now();

 
	while(!exitProgram)
	{
		//从广播地址接受消息
		int ret=recvfrom(sock, smsg, BUFFER_SIZE, 0, (struct sockaddr*)&from,(socklen_t*)&len);
		if (ret > 0)
		{		
			_on_read(smsg, ret, (struct sockaddr*)&from, len);
		}

		finishTP = std::chrono::system_clock::now();


		int wait_time =  std::chrono::duration_cast<std::chrono::seconds>(finishTP - startTP).count();

		if (is_timeout && wait_time >= timeout_sec)
			break;
	}
    return true;
}

udp_interface::udp_interface()
{
	_on_read = [](const char* buffer, int buffer_len, struct sockaddr *addr, int addr_len) {
		cout << "Socket not set read callback, data ignored: " << buffer_len << endl;
	};
}

udp_interface::~udp_interface()
{
}

void udp_interface::setOnReceive(onReadCB cb)
{
	if (cb) {
        _on_read = std::move(cb);
    }
}

void udp_interface::setTimeout(int seconds)
{
	if (seconds){
		is_timeout = true;
		timeout_sec = seconds;
	}
}

bool udp_broadcast::send_broadcast(const char* smsg, int smsg_len, int port)
{
    setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
 
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
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
	close(sock);
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
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
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

		//发送组播忽略本机
	int loop = 0;
	int nb = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loop,sizeof(loop));
	if(nb == -1)
	{
		cout<<"set socket error..."<<endl;
		return false;
	}
 
	//从组播地址发送消息
	int ret=sendto(sock, smsg, smsg_len, 0, (sockaddr*)&addrto, nlen);
	if(ret<0)
	{
		cout<<"send error : "<<  strerror(ret) <<endl;
	}
	close(sock);
    return true;
}

bool udp_multicast::receive_multicast(const char *group, int port)
{

    setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 


	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) 
	{   
		cout<<"socket error"<<endl;	
		return false;
	}
 
	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	   

	int nb = 0;
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

    
 
	if(bind(sock,(struct sockaddr *)&(addr), sizeof(addr)) == -1) 
	{   
		cout<<"bind error..."<<endl;
		return false;
	}
 
	int len = sizeof(sockaddr_in);
	char smsg[BUFFER_SIZE] = {0};

	startTP = std::chrono::system_clock::now();
//to do something

 
	while(!exitProgram)
	{
		//从组播地址接受消息
		int ret=recvfrom(sock, smsg, BUFFER_SIZE, 0, (struct sockaddr*)&addr,(socklen_t*)&len);
		if (ret > 0)
		{		
			_on_read(smsg, ret, (struct sockaddr*)&addr, len);
		}
		finishTP = std::chrono::system_clock::now();

		int wait_time =  std::chrono::duration_cast<std::chrono::seconds>(finishTP - startTP).count();

		if (is_timeout && wait_time >= timeout_sec)
			break;
	}
    return true;
}
