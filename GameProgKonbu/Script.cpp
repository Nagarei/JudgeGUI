#ifndef PARENT_MODE
#include "Script.h"
#include "my_utility.h"
#include "Data.h"

namespace
{
	//@return true：処理した false:スルーした
	bool receve_script(dxle::tstring& bace_str, const TCHAR* accept_script) {
		auto bace_str_iter = bace_str.begin(), bace_str_end = bace_str.end();
		for (; *accept_script != _T('\0') && bace_str_iter != bace_str_end; ++accept_script, ++bace_str_iter) {
			if (*accept_script != *bace_str_iter) {
				return false;
			}
		}
		if (*accept_script == _T('\0')) {
			bace_str.erase(bace_str.begin(), bace_str_iter);
			return true;
		}
		else {
			return false;
		}
	}
	template<typename INT = int64_t>
	std::pair<INT, dxle::tstring::iterator> my_stoi(dxle::tstring::iterator iter, dxle::tstring::iterator iter_end)
	{
		INT value = 0;
		if (iter == iter_end) { return{ value,iter }; }
		bool minus_flag = false;
		if (*iter == _T('-')) {
			minus_flag = true;
			++iter;
		}
		while (iter != iter_end && _T('0') <= *iter && *iter <= _T('9'))
		{
			assert(value <= std::numeric_limits<INT>::max() / 10 - 1);
			value *= 10;
			value += (*iter - _T('0'));
			++iter;
		}
		if (minus_flag) {
			value = -value;
		}
		return{ value,iter };
	}
}

namespace Script
{
	namespace impl
	{
		//---------------------------impl::Text_Bace------------------------------------------------//

		void Text_Bace::Init(dxle::tstring& str, int font)
		{
			FINALLY([&str]() {
				//すべて処理する
				str.clear();
			});

			//文字サイズ取得
			auto get_str_size = [font](const dxle::tstring& str_) {
				dxle::sizei str_size;
				DxLib::GetDrawStringSizeToHandle(&str_size.width, &str_size.height, nullptr,
					str_.c_str(), str_.size(), font);//@todo dxlibex
				return static_cast<dxle::sizeui32>(str_size);
			};
			//一行目
			auto first_eiter = std::find(str.begin(), str.end(), _T('\n'));
			line_str[0].assign(str.begin(), first_eiter);
			SetLineSize_first(get_str_size(line_str[0]));
			//末端行
			if (first_eiter == str.end()) { return; }
			auto last_siter = str.end() - 1;
			for (; *last_siter != _T('\n'); --last_siter) {}
			++last_siter;
			line_str[2].assign(last_siter, str.end());
			SetLineSize_last(get_str_size(line_str[2]));
			//中間部分
			if (first_eiter == last_siter - 1) { return; }
			line_str[1].assign(first_eiter + 1, last_siter - 1);
			auto temp_size = get_str_size(line_str[1]);
			SetLineSize_middle(temp_size);
		}
		void Text_Bace::draw_extend_impl(unsigned draw_line, const dxle::pointi32 & pos, double extend_rate, int font, dxle::dx_color_param color)const
		{
			DrawExtendStringToHandle(pos.x, pos.y, extend_rate, extend_rate,
				line_str[draw_line].c_str(), color.get(), font);
		}
	}//namespace impl

//---------------------------Plain_Text------------------------------------------------//

