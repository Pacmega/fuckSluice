#ifndef SIMULATIONCOMMUNICATOR_H_
#define SIMULATIONCOMMUNICATOR_H_ 

#include "lib/createTCPClientSocket.h"		// Used to create the TCP client socket
#include "lib/enums.h"

const int RCVBUFSIZE = 32;    /* Size of receive buffer */

class SimulationCommunicator
{
public:
	SimulationCommunicator(int port);
	~SimulationCommunicator();

	char* sendMessage(const char message[]);

private:
	// int port; <- Maybe not used since the auxiliary handles this?
	int sock; // Socket descriptor
	char echoBuffer[RCVBUFSIZE];

	char* receiveMessage();
	int sizeOfMessage(const char message[]);
};

#endif