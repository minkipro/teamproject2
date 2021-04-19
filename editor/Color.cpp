#include "Color.h"

Color::Color()
	:_color(0)
{}

Color::Color(unsigned int val)
	: _color(val)
{}

Color::Color(BYTE r, BYTE g, BYTE b)
	: Color(r, g, b, 255)
{
}

Color::Color(BYTE r, BYTE g, BYTE b, BYTE a)
{
	_rgba[0] = r;
	_rgba[1] = g;
	_rgba[2] = b;
	_rgba[3] = a;
}

Color::Color(const Color& src)
	:_color(src._color)
{}

Color& Color::operator=(const Color& src)
{
	_color = src._color;
	return *this;
}

bool Color::operator==(const Color& rhs) const
{
	return (_color == rhs._color);
}

bool Color::operator!=(const Color& rhs) const
{
	return !(*this == rhs);
}

constexpr BYTE Color::GetR() const
{
	return _rgba[0];
}
void Color::SetR(BYTE r)
{
	_rgba[0] = r;
}

constexpr BYTE Color::GetG() const
{
	return _rgba[1];
}
void Color::SetG(BYTE g)
{
	_rgba[1] = g;
}

constexpr BYTE Color::GetB() const
{
	return _rgba[2];
}
void Color::SetB(BYTE b)
{
	_rgba[2] = b;
}

constexpr BYTE Color::GetA() const
{
	return _rgba[3];
}
void Color::SetA(BYTE a)
{
	_rgba[3] = a;
}