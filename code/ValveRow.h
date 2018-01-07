#ifndef VALVEROW_H_
#define VALVEROW_H_

#include "CommunicationHandler.h"
#include "lib/enums.h"

class ValveRow
{
public:
	ValveRow(CommunicationHandler& existingHandler, int Row, DoorSide Side);
	~ValveRow();

	bool openValveRow();
	bool closeValveRow();
	bool getValveRowOpened();

private:
	CommunicationHandler& cHandler;
	bool messageReceived;
	int row;
	DoorSide side;
};

#endif