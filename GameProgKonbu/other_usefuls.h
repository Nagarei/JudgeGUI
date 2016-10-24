#pragma once

//class Uncopyable : public boost::noncopyable
//{};


	//-------type_modifier-------
//上から修飾子なし用、const用、volatile用、const volatile用

//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type , typename base_type>
struct type_modifier{
	typedef base_type result_type;
};
//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type , typename base_type>
struct type_modifier<const check_type,base_type>{
	typedef const base_type result_type;
};
//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type , typename base_type>
struct type_modifier<volatile check_type,base_type>{
	typedef volatile base_type result_type;
};
//第一テンプレート引数についている修飾子を第二テンプレート引数につける
template<typename check_type , typename base_type>
struct type_modifier<const volatile check_type,base_type>{
	typedef const volatile base_type result_type;
};

	//------pointer_cast-------

//ポインタからポインタへの型変換を行う
template<typename return_type , typename parameter_type>
inline return_type pointer_cast(parameter_type*const& pointer)
{
	//いったん対応する修飾子をつけたvoid*に変換してからもう一度キャストする
	return static_cast<return_type>(static_cast<typename type_modifier<parameter_type,void*>::result_type >(pointer));
}

	//BIT

template<unsigned long long int bits>
struct BIT{
	enum{ decimal = (BIT<bits % 10>::decimal + BIT<bits / 10>::decimal * 2) };
	enum{ value = decimal };
};
template<>
struct BIT<1>{
	enum{ decimal = 1 };
	enum{ value = decimal };
};
template<>
struct BIT<0>{
	enum{ decimal = 0 };
	enum{ value = decimal };
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
