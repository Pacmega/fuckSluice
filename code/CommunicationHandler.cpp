// Destructor, copy constructor and assignment operator overloading is not
// needed as this class does not contain allocated memory

#include <iostream>
#include <string.h>

#include "CommunicationHandler.h"
#include "lib/commands.h"
#include "lib/enums.h"
#include "lib/returnValues.h"

CommunicationHandler::CommunicationHandler(int socket)
	: simulation(SimulationCommunicator(socket))
{
	
}

CommunicationHandler::~CommunicationHandler()
{

}

DoorState CommunicationHandler::getDoorState(DoorSide side)
{
	DoorState dState = doorStateError;
	const char* messageToSend;

	if (side == left)
	{
		messageToSend = GetDoorLeft;
	}
	else // side == right
	{
		messageToSend = GetDoorRight;
	}

	receivedMessage = simulation.sendMessage(messageToSend);
	
	// Switch cases aren't possible for strings sadly.
	if (strcmp(receivedMessage, "doorLocked") == 0)
	{
		dState = doorLocked;
	}
	else if (strcmp(receivedMessage, "doorClosed") == 0)
	{
		dState = doorClosed;
	}
	else if (strcmp(receivedMessage, "doorOpen") == 0)
	{
		dState = doorOpen;
	}
	else if (strcmp(receivedMessage, "doorClosing") == 0)
	{
		dState = doorClosing;
	}
	else if (strcmp(receivedMessage, "doorOpening") == 0)
	{
		dState = doorOpening;
	}
	else if (strcmp(receivedMessage, "doorStopped") == 0)
	{
		dState = doorStopped;
	}
	else if (strcmp(receivedMessage, "motorDamage") == 0)
	{
		dState = motorDamage;
	}

	return dState;
}

bool CommunicationHandler::lockDoor(DoorSide side)
{
	if (side == left)
	{
		receivedMessage = simulation.sendMessage(DoorLeftLock);
	}
	else // side == right
	{
		receivedMessage = simulation.sendMessage(DoorRightLock);
	}

	if (strcmp(receivedMessage, "ack") != 0)
	{
		return false; // Message was not acknowledged by the simulator or door could not be locked.
	}
	else
	{
		return true;
	}
}

bool CommunicationHandler::unlockDoor(DoorSide side)
{
	if (side == left)
	{
		receivedMessage = simulation.sendMessage(DoorLeftUnlock);
	}
	else // side == right
	{
		receivedMessage = simulation.sendMessage(DoorRightUnlock);
	}

	if (strcmp(receivedMessage, "ack") != 0)
	{
		return false; // Message was not acknowledged by the simulator
	}
	else
	{
		return true;
	}
}

bool CommunicationHandler::openDoor(DoorSide side)
{
	if (side == left)
	{
		receivedMessage = simulation.sendMessage(DoorLeftOpen);
	}
	else // side == right
	{
		receivedMessage = simulation.sendMessage(DoorRightOpen);
	}

	if (strcmp(receivedMessage, "ack") == 0)
	{
		return true; // Door was told to open.
	}
	else
	{
		return false; // Message was not acknowledged by the simulator
	}
}

bool CommunicationHandler::closeDoor(DoorSide side)
{
	// Door should deal with locking itself.

	if (side == left)
	{
		receivedMessage = simulation.sendMessage(DoorLeftClose);
	}
	else // side == right
	{
		receivedMessage = simulation.sendMessage(DoorRightClose);
	}

	if (strcmp(receivedMessage, "ack") == 0)
	{
		return true; // Door was told to close.
	}
	else
	{
		return false; // Message was not acknowledged by the simulator
	}
}

bool CommunicationHandler::stopDoor(DoorSide side)
{
	if (side == left)
	{
		receivedMessage = simulation.sendMessage(DoorLeftStop);
	}
	else // side == right
	{
		receivedMessage = simulation.sendMessage(DoorRightStop);
	}

	if (strcmp(receivedMessage, "ack") == 0)
	{
		return success; // Successfully stopped
	}
	else
	{
		return noAckReceived; // Message was not acknowledged by simulator
	}
}

