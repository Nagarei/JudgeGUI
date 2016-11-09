#pragma once


namespace Script
{
	class Script {
	private:
		std::array<dxle::sizeui32, 3> line_size;//�n�߂̍s�A�^�񒆂̍s(������)�A�Ō�̍s
		unsigned line_num = 0;//[0,3](3�����3�Ɋۂ߂�)
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
		//�n�߂̍s�A�^�񒆂̍s(������)�A�Ō�̍s
		const auto& get_line_size()const noexcept { return line_size; }
		//[0,3]
		const auto& get_line_nums()const noexcept { return line_num; }
		virtual void update(){}
		virtual void draw() = 0;
		virtual ~Script()noexcept {}
	};

	//@���Ȃ� or �N���󂯎��Ȃ������ꍇ�̃f�t�H���g����
	class Text final : public Script
	{
	public:
		Text(dxle::tstring& str);

		static std::unique_ptr<Script> get_script(dxle::tstring& str);
	};

	std::unique_ptr<Script> build_script(dxle::tstring& str);
}
