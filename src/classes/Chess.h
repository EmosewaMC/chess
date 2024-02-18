#ifndef __CHESS__H__
#define __CHESS__H__

#include <array>

#include "Game.h"
#include "ChessSquare.h"

enum ChessPiece {
	Pawn = 1,
	Knight = 2,
	Bishop = 3,
	Rook = 4,
	Queen = 5,
	King = 6
};

//
// the classic game of Rock, Paper, Scissors
//
const int pieceSize = 100;

struct Move {
	std::string from;
	std::string to;
};

class Chess : public Game
{
public:
	static constexpr int BoardSize = 8;
	static constexpr int NumberOfSquares = BoardSize * BoardSize;
	~Chess();

	// set up the board
	void Reset() override;

	Player* checkForWinner() override;
	bool checkForDraw() override;

	std::string initialStateString() override;
	std::string stateString() override;
	void setStateString(const std::string& s) override;
	bool canBitMoveFrom(Bit& bit, BitHolder& src) override;
	bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
	void bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
	void stopGame() override;
	BitHolder& getHolderAt(const int x, const int y) override { return m_Grid[x * BoardSize + y]; }
private:
	std::string pieceNotation(int row, int col);
	Bit* PieceForPlayer(const int playerNumber, const ChessPiece tag, const std::string_view texture);
	Player* ownerAt(int index);
	void    scanForMouse();
	void GenerateMoves(char color);
	void addMoveIfValid(std::vector<Move>& moves, const int fromRow, const int fromColumn, const int toRow, const int toColumn);
	void GenerateKnightMoves(std::vector<Move>& moves, int row, int col);
	void GenerateLinearMoves(std::vector<Move>& moves, int row, int col, std::vector<std::pair<int, int>> directions);
	void GenerateBishopMoves(std::vector<Move>& moves, int row, int col);
	void GenerateRookMoves(std::vector<Move>& moves, int row, int col);
	void GenerateQueenMoves(std::vector<Move>& moves, int row, int col);
	void GenerateKingMoves(std::vector<Move>& moves, int row, int col);
	void GeneratePawnMoves(std::vector<Move>& moves, int row, int col, char color);
	std::array<ChessSquare, NumberOfSquares> m_Grid;
	std::vector<Move> _moves;
};

#endif  //!__CHESS__H__
