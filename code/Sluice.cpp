// Destructor, copy constructor and assignment operator overloading is not
// needed as this class does not contain allocated memory

#include "Sluice.h"
#include "CommunicationHandler.h"
#include <iostream>
#include "Door.h"
#include "lib/enums.h"
#include "lib/returnValues.h"

Sluice::Sluice(int port)
	: cHandler(port)
	, leftDoor(cHandler, (port==5557) ? fastLock : noLock, left)
	, rightDoor(cHandler, (port==5557) ? fastLock : noLock, right)
{

}

Sluice::~Sluice()
{

}

void Sluice::passInterrupt()
{
	leftDoor.interruptReaction();
	rightDoor.interruptReaction();

	if (!emergency)
	{
		// Emergency situation triggered
		emergency = true;
	}
	else
	{
		// Restore triggered
		switch(stateBeforeEmergency)
		{
			case sluicingUp:
				// Start itself handles the difference between up and down.
				start();
				break;
			case sluicingDown:
				// Start itself handles the difference between up and down.
				start();
				break;
			case allowingEntry:
				allowEntry();
				break;
			case allowingExit:
				allowExit();
				break;
			case waitingForCommand:
				// Do nothing
				break;
		}
		emergency = false;
	}
}

bool Sluice::closeValves(DoorSide side)
{
	if (side == left)
	{
		if(leftDoor.topValves.getValveRowOpened())
		{
			if (!leftDoor.topValves.closeValveRow())
			{
				return false; // Failed to close valves: simulator did not acknowledge message
			}
		}
		if(leftDoor.middleValves.getValveRowOpened())
		{
			if (!leftDoor.middleValves.closeValveRow())
			{
				return false; // Failed to close valves: simulator did not acknowledge message
			}
		}
		if(leftDoor.bottomValves.getValveRowOpened())
		{
			if (!leftDoor.bottomValves.closeValveRow())
			{
				return false; // Failed to close valves: simulator did not acknowledge message
			}
		}
	}
	else // side == right
	{
		if(rightDoor.topValves.getValveRowOpened())
		{
			if (!rightDoor.topValves.closeValveRow())
			{
				return false; // Failed to close valves: simulator did not acknowledge message
			}
		}
		if(rightDoor.middleValves.getValveRowOpened())
		{
			if (!rightDoor.middleValves.closeValveRow())
			{
				return false; // Failed to close valves: simulator did not acknowledge message
			}
		}
		if(rightDoor.bottomValves.getValveRowOpened())
		{
			if (!rightDoor.bottomValves.closeValveRow())
			{
				return false; // Failed to close valves: simulator did not acknowledge message
			}
		}
	}

	// If this point is reached, sluice succeeded in closing all valves where necessary.
	return true;
}

int Sluice::sluiceUp(WaterLevel currentWLevel)
{
	do
	{
		currentWLevel = cHandler.getWaterLevel();
		switch(currentWLevel)
		{
			case low:
				if(!rightDoor.bottomValves.getValveRowOpened())
				{
					if (!rightDoor.bottomValves.openValveRow())
					{
						return noAckReceived;
					}
				}
				break;
			case belowValve2:
				// Can't use one case for two possiblities, but these actually have the same consequences.
				if(!rightDoor.bottomValves.getValveRowOpened())
				{
					if (!rightDoor.bottomValves.openValveRow())
					{
						return noAckReceived;
					}
				}
				break;
			case aboveValve2:
				if(!rightDoor.middleValves.getValveRowOpened())
				{
					if (!rightDoor.middleValves.openValveRow())
					{
						return noAckReceived;
					}
				}
				break;
			case aboveValve3:
				if(!rightDoor.topValves.getValveRowOpened())
				{
					if (!rightDoor.topValves.openValveRow())
					{
						return noAckReceived;
					}
				}
				break;
			case high:
				// Nothing to do here, about to break out of the while loop.
				break;
			case waterError:
				// Can't go on with incorrect data.
				return incorrectWaterLevel;
		}
	} while (currentWLevel != high && !emergency);

	if (currentWLevel != high)
	{
		return interruptReceived;
	}
	else
	{
		// After finishing the process, close all valves.
		if (!closeValves(right))
		{
			return noAckReceived;
		}

		return success;
	}
}

int Sluice::sluiceDown(WaterLevel currentWLevel)
{
	leftDoor.bottomValves.openValveRow();

	do
	{
		currentWLevel = cHandler.getWaterLevel();
		if (currentWLevel == waterError)
		{
			// Can't go on with incorrect data.
			return incorrectWaterLevel;
		}
	} while (currentWLevel != low && !emergency);

	if (currentWLevel != low)
	{
		return interruptReceived;
	}
	else
	{
		// After finishing the process, close all valves.
		if (!closeValves(left))
		{
			return noAckReceived;
		}
		
		return success;
	}
}

int Sluice::start()
{
	WaterLevel currentWLevel = cHandler.getWaterLevel();
	if (!emergency)
	{
		int rtnval;

		switch(currentWLevel)
		{
			case low:
				stateBeforeEmergency = sluicingUp;

				if (cHandler.getDoorState(left) == doorOpen)
				{
					rtnval = leftDoor.closeDoor();
					if (rtnval != success)
					{
						// Don't continue if we can't close the door.
						return rtnval;
					}
				}
				if (cHandler.getDoorState(left) == doorClosed || cHandler.getDoorState(left) == doorLocked)
				{
					return sluiceUp(currentWLevel);
				}
				else
				{
					return incorrectDoorState; // Door is not in a state where sluicing can occur.
				}
				break;

			case high:
				stateBeforeEmergency = sluicingDown;
				if (cHandler.getDoorState(right) == doorOpen)
				{
					rtnval = rightDoor.closeDoor();
					if (rtnval != success)
					{
						// Don't continue if we can't close the door.
						return rtnval;
					}
				}

				if (cHandler.getDoorState(right) == doorClosed || cHandler.getDoorState(right) == doorLocked)
				{
					return sluiceDown(currentWLevel);
				}
				else
				{
					return incorrectDoorState; // Door is not in a state where sluicing can occur.
				}

				break;
			default:
				// Can't start moving a boat that can't possibly have entered.
				return invalidWaterLevel;
		}
	}
	else
	{
		// There was an emergency, which means there should be an old state to restore to.

		if (stateBeforeEmergency == sluicingUp)
		{
			return sluiceUp(currentWLevel);
		}
		else if (stateBeforeEmergency == sluicingDown)
		{
			return sluiceDown(currentWLevel);
		}
		else
		{
			// There was no state to restore to. Start may have been called while an emergency was going on.
			return invalidCall;
		}
	}

	return workInProgress;
}

int Sluice::allowEntry()
{
	WaterLevel currentWLevel = cHandler.getWaterLevel();
	if (currentWLevel == low)
	{
		stateBeforeEmergency = allowingEntry;
		return leftDoor.allowEntry();
	}
	else if (currentWLevel == high)
	{
		stateBeforeEmergency = allowingEntry;
		return rightDoor.allowEntry();
	}
	else
	{
		return incorrectWaterLevel;
	}
}

int Sluice::allowExit()
{
	WaterLevel currentWLevel = cHandler.getWaterLevel();
	if (currentWLevel == low)
	{
		stateBeforeEmergency = allowingExit;
		return leftDoor.allowExit();
	}
	else if (currentWLevel == high)
	{
		stateBeforeEmergency = allowingExit;
		return rightDoor.allowExit();
	}
	else
	{
		return incorrectWaterLevel;
	}
}