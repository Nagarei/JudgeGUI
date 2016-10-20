#pragma once
#include <memory>

class Sequence{
public:
	virtual std::unique_ptr<Sequence> update() = 0;
	virtual void draw()const = 0;
	virtual ~Sequence(){}
};