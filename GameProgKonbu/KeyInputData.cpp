#include "KeyInputData.h"

KeyInputData::KeyInputData()
	: now( GetNowCount() )
	, now_key(key_buf)
	, last_key(key_buf+256)
{
	for(int& i : last_arrow_key){
		i = -1;
	}
	for(auto& i : key_buf){
		i = 0;
	}
}
void KeyInputData::Update()
{
	//�X�V�O�̕����L�[�̏�Ԃ�ۑ����Ă���
	for (uint8_t i = 0; i < end_DirectionKey; ++i)
	{
		if( now_key[GetKeyMask(static_cast<DirectionKey>(i))] == 1 ){//�L�[��������Ă���ꍇ
			if((last_arrow_key[i] + 200 <= now)){//0.2�b�O�L�[��������Ă��Ȃ��ꍇ
				last_arrow_key[i] = now;//����������
			}
		}
		else{
			last_arrow_key[i] = -1;
		}
	}
	//�X�V
	std::swap(last_key, now_key);
	GetHitKeyStateAll(now_key);//���݂̃L�[�̏�Ԃ��L�^
	now = GetNowCount();
}
uint_fast8_t KeyInputData::GetKeyMask(DirectionKey direction)
{
	switch (direction)
	{
	case KeyInputData::KEY_UP:
		return KEY_INPUT_UP;
	case KeyInputData::KEY_DOWN:
		return KEY_INPUT_DOWN;
	case KeyInputData::KEY_LEFT:
		return KEY_INPUT_LEFT;
	case KeyInputData::KEY_RIGHT:
		return KEY_INPUT_RIGHT;
	default:
		assert(false);
		return 0;
	}
}
bool KeyInputData::GetKeyInput(uint_fast8_t Mask)
{
	return now_key[Mask] == 1;
}
bool KeyInputData::GetNewKeyInput(uint_fast8_t Mask)
{
	return ((last_key[Mask] != 1) && (now_key[Mask] == 1));
}
bool KeyInputData::GetDirectionKeyInput(DirectionKey direction)
{
	if( now_key[GetKeyMask(direction)] ){//�L�[��������Ă���ꍇ
		if((last_arrow_key[direction] + 200 <= now)){//0.2�b�O�L�[��������Ă��Ȃ��ꍇ
			return true;
		}
	}
	return false;
}
