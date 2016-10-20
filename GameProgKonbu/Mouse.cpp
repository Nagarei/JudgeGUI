#include "Mouse.h"

void Mouse::update()
{
	//���ݏ��̎擾
	DxLib::GetMousePoint(&now_pos.x, &now_pos.y);
	int now_input = DxLib::GetMouseInput();//���O�̌�ɃL���[�ɐςނ��ߕۗ�

	//���O�̃`�F�b�N
	{int log_input;
	for (dxle::pointi log_pos; DxLib::GetMouseInputLog(&log_input, &log_pos.x, &log_pos.y) == 0;)
	{
		if (log_input & ~now_input)
		{
			//��������Ă��Ȃ����A�O�ɉ�����Ă���
			//=>�N���b�N�Ƃ��ď���
			click_point.emplace_back(click_data{ log_pos, log_input });
		}
		else
		{
			//���������Ă���
			//=>�h���b�O�Ƃ��ď���
			drag_data data;
			data.start = log_pos;
			data.value = now_pos - data.start;
			data.is_drag = (data.value != 0);
			erase_drag(now_input & log_input);
			drag_point[now_input & log_input] = data;
		}
	}}

	//�h���b�O�̏���
	//�����̕����X�V
	for (auto i = drag_point.begin(), iend = drag_point.end(); i != iend;)
	{
		if (now_input & i->first) {
			//���������Ă���
			i->second.value = now_pos - i->second.start;
			i->second.is_drag |= (i->second.value != 0);
			now_input &= ~i->first;//�����ςݓo�^
			++i;
		}
		else {
			//�����ꂽ
			i = erase_drag(i);
		}
	}

	//�������̕����Ƃ肠�����h���b�O�Ƃ��ď���
	for (size_t i = 0; i < sizeof(int); ++i)
	{
		if (now_input & (1 << i)) {
			//�V�K����
			//�Ƃ肠�����h���b�O�Ƃ��ď���
			drag_data data;
			data.start = now_pos;
			//data.value = 0;
			data.is_drag = false;
			//erase_drag(now_input & (1 << i));
			drag_point[now_input & (1 << i)] = data;
		}
	}
}
void Mouse::erase_drag(int input)
{
	//���͂̌㏈��
	auto iter = drag_point.find(input);
	if (iter != drag_point.end()) {
		erase_drag(iter);
	}
}

auto Mouse::erase_drag(decltype(drag_point)::const_iterator iter)->decltype(drag_point)::iterator
{
	if (iter->second.is_drag == false) {
		//�N���b�N�Ƃ��ď���
		assert(iter->second.value == 0);
		click_point.emplace_back(click_data{ iter->second.start, iter->first });
	}
	return drag_point.erase(iter);
}
