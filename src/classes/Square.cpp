#include "Square.h"
#include <string>

void Square::initHolder(const ImVec2& position, const char* spriteName, const int column, const int row) {
	_column = column;
	_row = row;
	ImVec4 color = (column + row) % 2 != 0 ?
		ImVec4(1, 1, 1, 0.5) :
		ImVec4(0.5, 0.5, 0.75, 0.5);

	BitHolder::initHolder(position, color, spriteName);
}
