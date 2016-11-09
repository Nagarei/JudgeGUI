#include "Script.h"
#include "useful_func_and_class.h"

namespace Script
{
	std::vector<std::unique_ptr<Script>> build_script(const dxle::tstring & file_name)
	{
		tifstream problem_file(file_name);

		dxle::tstring temp_str;
		std::getline(problem_file, temp_str, _T('@'));//'@'Ç™Ç≈ÇÈÇ‹Ç≈ì«Ç›çûÇﬁ


		std::vector<std::unique_ptr<Script>> result;
		return ;
	}


}
