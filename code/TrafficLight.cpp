#include <iostream>
#include "lib/enums.h"
#include "lib/returnValues.h"
#include "CommunicationHandler.h"
#include "TrafficLight.h"

TrafficLight::TrafficLight(CommunicationHandler& existingHandler, int Location)
    : cHandler(existingHandler)
{
    location = Location;
}

TrafficLight::~TrafficLight()
{

}

LightState TrafficLight::getLightState()
{
    // Calls the getLightState function from the Communication Handler,
    // passing it the location of this specific light.
    return cHandler.getLightState(location);
}

int TrafficLight::redLight()
{
    switch(getLightState())
    {
        case redLightOn:
            return success; // Done. Nothing to change.
        case greenLightOn:
            switch (cHandler.redLight(location))
            {
                case 0:
                    return success; // Success
                case -1:
                    return noAckReceived; // One of the messages was not received by the simulator
                case -2:
                    return invalidLightState; // Invalid light state received from simulator
                default:
                    return noAckReceived; // Message was not received by simulator
            }
        case lightError:
            return invalidLightState; // Invalid light state received from simulator
    }

    // This should be unreachable.
    return lightError;
}

int TrafficLight::greenLight()
{
    switch(getLightState())
    {
        case greenLightOn:
            // std::cout << "[DBG] GreenLight on " << std::endl;
            return success; // Done. Nothing to change.
        case redLightOn:
            // std::cout << "[DBG] GreenLight being turned on" << std::endl;
            switch (cHandler.greenLight(location))
            {
                case 0:
                    return success; // Success
                case -1:
                    return noAckReceived; // One of the messages was not received by the simulator
                case -2:
                    return invalidLightState; // Invalid light state received from simulator
                default:
                    return noAckReceived; // Message was not received by simulator
            }
            break;
        case lightError:
            return invalidLightState; // Invalid light state received from simulator
    }

    // This should be unreachable.
    return lightError;
}