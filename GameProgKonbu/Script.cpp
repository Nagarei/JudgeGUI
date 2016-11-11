#include "Script.h"
#include "useful_func_and_class.h"

namespace Script
{
	Plain_Text::Plain_Text(dxle::tstring& str)
	{
		//一行目
		auto first_eiter = std::find(str.begin(), str.end(), _T('\n'));
		if (first_eiter == str.end()){ return; }
		line_str[0].assign(str.begin(), first_eiter);
		SetLineSize_first();
		//末端行
		auto last_siter = あああ std::find(str.begin(), str.end(), _T('\n'));
		if (last_siter == str.end()){ return; }
		line_str[0].assign(str.begin(), first_iter);
		SetLineSize_first();
	}
	std::unique_ptr<Script> Plain_Text::get_script(dxle::tstring& str)
	{
		//常に作る
		return std::make_unique<Plain_Text>(str);
	}


	std::unique_ptr<Script> build_script(dxle::tstring& str)
	{
#define RUN_bs(CLASS) if(auto res = CLASS::get_script(str)){return res;}
		
#undef  RUN_bs
		return Text::get_script(str);
	}


}
