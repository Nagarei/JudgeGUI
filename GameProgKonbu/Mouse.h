#pragma once

class Mouse final
{
public:
	struct click_data {
		dxle::pointi pos;
		int type;
	};
	struct drag_data {
		dxle::pointi start;
		dxle::sizei value;
		bool is_drag = false;
	};
private:
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	Mouse() = default;

	dxle::pointi now_pos;
	int32_t now_wheel;
	int32_t now_H_wheel;
	int32_t now_input;
	std::deque<click_data> click_point;//front: pop back:push
	std::unordered_map<int, drag_data> drag_point;//key: 入力タイプ　first ドラッグ開始位置 second　ドラッグ量

	void erase_drag(int input);
	decltype(drag_point)::iterator erase_drag(decltype(drag_point)::const_iterator iter);
public:
	static Mouse& GetIns() {
		static Mouse ins;
		return ins;
	}
	void update();

	int32_t get_now_wheel()const { return now_wheel; }
	int32_t get_now_H_wheel()const { return now_H_wheel; }//!< 水平ホイール
	dxle::pointi get_now_pos()const { return now_pos; }
	int32_t get_now_input()const { return now_input; }

	auto click_log_is_empty()const { return click_point.empty(); }
	auto click_log_front()const { return click_point.front(); }
	auto click_log_pop() { return click_point.pop_front(); }
	auto click_log_clear() { return click_point.clear(); }

	auto drag_end()const { return drag_point.end(); }
	//@param is_value_reset valueとstartを0にリセットするか
	//@return first is_valid
	std::pair<bool, drag_data> get_drag_iter(int type, bool is_value_reset = false) {
		auto iter = drag_point.find(type);
		if (iter == drag_point.end()) {
			return{ false, drag_data{} };
		}
		if (is_value_reset) {
			auto result = std::make_pair(true, iter->second);
			iter->second.start += iter->second.value;
			iter->second.value.width = iter->second.value.height = 0;
			return result;
		}
		else {
			return{ true, iter->second };
		}
	}
	//@return first is_valid
	std::pair<bool, drag_data> get_drag_iter(int type)const {
		auto iter = drag_point.find(type);
		if (iter == drag_point.end()) {
			return{ false, drag_data{} };
		}
		return{ true, iter->second };
	}
};