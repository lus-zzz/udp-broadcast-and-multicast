#include <iostream>
#include <signal.h>
#include "udp_broadcast_and_multicast.h"

bool exitProgram = false;

int main(int argc, char *argv[]) {
	signal(SIGINT, [](int){exitProgram = true;});
	udp_broadcast udp_bc;
	char *msg = argv[2];

	if (atoi(argv[1]) == 1){
		udp_bc.setOnReceive([](const char* buffer, int buffer_len, struct sockaddr *addr, int addr_len){
			struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
			std::cout << "Received: " << buffer << "  From: " << inet_ntoa(addr_in->sin_addr) << std::endl;
		});
		udp_bc.receive_broadcast(6000);
	}	
	if (atoi(argv[1]) == 2)
		udp_bc.send_broadcast(msg, strlen(msg), 6000);


	udp_multicast udp_mc;
	if (atoi(argv[1]) == 3){
		udp_mc.setOnReceive([](const char* buffer, int buffer_len, struct sockaddr *addr, int addr_len){
			struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
			std::cout << "Received: " << buffer << "  From: " << inet_ntoa(addr_in->sin_addr) << std::endl;
		});
		udp_mc.setTimeout(1);
		udp_mc.receive_multicast("224.223.222.220", 6000);
	}

	if (atoi(argv[1]) == 4)
		udp_mc.send_multicast(msg, strlen(msg), "224.223.222.220", 6000);



  return 0;

}
