#pragma once
#include "my_utility.h"
#include "button.h"
class Option final
{
public:
	Option(const Option&) = delete;
	Option& operator=(const Option&) = delete;

	static constexpr unsigned username_length = 100;
	TCHAR username[username_length];
	static const Option& GetIns() {
		return ins;
	}
private:
	Option() = default;
	~Option() = default;
	static Option ins;
	friend class Option_Sequence;
};

class Option_Sequence final : public Sequence
{
private:
	EnumClassSelectDeclaration(local, server_contact, contest, name);
	dxle::texture2d str_graph;
	int name_input_handle;
	std::array<Button, 4> buttons;
	bool is_username_inputing;
public:
	Option_Sequence();
	~Option_Sequence();
	std::unique_ptr<Sequence> update()override;
	void draw()const override;
};