bool CommunicationHandler::getValveOpened(DoorSide side, int row)
{
	bool opened = false;
	const char* messageToSend;

	if (side == left)
	{
		switch (row)
		{
			case 1:
				messageToSend = DoorLeftGetBottomValve;
				break;
			case 2:
				messageToSend = DoorLeftGetMiddleValve;
				break;
			case 3:
				messageToSend = DoorLeftGetTopValve;
				break;
		}
	}
	else // side == right
	{
		switch (row)
		{
			case 1:
				messageToSend = DoorRightGetBottomValve;
				break;
			case 2:
				messageToSend = DoorRightGetMiddleValve;
				break;
			case 3:
				messageToSend = DoorRightGetTopValve;
				break;
		}
	}

	receivedMessage = simulation.sendMessage(messageToSend);

	if (strcmp(receivedMessage, "open") == 0)
	{
		opened = true;
	}

	return opened;
}

bool CommunicationHandler::valveOpen(DoorSide side, int row)
{
	// Valves don't break when opened while already open, so no need to check.
	
	if (row >= 1 && row <= 3)
	{
		switch(side)
		{
			case left:
				switch(row)
				{
					case 1:
						receivedMessage = simulation.sendMessage(DoorLeftOpenBottomValve);
						break;
					case 2:
						receivedMessage = simulation.sendMessage(DoorLeftOpenMiddleValve);
						break;
					case 3:
						receivedMessage = simulation.sendMessage(DoorLeftOpenTopValve);
						break;
				}
				break;
			case right:
				switch(row)
				{
					case 1:
						receivedMessage = simulation.sendMessage(DoorRightOpenBottomValve);
						break;
					case 2:
						receivedMessage = simulation.sendMessage(DoorRightOpenMiddleValve);
						break;
					case 3:
						receivedMessage = simulation.sendMessage(DoorRightOpenTopValve);
						break;
				}
				break;
		}

		if (strcmp(receivedMessage, "ack") == 0)
		{
			return true; // Valve opened.
		}
	}
	
	return false;
}

bool CommunicationHandler::valveClose(DoorSide side, int row)
{
	
	// Valves don't break when closed while already closed, so no need to check.
	
	if (row >= 1 && row <= 3)
	{
		switch(side)
		{
			case left:
				switch(row)
				{
					case 1:
						receivedMessage = simulation.sendMessage(DoorLeftCloseBottomValve);
						break;
					case 2:
						receivedMessage = simulation.sendMessage(DoorLeftCloseMiddleValve);
						break;
					case 3:
						receivedMessage = simulation.sendMessage(DoorLeftCloseTopValve);
						break;
				}
				break;
			case right:
				switch(row)
				{
					case 1:
						receivedMessage = simulation.sendMessage(DoorRightCloseBottomValve);
						break;
					case 2:
						receivedMessage = simulation.sendMessage(DoorRightCloseMiddleValve);
						break;
					case 3:
						receivedMessage = simulation.sendMessage(DoorRightCloseTopValve);
						break;
				}
				break;
		}

		if (strcmp(receivedMessage, "ack") == 0)
		{
			return true; // Valve closed.
		}
	}
	
	return false;
}

