#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <cstring>
#include <cstdint>

class String1
{
private:
	char* str;
	static size_t instance_num;
public:
	String1(char const * const str = 0);

	String1(const String1& str);

	~String1();

	String1& operator= (String1& r);

	char* get_str() const { return str; }

	static size_t get_instance_num();
};

size_t String1::instance_num = 0;

size_t String1::get_instance_num()
{
	return String1::instance_num;
}

String1::String1(char const * const str)
{
	if( str == 0 )
	{
		this->str = new char[1];
		this->str[0] = '\0';
	}
	else
	{
		this->str = new char[strlen(str)+1];
		strcpy(this->str, str);
	}
	this->instance_num++;
}

String1::String1(const String1& str)
{
	if( str.get_str() == 0 )
	{
		this->str = new char[1];
		this->str[0] = '\0';
	}
	else
	{
		this->str = new char[strlen(str.get_str())+1];
		strcpy(this->str, str.get_str());
	}
	this->instance_num++;
}

String1::~String1()
{
	delete[] this->str;
}

String1& String1::operator= (String1& r)
{
	if(r.get_str() == this->get_str())
		return *this;

	delete[] this->str;
	this->str = new char[strlen(r.get_str()) + 1];
	strcpy(this->str, r.get_str());
	return *this;
}

#endif