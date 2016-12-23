#pragma once
#include "Sequence.h"

class NetInit final : public Sequence
{
private:
	int name_input_handle;
	std::array<Button, 4> buttons;
	bool is_username_inputing;
public:
	Option_Sequence();
	~Option_Sequence();
	std::unique_ptr<Sequence> update()override;
	void draw()const override;
};
class NetMain final : public Sequence
{
private:
	int name_input_handle;
	std::array<Button, 4> buttons;
	bool is_username_inputing;
public:
	Option_Sequence();
	~Option_Sequence();
	std::unique_ptr<Sequence> update()override;
	void draw()const override;
};
