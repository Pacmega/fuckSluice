// Destructor, copy constructor and assignment operator overloading is not
// needed as this class does not contain allocated memory

#include <iostream>

#include "Door.h"
#include "CommunicationHandler.h"

#include "lib/enums.h"
#include "lib/returnValues.h"

Door::Door(CommunicationHandler& existingHandler, DoorType Type, DoorSide Side)
	: cHandler(existingHandler)
	, lightInside(existingHandler, (Side==left) ? 2 : 3)
	, lightOutside(existingHandler, (Side==left) ? 1 : 4)
	, topValves(existingHandler, 3, Side)
	, middleValves(existingHandler, 2, Side)
	, bottomValves(existingHandler, 1, Side)
{
	interruptCaught = false;
	messageReceived = false;
	side = Side;
	type = Type;

	resetSavedState(); // The initial state is the same as the state after resetting.
}

Door::~Door()
{

}

void Door::interruptReaction()
{
	if (!interruptCaught)
	{
		// No interrupt was caught yet, OHSHITPANIC
		interruptCaught = true;
	}
	else
	{
		interruptCaught = false; // Set up for another emergency stop later on
	}
}

int Door::allowExit()
{
	LightState outsideLightState = lightOutside.getLightState();
	if (outsideLightState == greenLightOn)
	{
		if (lightOutside.redLight() != success)
		{
			return noAckReceived;
		}
	}
	else if (outsideLightState == lightError)
	{
		return invalidLightState;
	}

	DoorState currentState = cHandler.getDoorState(side);
	int rtnval;

	if (currentState == doorOpen)
	{
		return lightInside.greenLight();
	}
	else if (currentState == doorClosed || currentState == doorLocked)
	{
		rtnval = openDoor();
		switch (rtnval)
		// Because greenLight has its own return values, we need to be change
		// openDoor's return to distinguish it from greenLight's.
		{
			case success:
				// Door has been opened
				return lightInside.greenLight();
			default:
				return rtnval; // This shouldn't be possible.
		}
	}
	else
	{
		return incorrectDoorState; // Door is not in a state where a boat can be allowed to leave.
	}
}

int Door::allowEntry()
{
	LightState insideLightState = lightInside.getLightState();
	if (insideLightState == greenLightOn)
	{
		if (lightInside.redLight() != success)
		{
			return noAckReceived;
		}
	}
	else if (insideLightState == lightError)
	{
		return invalidLightState;
	}

	DoorState currentState = cHandler.getDoorState(side);
	int rtnval;

	if (currentState == doorOpen)
	{
		return lightOutside.greenLight();
	}
	else if (currentState == doorClosed || currentState == doorLocked || currentState == doorStopped)
	{
		rtnval = openDoor();
		switch (rtnval)
		{
			case success:
				// Door has been opened
				return lightOutside.greenLight();
			default:
				// Unable to open the door, return the error code
				return rtnval;
		}
	}
	else if (currentState == motorDamage)
	{
		return motorDamaged; // Unable to open door since it's broken
	}
	else
	{
		return incorrectDoorState; // Door is not in a state where a boat can be allowed to enter.
	}
}

int Door::openDoor()
{
	// We can assume the left door can be opened when waterLevel = low,
	// while the right door can only be opened when waterLevel = high.

	WaterLevel currentWLevel = cHandler.getWaterLevel();
	if (!((side == left && currentWLevel == low) || (side == right && currentWLevel == high)))
	{
		// The water is not at the right level to open the left door,
		// but the water also isn't at the right level to open the right door
		return incorrectWaterLevel; // Water level invalid for opening door
	}

	if (type == fastLock || cHandler.getDoorState(side) == doorLocked)
	{
		// Door is locked
		messageReceived = cHandler.unlockDoor(side);

		if (!messageReceived)
		{
			return noAckReceived; // Message was not acknowledged by the simulator
		}
		// Door is not locked
	}

	messageReceived = cHandler.openDoor(side);
	if (!messageReceived)
	{
		return noAckReceived; // Message was not acknowledged by the simulator
	}

	savedState.savedDoorState = doorOpening;
	DoorState currentState = cHandler.getDoorState(side);
	do
	{
		if (currentState == doorStopped)
		{
			messageReceived = cHandler.openDoor(side);
			
			if (!messageReceived)
			{
				return noAckReceived; // Message was not acknowledged by the simulator
			}
		}
		else if (currentState == motorDamage)
		{
			return motorDamaged;
		}
		currentState = cHandler.getDoorState(side);
	} while (!interruptCaught && currentState != doorOpen);

	if (currentState != doorOpen)
	{
		stopDoor();
		return interruptReceived; // An interrupt was received, door was not fully opened
	}
	else
	{
		return success; // Door opened
	}
}

