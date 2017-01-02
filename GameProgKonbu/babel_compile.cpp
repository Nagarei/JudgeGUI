#include "babel/babel.cpp"

void erase_BOM(std::string& str)
{
	static_assert(sizeof(char) == 1, "");

	auto str_bite_size = str.size() * sizeof(std::string::value_type);
	auto impl = [&str, &str_bite_size](const char BOM[], size_t length)->size_t
	{
		if (str_bite_size < length) { return 0; }
		const char* iter = reinterpret_cast<const char*>(str.data());
		for (auto b_iter = BOM, b_iter_end = BOM + length;
			b_iter != b_iter_end; ++b_iter, ++iter) {
			if (*b_iter != *iter) {
				return 0;
			}
		}
		return length;
	};
	size_t res;
	res = impl("\xEF\xBB\xBF", 3); if (0 < res) { str.erase(0, res); return; }//UTF-8
	res = impl("\xFE\xFF", 2); if (0 < res) { str.erase(0, res); return; }//UTF-16 BE
	res = impl("\xFF\xFE", 2); if (0 < res) { str.erase(0, res); return; }//UTF-16 LE
	res = impl("\x00\x00\xFE\xFF", 4); if (0 < res) { str.erase(0, res); return; }//UTF-32 BE
	res = impl("\xFF\xFE\x00\x00", 4); if (0 < res) { str.erase(0, res); return; }//UTF-32 LE
}
