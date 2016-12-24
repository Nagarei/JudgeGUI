#pragma once
#include "test.h"

struct Userdata_GUI final : CP_user_data_bace
{
	Userdata_GUI(){}
	Userdata_GUI(size_t problem_set_num_):problem_set_num(problem_set_num_){}
	size_t problem_set_num=0;
};
