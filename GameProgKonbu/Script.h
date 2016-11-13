#pragma once

//@h[text]//見出しのように大きく、太く表示します
//@[text]//太く表示します
//@@ //@を表示します

namespace Script
{
	class Script {
	private:
		std::array<dxle::sizeui32, 3> line_size;//始めの行、真ん中の行(複数可)、最後の行
		unsigned line_num = 0;//[0,3](3より上は3に丸める)
	protected:
		void SetLineSize_first(const dxle::sizeui32& line_size_) {
			assert(line_num == 0);
			line_size[0] = line_size_;
			line_num = std::max(line_num, 1u);
		}
		void SetLineSize_last(const dxle::sizeui32& line_size_) {
			assert(line_num == 1);
			line_size[2] = line_size_;
			line_num = std::max(line_num, 2u);
		}
		void SetLineSize_middle(const dxle::sizeui32& line_size_) {
			assert(line_num == 2);
			line_size[1] = line_size_;
			line_num = std::max(line_num, 3u);
		}
	public:
		//始めの行、真ん中の行(複数可)、最後の行
		const auto& get_line_size()const noexcept { return line_size; }
		//[0,3]
		const auto& get_line_nums()const noexcept { return line_num; }
		virtual void draw_extend(unsigned draw_line, const dxle::pointi32& pos, double extend_rate)const = 0;
		virtual ~Script()noexcept {}
	};

	//@image<width hight>[image-name] //画像を表示します(サイズを省略した場合デフォルトになります)
	class Image final : public Script
	{
	private:
		dxle::texture2d graph;
		dxle::sizeui32 size;
	public:
		Image(dxle::tstring& str);

		void draw_extend(unsigned draw_line, const dxle::pointi32& pos, double extend_rate)const override;
		static std::unique_ptr<Script> get_script(dxle::tstring& str);
	};
	//@がない or 誰も受け取らなかった場合のデフォルト動作
	class Plain_Text final : public Script
	{
	private:
		std::array<dxle::tstring, 3> line_str;
		static int font;//作りっぱなし
	public:
		static constexpr int32_t font_height = 22;
		Plain_Text(dxle::tstring& str);

		void draw_extend(unsigned draw_line, const dxle::pointi32& pos, double extend_rate)const override;
		static std::unique_ptr<Script> get_script(dxle::tstring& str);
	};

	std::unique_ptr<Script> build_script(dxle::tstring& str);

	constexpr int32_t linestart_space = 2;
	//スクリプトの大きさ分next_start_posを進める
	void advance_script(const std::unique_ptr<Script::Script>& script, dxle::pointui32& next_start_pos, uint32_t& next_line_start,
		double extend_rate, uint32_t* max_width_buf, const dxle::pointi32* draw_pos1/*nullptrで描画しない*/);
}
