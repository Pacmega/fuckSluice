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
	TrafficLight(const TrafficLight&);
	TrafficLight& operator= (const TrafficLight&);
	CommunicationHandler& cHandler;
	bool messageReceived;
	int location;
};

#endif