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

struct ST_PROTO_HEADER
{
	E_PROTO_TYPE	eProtoType;
	DWORD			dwSizeOfStruct;

	ST_PROTO_HEADER() : eProtoType(E_PROTO_UNKNOWN), dwSizeOfStruct(0) {}
};

struct ST_PROTO_HEALTH_REQ
{
	bool bAlive;
};

struct ST_PROTO_HEALTH_RES
{
	bool bAlive;
};

struct ST_PROTO_ACTION_REQ
{
};

struct ST_PROTO_ACTION_RES
{
};


#endif 