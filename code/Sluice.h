#ifndef SLUICE_H_
#define SLUICE_H_

#include "lib/enums.h"
#include "CommunicationHandler.h"
#include "Door.h"

class Sluice
{
public:
	Sluice(int port);
	~Sluice();
	
	int start();
	int allowEntry();
    int allowExit();
    void passInterrupt();
    CommunicationHandler cHandler;

private:
	Door leftDoor;
	Door rightDoor;

	bool emergency;
	SluiceState stateBeforeEmergency;

	int sluiceUp(WaterLevel currentWLevel);
	int sluiceDown(WaterLevel currentWLevel);
	bool closeValves(DoorSide side);
};

#endif