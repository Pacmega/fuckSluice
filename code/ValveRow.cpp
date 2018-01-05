#include "ValveRow.h"
#include "CommunicationHandler.h"
#include "lib/enums.h"

ValveRow::ValveRow(CommunicationHandler& existingHandler, int Row, DoorSide Side)
	: cHandler(existingHandler)
{
	row = Row;
	side = Side;
}

ValveRow::~ValveRow()
{

}

bool ValveRow::openValveRow()
{
	return cHandler.valveOpen(side, row);
}

bool ValveRow::closeValveRow()
{
	return cHandler.valveClose(side, row);
}

bool ValveRow::getValveRowOpened()
{
	return cHandler.getValveOpened(side, row);
}