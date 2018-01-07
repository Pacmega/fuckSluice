#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

#include "CommunicationHandler.h"
#include "lib/enums.h"

class TrafficLight
{
public:
	TrafficLight(CommunicationHandler& existingHandler, int location);
	~TrafficLight();

	LightState getLightState();
	int redLight();
	int greenLight();

private:
	CommunicationHandler& cHandler;
	bool messageReceived;
	int location;
};

#endif