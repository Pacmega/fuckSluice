#ifndef DOOR_H_
#define DOOR_H_

#include "lib/enums.h"
#include "CommunicationHandler.h"
#include "TrafficLight.h"
#include "ValveRow.h"

struct savedDoor
{
	DoorState savedDoorState;
	bool topValveOpen;
	bool middleValveOpen;
	bool bottomValveOpen;
};

class Door
{
private:
	bool messageReceived;
	bool interruptCaught;
	CommunicationHandler* cHandler;
	DoorType type;
	DoorSide side;
	TrafficLight lightInside;
	TrafficLight lightOutside;
	
	
	void stopValves();
	void resetSavedState();

public:
	Door(CommunicationHandler* existingHandler, DoorType Type, DoorSide Side);
	~Door();
	
	void interruptReaction();
	int allowExit();
	int allowEntry();
	int openDoor();
	int closeDoor();
	int stopDoor();

	// TODO: should savedState be public?
	savedDoor savedState;
	ValveRow topValves;
	ValveRow middleValves;
	ValveRow bottomValves;
};

#endif