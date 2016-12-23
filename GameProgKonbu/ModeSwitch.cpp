#include "ModeSwitch.h"

#ifdef PARENT_MODE
#include "test.h"
#include "NetMain.h"

std::unique_ptr<Sequence> init_switch()
{
	return std::make_unique<NetInit>();
}

void update()
{
	//後で書く;
}
void update_after_maindraw()
{
	//nothing to do
}

#else

#include "popup.h"
#include "test.h"
#include "Mouse.h"
#include "Data.h"
#include "WaitJudge.h"
#include "MyTchar.h"
#include "fps.h"
#include "Option.h"

std::unique_ptr<Sequence> init_switch()
{
	frame_time::GetIns().upadte();
	frame_time::GetIns().upadte();
	return  std::make_unique<Option_Sequence>();
}
void update()
{
	KeyInputData::GetIns().Update();
	Mouse::GetIns().update();
	Data::GetIns().update();
	frame_time::GetIns().upadte();
	compile_taskmanager::process_test_result(
		[](compile_taskmanager::test_result_info&& res) {
			auto& data = Data::GetIns();
			if (data.get_problemset_num() != res.problem_set_num) {
				return;
			}
			auto& ns = res.submission_result;
			auto& prob_num = res.problem_num;
			auto type_draw = get_result_type_fordraw(ns);
			popup::set(_T("結果が出ました："_ts) + type_draw.first.data(), type_draw.second, dxle::color_tag::black, 3000);
			WaitJudgeQueue::pop(prob_num);
			data[prob_num].AddSubmission(std::move(ns));
		}
	);
}
void update_after_maindraw()
{
	popup::update();
}

#endif
