// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
// VC++�̐ݒ�ɂ��t�@�C���̐擪�Ŏ����I�ɓǂݍ��܂�܂��B

#ifndef STDAFX_H_201505121652_1_6_5_8_
#define STDAFX_H_201505121652_1_6_5_8_

#include "dxlibex/graph2d.hpp"
#include "dxlibex/color.hpp"
#include <Windows.h>
#include <boost/utility.hpp>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <cstdint>
#include <Lmcons.h>
#include "DxLib.h"
#include "FormatToString.h"
#include "Sequence.h"
#include "KeyInputData.h"

// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B

namespace dxle
{
	using pointi32 = dxle::point_c<int32_t>;
	using sizei32 = dxle::size_c<int32_t>;
}

#endif