	int Plain_Text::font = -1;
	Plain_Text::Plain_Text(dxle::tstring& str)
	{
		if (font == -1) {
			font = DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 20, 1);
		}
		//すべて処理する
		Init(str, font);
	}
	void Plain_Text::draw_extend(unsigned draw_line, const dxle::pointi32 & pos, double extend_rate)const
	{
		draw_extend_impl(draw_line, pos, extend_rate, font, dxle::color_tag::black);
	}
	std::unique_ptr<Script> Plain_Text::get_script(dxle::tstring& str)
	{
		//常に作る
		if (!str.empty() && str.front() == _T('@')) {
			str.erase(str.begin());//拾われなかった@は無視
		}
		return std::make_unique<Plain_Text>(str);
	}

	//---------------------------Image------------------------------------------------//

		//@image<width hight>[image-name]
	Image::Image(dxle::tstring & str)
	{
		if (str.empty()) { return; }
		auto iter = str.begin();
		FINALLY([&] { str.erase(str.begin(), iter); });
		if (*iter == _T('<')) {
			++iter;
			//サイズ取得
			auto stoi_res = my_stoi<uint32_t>(iter, str.end());
			size.width = stoi_res.first;
			iter = std::find_if(stoi_res.second, str.end(), [](TCHAR c) {
				return (_T('0') <= c && c <= _T('9')) || (c == _T('>'));
			});
			if (iter == str.end()) { return; }
			if (*iter == _T('>')) {
				//heightは無し
			}
			else {
				stoi_res = my_stoi<uint32_t>(iter, str.end());
				size.height = stoi_res.first;
				iter = std::find_if(stoi_res.second, str.end(), [](TCHAR c) {
					return c == _T('>');
				});
			}
			if (iter == str.end()) { return; }
			++iter;//'>'まで読み込んだ
		}
		if (iter == str.end()) { return; }
		if (*iter == _T('[')) {
			++iter;
			auto path_end = std::find_if(iter, str.end(), [](TCHAR c) {
				return (c == _T(']'));
			});
			dxle::tstring file_path{ iter, path_end };
			if (PathIsRelative(file_path.c_str())) {
				//相対パスなので、問題フォルダからの相対パスに
				file_path.insert(0, Data::GetIns().GetLoadingProblemDir());
			}
			graph = dxle::LoadGraph(file_path);
			iter = path_end; if (path_end != str.end()) { ++iter; }

			if (graph.valid())
			{
				auto gr_size = graph.GetGraphSize();
				if (size.height == 0) { size.height = gr_size.height; }
				if (size.width == 0) { size.width = gr_size.width; }
				SetLineSize_first(size);
			}
		}
	}

	void Image::draw_extend(unsigned draw_line, const dxle::pointi32 & pos, double extend_rate) const
	{
		assert(draw_line == 0);
		graph.DrawExtendGraph(pos, dxle::pointi(pos + size*extend_rate), false);
	}

	std::unique_ptr<Script> Image::get_script(dxle::tstring & str)
	{
		if (receve_script(str, _T("@image"))) {
			return std::make_unique<Image>(str);
		}
		return nullptr;
	}


	//---------------------------build_script------------------------------------------------//

	std::unique_ptr<Script> build_script(dxle::tstring& str)
	{
#ifdef _DEBUG
		auto bef_str = str;
		FINALLY([&]() {
			assert(bef_str != str);
});
#endif
#define RUN_bs(CLASS) if(auto res = CLASS::get_script(str)){return res;}
		RUN_bs(Image)
#undef  RUN_bs
			return Plain_Text::get_script(str);
	}

}//namespace Script

//---------------------------advance_script------------------------------------------------//
namespace {
	//改行&復帰
	inline void new_line(uint32_t& next_line_start, dxle::pointui32& next_start_pos, double extend_rate) {
		//改行
		if (next_line_start <= next_start_pos.y) {
			//デフォルトの行間を設定
			next_line_start = static_cast<uint32_t>(next_start_pos.y + Script::Plain_Text::font_height * extend_rate);
		}
		next_start_pos.y = next_line_start;
		//復帰
		next_start_pos.x = Script::linestart_space;
	}
	//記述
	inline void add_to_thisline(uint32_t& next_line_start, dxle::pointui32& next_start_pos, double extend_rate, const dxle::sizeui32& size) {
		//今の行の更新
		next_start_pos.x += static_cast<int32_t>(size.width * extend_rate);
		next_line_start = std::max<uint32_t>(next_line_start, next_start_pos.y + (size.height * extend_rate));
	}
}
namespace Script {
	//スクリプトの大きさ分next_start_posを進める
	void advance_script(const std::unique_ptr<Script>& script, dxle::pointui32& next_start_pos, uint32_t& next_line_start, double extend_rate, uint32_t* max_width_buf, const dxle::pointi32* draw_pos1)
	{
		assert(0 < extend_rate);
		//大きさチェック
		uint32_t max_width = 0;
		//改行&復帰
		auto new_line_ = [&next_line_start, &next_start_pos, extend_rate, &max_width]() {
			max_width = std::max(max_width, next_start_pos.x);
			new_line(next_line_start, next_start_pos, extend_rate);
		};
		//記述
		auto add_to_thisline_ = [&next_line_start, &next_start_pos, extend_rate](const dxle::sizeui32& size) {
			add_to_thisline(next_line_start, next_start_pos, extend_rate, size);
		};
		const auto& line_num = script->get_line_nums();
		const auto& line_size = script->get_line_size();
		if (1 <= line_num) {
			//今の行の更新
			if (draw_pos1) {
				script->draw_extend(0, *draw_pos1 + static_cast<dxle::pointi32>(next_start_pos), extend_rate);
			}
			add_to_thisline_(line_size[0]);
		}
		if (3 <= line_num) {
			//改行&復帰
			new_line_();

			if (draw_pos1) {
				script->draw_extend(1, *draw_pos1 + static_cast<dxle::pointi32>(next_start_pos), extend_rate);
			}

			//中央の塊の更新
			add_to_thisline_(line_size[1]);
		}
		if (2 <= line_num) {
			//改行&復帰
			new_line_();

			if (draw_pos1) {
				script->draw_extend(2, *draw_pos1 + static_cast<dxle::pointi32>(next_start_pos), extend_rate);
			}

			//末端の更新
			add_to_thisline_(line_size[2]);
		}

		if (max_width_buf) {
			*max_width_buf = max_width;
		}
	}
}//namespace Script

#endif // !1
