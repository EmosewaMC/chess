#include "ChessSquare.h"
#include "Chess.h"

ChessSquare::~ChessSquare() {
	destroyBit();
}

void ChessSquare::initHolder(const ImVec2& position, const char* spriteName, const int column, const int row) {
	_column = column;
	_row = row;
	int odd = (column + row) % 2;
	ImVec4 color = odd ? ImVec4(0.48f, 0.58f, 0.36f, 1.0f) : ImVec4(0.93f, 0.93f, 0.84f, 1.0f);
	BitHolder::initHolder(position, color, spriteName);
	setSize(pieceSize, pieceSize);
}

bool ChessSquare::canDropBitAtPoint(Bit* newbit, const ImVec2& point) {
	(void)point;
	if (!bit()) return true;
	auto gameTag = newbit->gameTag();
	return gameTag != '0' && std::isupper(bit()->gameTag()) != std::isupper(newbit->gameTag());
}

bool ChessSquare::dropBitAtPoint(Bit* newbit, const ImVec2& point) {
	(void)point;
	if (bit() == nullptr) {
		setBit(newbit);
		newbit->setParent(this);
		newbit->moveTo(getPosition());
		return true;
	}

	// we're taking a piece!
	auto gameTag = newbit->gameTag();
	if (gameTag != '0' && std::isupper(bit()->gameTag()) != std::isupper(gameTag)) {
		setBit(newbit);
		newbit->setParent(this);
		newbit->moveTo(getPosition());
		return true;
	}
	return false;
}

void ChessSquare::setMoveHighlighted(bool highlighted) {
	int odd = (_column + _row) % 2;
	_color = odd ? ImVec4(0.93f, 0.93f, 0.84f, 1.0f) : ImVec4(0.48f, 0.58f, 0.36f, 1.0f);
	if (highlighted) {
		_color = odd ? ImVec4(0.48f, 0.58f, 0.36f, 1.0f) : ImVec4(0.93f, 0.93f, 0.84f, 1.0f);
		_color = Lerp(_color, ImVec4(0.75f, 0.79f, 0.30f, 1.0f), 0.75f);
	}
}
