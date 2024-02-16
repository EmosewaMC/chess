#ifndef __CHESS__H__
#define __CHESS__H__

#include <array>

#include "Game.h"
#include "ChessSquare.h"

//
// the classic game of Rock, Paper, Scissors
//
const int pieceSize = 100;

class Chess : public Game
{
public:
	static constexpr int BoardSize = 8;
	static constexpr int NumberOfSquares = BoardSize * BoardSize;
	Chess();
	~Chess();

	// set up the board
	void setUpBoard() override;

	Player* checkForWinner() override;
	bool checkForDraw() override;

	std::string initialStateString() override;
	std::string stateString() override;
	void setStateString(const std::string& s) override;
	bool actionForEmptyHolder(BitHolder& holder) override;
	bool canBitMoveFrom(Bit& bit, BitHolder& src) override;
	bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
	void stopGame() override;
	BitHolder& getHolderAt(const int x, const int y) override { return m_Grid[x * BoardSize + y]; }

private:
	Bit* PieceForPlayer(const int playerNumber, const std::string_view texture);
	Player* ownerAt(int index);
	void    scanForMouse();
	std::array<ChessSquare, NumberOfSquares> m_Grid;
};

#endif  //!__CHESS__H__
