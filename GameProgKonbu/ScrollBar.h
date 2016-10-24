#pragma once
#include <cstdint>

#if !defined(__clang__) && 1
static_assert(false, "");
using int32_t = int;
using uint32_t = unsigned;
#endif
class ScroolBar
{
public:
	ScroolBar();
	//@param object_size_ ���������̂̑傫��
	//@param page_size_ ����\������傫��(�o�[�܂�)
	//@param is_horizontal_ true�ŉ������o�[
	ScroolBar(int32_t object_size_, int32_t page_size_, bool is_horizontal_ = false) :ScroolBar() { set(object_size_, page_size_, is_horizontal_); }
	//@param object_size_ ���������̂̑傫��
	//@param page_size_ ����\������傫��(�o�[�܂�)
	//@param is_horizontal_ true�ŉ������o�[
	void set(int32_t object_size_, int32_t page_size_, bool is_horizontal_ = false);

	struct keyboard_input_mask{
		static const uint32_t up        = 0b0001;
		static const uint32_t down      = 0b0010;
		static const uint32_t page_up   = 0b0100;
		static const uint32_t page_down = 0b1000;
	};
	//@param bar_size �o�[�̕`��y(���̎���x)�T�C�Y
	//@param mouse_relative �o�[�̍�������_�Ƃ����}�E�X�̑��΍��W
	//@param wheel �}�E�X�̃z�C�[���̉�]��
	//@param mouse_left_input �}�E�X�́u�͂ށv�{�^���̓���
	//@param keyboard_input keyboard_input_mask�ō\�����ꂽ�L�[�̓��͏�
	//@param arrow_value ���{�^���������ꂽ�Ƃ��̈ړ���
	void update(uint32_t bar_size, dxle::point_c<int32_t> mouse_relative, int32_t wheel, bool mouse_left_input, uint32_t keyboard_input, uint32_t arrow_value = 2);
	void draw(dxle::pointi32 bar_pos, uint32_t bar_size)const;
	
	//�ǂꂾ�����炷�ׂ����擾
	int32_t get_value()const { return now_pos; }
private:
	int32_t object_size;//���炵�����I�u�W�F�N�g��y�T�C�Y(pix)
	int32_t page_size;//�X�N���[���o�[�������������I�u�W�F�N�g�̕`��͈�(pix)
	DEBUG_NOTE;//���X�N���[���o�[��s�v�ȂƂ��ɏ������������������
	int32_t now_pos;//���炷�s�N�Z�� [0,object_size-page_size) (pix)
	int32_t grip_start_mousepos;//�O���b�v���n�܂����Ƃ��̃}�E�X�̈ʒu
	int32_t grip_start_nowpos;//�O���b�v���n�܂����Ƃ���now_pos
	bool is_horizontal;
	bool last_mouse_input;
	bool is_holded;//�}�E�X�ɒ͂܂�Ă��邩
	enum class mouse_pos{up_arrow, up_space, grip, down_space, down_arrow, out}on_mouse_pos;
	static const int32_t bar_width = 17;
	static const int32_t arrow_size = 15;
};
