#pragma once
#ifndef MY_UTILITY_H_201503222223_1223_xhj
#define MY_UTILITY_H_201503222223_1223_xhj

static constexpr double sqrt3 = (1.7320508075688772935274463415059);


template<typename FUNC>
struct Finally_C {
	Finally_C(FUNC&& func_):func(std::forward<FUNC>(func_)){}
	~Finally_C() { func(); }
	FUNC func;
};
template<typename FUNC>
Finally_C<FUNC> Finally(FUNC&& func_) {
	return Finally_C<FUNC>{ std::forward<FUNC>(func_) };
}
#define FINALLY_2(FUNC,N) \
	auto&& FINALLY_ ## N ## _FINALLY_ = Finally(FUNC);(void*)(&FINALLY_ ## N ## _FINALLY_)
#define FINALLY_1(FUNC,N) FINALLY_2(FUNC, N)
#define FINALLY(FUNC) FINALLY_1(FUNC, __COUNTER__)

inline bool IsInArea(dxle::pointi pos, dxle::pointi area_pos1, dxle::sizei area_size)
{
	return ((area_pos1.x <= pos.x && pos.x < area_pos1.x + area_size.width) &&
		(area_pos1.y <= pos.y && pos.y < area_pos1.y + area_size.height));
}



//-------type_modifier-------
//上から修飾子なし用、const用、volatile用、const volatile用

//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type, typename base_type>
struct type_modifier {
	typedef base_type result_type;
};
//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type, typename base_type>
struct type_modifier<const check_type, base_type> {
	typedef const base_type result_type;
};
//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type, typename base_type>
struct type_modifier<volatile check_type, base_type> {
	typedef volatile base_type result_type;
};
//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type, typename base_type>
struct type_modifier<const volatile check_type, base_type> {
	typedef const volatile base_type result_type;
};

//------pointer_cast-------

//ポインタからポインタへの型変換を行う
template<typename return_type, typename parameter_type>
inline return_type pointer_cast(parameter_type*const& pointer)
{
	//いったん対応する修飾子をつけたvoid*に変換してからもう一度キャストする
	return static_cast<return_type>(static_cast<typename type_modifier<parameter_type, void*>::result_type >(pointer));
}

//BIT

template<unsigned long long int bits>
struct BIT {
	enum { decimal = (BIT<bits % 10>::decimal + BIT<bits / 10>::decimal * 2) };
	enum { value = decimal };
};
template<>
struct BIT<1> {
	enum { decimal = 1 };
	enum { value = decimal };
};
template<>
struct BIT<0> {
	enum { decimal = 0 };
	enum { value = decimal };
};

#ifdef _DEBUG
#define DEBUG_NOTE
#else
#define DEBUG_NOTE static_assert(false,"");
#endif

//***選択肢に使うenum class:unsigned char;***
#define EnumClassSelectDeclarationMain(ENUM,ENUM_NAME,...) \
	enum class ENUM:unsigned char{ __VA_ARGS__ , end}ENUM_NAME;\
	friend auto operator++(ENUM& param)->ENUM&{\
		unsigned char temp(static_cast<unsigned char>(param));\
		if(++temp == static_cast<unsigned char>(ENUM::end)){\
			temp = 0;\
		}\
		return (param = static_cast<ENUM>(temp));\
	}\
	friend auto operator--(ENUM& param)->ENUM&{\
		unsigned char temp(static_cast<unsigned char>(param));\
		if(temp == 0){\
			temp = static_cast<unsigned char>(ENUM::end)-1ui8;\
		}\
		else{\
			--temp;\
		}\
		return (param = static_cast<ENUM>(temp));\
	}

//***選択肢に使うenum:unsigned char;***
#define EnumSelectDeclarationMain(ENUM,ENUM_NAME,...) \
	enum ENUM:unsigned char{ __VA_ARGS__ , ENUM##_end}ENUM_NAME;\
	friend auto operator++(ENUM& param)->ENUM&{\
		unsigned char temp(static_cast<unsigned char>(param));\
		if(++temp == static_cast<unsigned char>(ENUM::ENUM##_end)){\
			temp = 0;\
		}\
		return (param = static_cast<ENUM>(temp));\
	}\
	friend auto operator--(ENUM& param)->ENUM&{\
		unsigned char temp(static_cast<unsigned char>(param));\
		if(temp == 0){\
			temp = static_cast<unsigned char>(ENUM::ENUM##_end)-1ui8;\
		}\
		else{\
			--temp;\
		}\
		return (param = static_cast<ENUM>(temp));\
	}

//***選択肢に使うenum class Select:unsigned char;***
#define EnumClassSelectDeclaration(...) EnumClassSelectDeclarationMain(Select,selecting,__VA_ARGS__)


#endif
