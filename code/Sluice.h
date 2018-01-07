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
	
	void passInterrupt();
	int start();
	int allowEntry();
    int allowExit();

private:
	CommunicationHandler cHandler;
	Door leftDoor;
	Door rightDoor;

	bool emergency;
	SluiceState stateBeforeEmergency;

	bool closeValves(DoorSide side);
	int sluiceUp(WaterLevel currentWLevel);
	int sluiceDown(WaterLevel currentWLevel);
};

#endif