int Door::closeDoor()
{
	// Check if any lights are green, they need to be turned red before closing the door.
	LightState currentLightState = lightInside.getLightState();
	if (currentLightState == greenLightOn)
	{
		lightInside.redLight();
	}
	else if (currentLightState == lightError)
	{
		return invalidLightState;
	}
	// If neither, the light was already red.

	currentLightState = lightOutside.getLightState();
	if (currentLightState == greenLightOn)
	{
		lightOutside.redLight();
	}
	else if (currentLightState == lightError)
	{
		return invalidLightState;
	}
	// If neither, the light was already red.

	// Check if any valves are open, can't close the door with open valves.
	if (topValves.getValveRowOpened())
	{
		topValves.closeValveRow();
	}
	if (topValves.getValveRowOpened())
	{
		middleValves.closeValveRow();
	}
	if (topValves.getValveRowOpened())
	{
		bottomValves.closeValveRow();
	}

	messageReceived = cHandler.closeDoor(side);
	if (!messageReceived)
	{
		return noAckReceived; // Message was not acknowledged by the simulator
	}

	savedState.savedDoorState = doorClosing;
	DoorState currentState = cHandler.getDoorState(side);
	do
	{
		if (currentState == doorStopped)
		{
			messageReceived = cHandler.closeDoor(side);
			
			if (!messageReceived)
			{
				return noAckReceived; // Message was not acknowledged by the simulator
			}
		}
		currentState = cHandler.getDoorState(side);
	} while (!interruptCaught && currentState != doorClosed);

	if (currentState != doorClosed)
	{
		stopDoor();
		return interruptReceived; // An interrupt was received
	}
	else
	{
		if (type == fastLock)
		{
			// Door should be locked
			messageReceived = cHandler.lockDoor(side);

			if (!messageReceived)
			{
				return noAckReceived; // Message was not acknowledged by the simulator
			}
			// Door is locked
		}
		
		return success; // Door closed
	}
}

int Door::stopDoor()
{
	if (interruptCaught)
	{
		DoorState currentState = cHandler.getDoorState(side);
		if (currentState == doorLocked || currentState == doorClosed)
		{
			// The door is closed, but valves may be open.
			savedState.savedDoorState = currentState;
			stopValves();
		}
		else if (currentState == doorOpening || currentState == doorClosing || currentState == doorStopped)
		{
			// The door is in the process of either opening or closing.
			// Don't save the current state, if it's stopped there is no way to know whether it was opening or closing.
			// Whether it was opening or closing was set when openDoor() or closeDoor() was called.
			cHandler.stopDoor(side);
		}
		else
		{
			// The door wasn't moving, but valves may have been open.
			// Just like stopDoor, stopValves doubles as both stop and restore function.
			stopValves();
		}
	}
	else
	{
		// Door was already stopped, restore status.
		if (savedState.savedDoorState == doorLocked || savedState.savedDoorState == doorClosed)
		{
			// The door was closed, but valves may have been open.
			// Just like stopDoor, stopValves doubles as both stop and restore function.
			stopValves();
		}
		else if (savedState.savedDoorState == doorOpening)
		{
			// The door was in the process of opening.
			openDoor();
		}
		else if (savedState.savedDoorState == doorClosing)
		{
			// The door was in the process of closing.
			closeDoor();
		}
		else
		{
			// These are door states where we didn't need to do anything to stop in the first place.
		}
	}
	cHandler.stopDoor(side);
	return noAckReceived;
}

void Door::stopValves()
{
	if (interruptCaught)
	{
		// Save valve states and close opened valves.
		savedState.topValveOpen = topValves.getValveRowOpened();
		savedState.middleValveOpen = middleValves.getValveRowOpened();
		savedState.bottomValveOpen = bottomValves.getValveRowOpened();

		if (savedState.topValveOpen)
		{
			topValves.closeValveRow();
		}
		if (savedState.middleValveOpen)
		{
			middleValves.closeValveRow();
		}
		if (savedState.bottomValveOpen)
		{
			bottomValves.closeValveRow();
		}
	}
	else
	{
		// Reopen valves that were open before the stop.

		if (savedState.topValveOpen)
		{
			topValves.openValveRow();
		}
		if (savedState.middleValveOpen)
		{
			middleValves.openValveRow();
		}
		if (savedState.bottomValveOpen)
		{
			bottomValves.openValveRow();
		}
	}
}

void Door::resetSavedState()
{
	savedState.savedDoorState = doorStateError;
	savedState.topValveOpen = false;
	savedState.middleValveOpen = false;
	savedState.bottomValveOpen = false;
}