int CommunicationHandler::redLight(int lightLocation)
{
	if (lightLocation >= 1 && lightLocation <= 4)
	{
		const char* message1ToSend;
		const char* message2ToSend;

		switch(lightLocation)
		{
			case 1:
				message1ToSend = TrafficLight1GreenOff;
				message2ToSend = TrafficLight1RedOn;
				break;
			case 2:
				message1ToSend = TrafficLight2GreenOff;
				message2ToSend = TrafficLight2RedOn;
				break;
			case 3:
				message1ToSend = TrafficLight3GreenOff;
				message2ToSend = TrafficLight3RedOn;
				break;
			case 4:
				message1ToSend = TrafficLight4GreenOff;
				message2ToSend = TrafficLight4RedOn;
				break;
		}

		receivedMessage = simulation.sendMessage(message1ToSend);
	
		if (strcmp(receivedMessage, "ack") == 0)
		{
			receivedMessage = simulation.sendMessage(message2ToSend);
			if (strcmp(receivedMessage, "ack") == 0)
			{
				return success; // Success
			}
			else
			{
				return noAckReceived; // Only one of the lights was changed
			}
		}
		else
		{
			return noAckReceived; // None of the lights were changed.
		}
	}
	else
	{
		return invalidLightLocation; // Invalid lightLocation was passed
	}
}

int CommunicationHandler::greenLight(int lightLocation)
{
	if (lightLocation >= 1 && lightLocation <= 4)
	{
		const char* message1ToSend;
		const char* message2ToSend;

		switch(lightLocation)
		{
			case 1:
				message1ToSend = TrafficLight1RedOff;
				message2ToSend = TrafficLight1GreenOn;
				break;
			case 2:
				message1ToSend = TrafficLight2RedOff;
				message2ToSend = TrafficLight2GreenOn;
				break;
			case 3:
				message1ToSend = TrafficLight3RedOff;
				message2ToSend = TrafficLight3GreenOn;
				break;
			case 4:
				message1ToSend = TrafficLight4RedOff;
				message2ToSend = TrafficLight4GreenOn;
				break;
		}

		receivedMessage = simulation.sendMessage(message1ToSend);
	
		if (strcmp(receivedMessage, "ack") == 0)
		{
			receivedMessage = simulation.sendMessage(message2ToSend);
			if (strcmp(receivedMessage, "ack") == 0)
			{
				return success; // Success
			}
		}
	}
	else
	{
		return invalidLightLocation; // Invalid lightLocation was passed
	}

	return noAckReceived; // One of the messages was not received by the simulator
}

LightState CommunicationHandler::getLightState(int lightLocation)
{
	LightState lState = lightError;

	if (lightLocation >= 1 && lightLocation <= 4)
	{
		const char* redLightMessage;
		const char* greenLightMessage;

		switch(lightLocation)
		{
			case 1:
				redLightMessage = TrafficLight1RedStatus;
				greenLightMessage = TrafficLight1GreenStatus;
				break;
			case 2:
				redLightMessage = TrafficLight2RedStatus;
				greenLightMessage = TrafficLight2GreenStatus;
				break;
			case 3:
				redLightMessage = TrafficLight3RedStatus;
				greenLightMessage = TrafficLight3GreenStatus;
				break;
			case 4:
				redLightMessage = TrafficLight4RedStatus;
				greenLightMessage = TrafficLight4GreenStatus;
				break;	
		}

		std::string redLightReceived = simulation.sendMessage(redLightMessage);
		std::string greenLightReceived = simulation.sendMessage(greenLightMessage);

		if (redLightReceived == "on" && greenLightReceived == "off")
		{
			lState = redLightOn;
		}
		else if (redLightReceived == "off" && greenLightReceived == "on")
		{
			lState = greenLightOn;
		}
	}

	return lState;
}

WaterLevel CommunicationHandler::getWaterLevel()
{
	WaterLevel wLevel = waterError;
	receivedMessage = simulation.sendMessage(GetWaterLevel);

	// Switch cases aren't possible for strings sadly.
	if (strcmp(receivedMessage, "low") == 0)
	{
		wLevel = low;
	}
	else if (strcmp(receivedMessage, "belowValve2") == 0)
	{
		wLevel = belowValve2;
	}
	else if (strcmp(receivedMessage, "aboveValve2") == 0)
	{
		wLevel = aboveValve2;
	}
	else if (strcmp(receivedMessage, "aboveValve3") == 0)
	{
		wLevel = aboveValve3;
	}
	else if (strcmp(receivedMessage, "high") == 0)
	{
		wLevel = high;
	}

	return wLevel;
}