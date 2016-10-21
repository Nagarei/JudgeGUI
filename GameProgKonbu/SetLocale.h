#ifndef SET_LOCALE_H_20150728_2104_1685168596
#define SET_LOCALE_H_20150728_2104_1685168596

inline void SetLocale()
{
	struct Initer{
		Initer(){
			std::locale jpn_loc_all("Japanese");
			std::locale jpn_loc(std::locale(""), jpn_loc_all, std::locale::ctype | std::locale::collate);
			std::locale::global(jpn_loc);
		}
	};
	static Initer initer;
}

#endif
