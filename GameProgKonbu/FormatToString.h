//printf�̂悤�ȃt�H�[�}�b�g��std::string�ɕϊ�����
//(boost::format�̏������󂯎����std::string�ɕϊ�����)

// 2015/11/07 by nagata
//�g�p����boost���C���N���[�h�f�B���N�g���ɒǉ����Ă�������
//���Ƃ͂��̃w�b�_���C���N���[�h���邾���Ŏg���܂�
//�i����Z:\library\boost�j

#ifndef FORMAT_TO_STRING_2015_11_07_1500_415386410534036025
#define FORMAT_TO_STRING_2015_11_07_1500_415386410534036025
#include <boost/format.hpp>
#include <string>
#include <sstream>
#include <utility>

template<typename Format_T>
inline dxle::tstring FormatToString_impl(Format_T&& format){
	std::basic_stringstream<TCHAR> ss;
	ss << std::forward<Format_T>(format);
	return ss.str();
}
template<typename Format_T, typename Type1, typename... Args>
inline dxle::tstring FormatToString_impl(Format_T&& format, Type1&& param1, Args&&... args)
{
	return FormatToString_impl(std::forward<Format_T>(format) % std::forward<Type1>(param1), std::forward<Args>(args)...);
}


template<typename... Args>
//printf�̂悤�ȃt�H�[�}�b�g��string�ɕϊ�����
//(boost::format�̏������󂯎����string�ɕϊ�����)
inline dxle::tstring FormatToString(dxle::tstring format_string, Args&&... args)
{
	return FormatToString_impl(boost::basic_format<TCHAR>(format_string), std::forward<Args>(args)...);
}


//�󂯎�����̂�Ђ��[����ostream�ɓ˂�����
inline void ToStringEx_impl(std::basic_ostream<TCHAR>&)
{}

template<typename Type1, typename... Args >
//�󂯎�����̂�Ђ��[����ostream�ɓ˂�����
inline void ToStringEx_impl(std::basic_ostream<TCHAR>& os, Type1&& param1, Args&&... args)
{
	ToStringEx_impl(os << std::forward<Type1>(param1), std::forward<Args>(args)...);
}

template<typename... Args>
//�󂯎�����̂�Ђ��[����sstream�ɓ˂�����
inline dxle::tstring ToStringEx(Args&&... args)
{
	std::basic_stringstream<TCHAR> ss;
	ToStringEx_impl(ss, std::forward<Args>(args)...);
	return ss.str();
}

#endif
