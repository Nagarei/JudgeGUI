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
	//@param bar_size_ �X�N���[���o�[�̃T�C�Y
	//@param object_size_ ���������̂̑傫��
	//@param display_area_size_ ����\������傫��
	//@param is_horizontal_ true�ŉ������o�[
	ScroolBar(int32_t bar_size_, int32_t object_size_, int32_t display_area_size_, bool is_horizontal_ = false) :ScroolBar() { set(bar_size_, object_size_, display_area_size_, is_horizontal_); }
	//@param bar_size �X�N���[���o�[�̃T�C�Y
	//@param object_size ���������̂̑傫��
	//@param display_area_size ����\������傫��
	//@param is_horizontal true�ŉ������o�[
	void set(int32_t bar_size, int32_t object_size, int32_t display_area_size, bool is_horizontal = false);

	struct keyboard_input_mask{
		static const uint32_t up        = 0b0001;
		static const uint32_t down      = 0b0010;
		static const uint32_t page_up   = 0b0100;
		static const uint32_t page_down = 0b1000;
	};
	//@param mouse_relative �o�[�̍�������_�Ƃ����}�E�X�̑��΍��W
	//@param mouse_left_input �}�E�X�́u�͂ށv�{�^���̓���
	void update(dxle::point_c<int32_t> mouse_relative, bool mouse_left_input, uint32_t keyboard_input);
	void draw()const;
	
	//�ǂꂾ�����炷�ׂ����擾
	int32_t get_value()const { return now_pos; }
private:
	int32_t bar_height;//�X�N���[���o�[���̂̕`��T�C�Y
	int32_t object_size;//���炵�����I�u�W�F�N�g��y�T�C�Y
	int32_t display_area_size;//�X�N���[���o�[�������������I�u�W�F�N�g�̕`��͈�
	DEBUG_NOTE;//���X�N���[���o�[��s�v�ȂƂ��ɏ������������������
	int32_t now_pos;//���炷�s�N�Z�� [0,display_area_size-object_size) �`����W�X�P�[��
	int32_t hold_pos_correction;//�}�E�X�ɒ͂܂�Ă���Ƃ� grip_y = mouse_y + hold_pos_correction �ɂȂ�l (old_grip_y-old_mouse_y) �o�[����W�X�P�[��
	bool is_horizontal;
	bool last_mouse_input;
	bool is_holded;//�}�E�X�ɒ͂܂�Ă��邩
	bool is_mouse_on_grip;//�}�E�X���o�[�̏�ɂ��邩
	static const int32_t bar_width = 17;
	static const int32_t arrow_size = 15;

	inline int32_t to_display_scale(int32_t bar_pos)const { return     bar_pos * object_size / bar_height; }//�o�[����W=>�`����W�̕ϊ�
	inline int32_t to_bar_scale(int32_t display_pos)const { return display_pos * bar_height / object_size; }//�`����W=>�o�[����W�̕ϊ�
};
