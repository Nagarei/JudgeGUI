#ifndef FPS_H_2016031915964120652
#define FPS_H_2016031915964120652

class frame_time
{
public:
	static frame_time& GetIns() { static frame_time obj; return obj; }
	int GetFrameTime()const DXLE_NOEXCEPT_OR_NOTHROW
	{
		return std::min(frame_time_raw, max_frame_time);
	}
	int myGetNowCount()const DXLE_NOEXCEPT_OR_NOTHROW
	{
		return now_time;
	}
	void upadte()DXLE_NOEXCEPT_OR_NOTHROW
	{
		int now_raw = DxLib::GetNowCount();
		frame_time_raw = (now_raw - last_count_raw) & INT_MAX;
		last_count_raw = now_raw;
		now_time += GetFrameTime();
	}
private:
	frame_time():last_count_raw(DxLib::GetNowCount()),now_time(DxLib::GetNowCount()), frame_time_raw(1){}
	static const int max_frame_time = 50;
	int last_count_raw;
	int now_time;
	int frame_time_raw;
};

inline int GetFrameTime() DXLE_NOEXCEPT_OR_NOTHROW
{
	return frame_time::GetIns().GetFrameTime();
}
inline int myGetNowCount() DXLE_NOEXCEPT_OR_NOTHROW
{
	return frame_time::GetIns().myGetNowCount();
}

#endif
