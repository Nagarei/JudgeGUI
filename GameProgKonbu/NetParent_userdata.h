#pragma once
#include "test.h"

struct Userdata_NetParent final : CP_user_data_bace
{
	Userdata_NetParent() {}
	Userdata_NetParent(int nethandle_) :nethandle(nethandle_) {}
	int nethandle = 0;
};
