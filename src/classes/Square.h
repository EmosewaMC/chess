#pragma once

#include "BitHolder.h"

class Square : public BitHolder
{
public:
	// initialize the holder with a position, color, and a sprite
	void initHolder(const ImVec2& position, const char* spriteName, const int column, const int row);

private:
	int _column = 0;
	int _row = 0;
};
