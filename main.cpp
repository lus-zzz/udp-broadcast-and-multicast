#include <iostream>
#include <signal.h>
#include "udp_broadcast_and_multicast.h"

bool exitProgram = false;

int main(int argc, char *argv[]) {
	signal(SIGINT, [](int){exitProgram = true;});
	udp_broadcast udp_bc;
	if (atoi(argv[1]) == 1)
		udp_bc.send_broadcast(6000);

	if (atoi(argv[1]) == 2)
		udp_bc.receive_broadcast(6000);

	udp_multicast udp_mc;
	if (atoi(argv[1]) == 3)
		udp_mc.send_multicast("224.223.222.221", 6000);

	if (atoi(argv[1]) == 4)
		udp_mc.receive_multicast("224.223.222.221", 6000);

  return 0;
  
}
