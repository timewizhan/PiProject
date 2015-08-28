#ifndef _PROTOCOL_
#define _PROTOCOL_

#include "Common.h"

enum E_PROTO_TYPE
{
	/*
		Health Type
	*/
	E_PROTO_HEALTH_REQ = 0,
	E_PROTO_HEALTH_RES,

	/*
		Action Type
	*/
	E_PROTO_ACTION_REQ,
	E_PROTO_ACTION_RES,

	E_PROTO_UNKNOWN
};

enum E_PROTO_CONTROL
{
	E_PROTO_CONTROL_UP = 1,
	E_PROTO_CONTROL_DOWN,
	E_PROTO_CONTROL_LEFT,
	E_PROTO_CONTROL_RIGHT,

	E_PROTO_CONTROL_UNKNOWN
};

struct ST_PROTO_HEADER
{
	E_PROTO_TYPE	eProtoType;
	DWORD			dwSizeOfStruct;

	ST_PROTO_HEADER() : eProtoType(E_PROTO_UNKNOWN), dwSizeOfStruct(0) {}
};

struct ST_PROTO_HEADER_RES
{
	bool bAck;

	ST_PROTO_HEADER_RES() : bAck(TRUE) {}
};

struct ST_PROTO
{
};

struct ST_PROTO_HEALTH_REQ : public ST_PROTO
{
	bool bAlive;

	ST_PROTO_HEALTH_REQ() : bAlive(TRUE) {}
};

struct ST_PROTO_HEALTH_RES : public ST_PROTO
{
	bool bAck;

	ST_PROTO_HEALTH_RES() : bAck(TRUE) {}
};

struct ST_PROTO_ACTION_REQ : public ST_PROTO
{
	E_PROTO_CONTROL	eProtoControl;

	ST_PROTO_ACTION_REQ() : eProtoControl(E_PROTO_CONTROL_UNKNOWN) {}
};

struct ST_PROTO_ACTION_RES : public ST_PROTO
{
	bool bAck;

	ST_PROTO_ACTION_RES() : bAck(TRUE) {}
};


#endif 