#ifndef __COMPLEX_HH__
#define __COMPLEX_HH__

#include <cstdio>

class complex
{
private:
	double re, im;

public:
	complex(double r=0, double i=0)
		: re(r), im(i)
	{
		// std::cout << "ClassInit 1" << std::endl;
		printf("ClassInit 1\n\r");
	}
	complex(complex& c)
	{
		this->re = c.re;
		this->im = c.im;
		// std::cout << "ClassInit 2" << std::endl;
		printf("ClassInit 2\n\r");
	}

	~complex() {printf("destructor\n\r");}

	double real() const { return re; }
	double real(double re) { return this->re = re; }

	double imag() const { return im; }
	double imag(double im) { return this->im = im; };

	complex& operator = (const complex& r);
};


static inline complex& __doapl(complex& l, const complex& r)
{
	l.real( l.real() + r.real() );
	l.imag( l.real() + r.imag() );
	return l;
}

static inline complex& operator += (complex& l, const complex& r)
{
	return __doapl(l,r);
}

complex& complex::operator = (const complex& r)
{
	// std::cout << "ClassInit 3" << std::endl;
	printf("ClassInit 3\n\r");
	this->real(r.real());
	this->imag(r.imag());
	return *this;
}

// static inline std::ostream& operator << (std::ostream& os, const complex& x)
// {
// 	return os << '(' << x.real() << ',' << x.imag() << ')';
// }



#endif