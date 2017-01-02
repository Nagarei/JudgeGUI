#pragma once
#include "NetCommon.h"
#include "my_type_traits.h"

class Sender_Bace {
public:
	Sender_Bace(){}
	Sender_Bace(int handle):handle(handle){}
	virtual void send()=0;
	virtual ~Sender_Bace()noexcept{}
protected:
	template<typename T>
	std::enable_if_t<std::is_trivial<T>::value> send_impl(const T& value);
	template<typename T>
	std::enable_if_t<has_iterator<T>::value> send_impl(const T& value);//とりあえずundefined
	template<typename T>
	void send_impl(const std::vector<T>& value);
	template<typename T>
	void send_impl(const T* value) = delete;
private:
	int handle = -1;
};
template<typename... T>
class Sender : Sender_Bace
{
private:
	Send_Data::Type type;
	T... send_value;
public:
	Sender(int handle, Send_Data::Type type, T&&... send_value) 
		: Sender_Bace(handle)
		, type(type)
		, send_value(std::forward<T>(send_value)...)
	{}
	void send()override;

};
template<typename... T>
Sender<T...> make_sender(int handle, Send_Data::Type type, T&&... send_value);
