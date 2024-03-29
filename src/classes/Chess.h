#ifndef __CHESS__H__
#define __CHESS__H__

#include <array>

#include "Game.h"
#include "ChessSquare.h"

//
// the classic game of Rock, Paper, Scissors
//
const int pieceSize = 100;

struct Move {
	std::string from;
	std::string to;
};

class ChessBoard {
public:
	static constexpr int Size = 8;
	static constexpr int NumberOfSquares = Size * Size;

	typedef std::array<ChessSquare, NumberOfSquares>::iterator iterator;

	ChessSquare& GetSquareAt(const int rank, const int file) { return m_Grid[rank + file * Size]; }
	ChessSquare& GetSquareAt(const int index) { return m_Grid[index]; }
	ChessSquare& operator[](const int index) { return m_Grid[index]; }
	iterator begin() { return m_Grid.begin(); }
	iterator end() { return m_Grid.end(); }
	size_t size() const { return m_Grid.size(); }

	static constexpr int RankFromIndex(const int index) { return index / Size; }
	static constexpr int FileFromIndex(const int index) { return index % Size; }

private:
	std::array<ChessSquare, NumberOfSquares> m_Grid;
};

class Chess : public Game
{
public:
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
	BitHolder& getHolderAt(const int x, const int y) override { return m_Board.GetSquareAt(y, x); }
	void updateAI() override;
private:
	char pieceNotation(int row, int col);
	Bit* PieceForPlayer(const int playerNumber, const char tag, const std::string_view texture);
	Player* ownerAt(int index);
	void    scanForMouse();
	std::vector<Move> GenerateMoves(std::string state, char color, bool filterMoves = false);
	void FilterIllegalMoves(std::vector<Move>& moves, std::string state, char color);
	void addMoveIfValid(std::vector<Move>& moves, const std::string& state, const int fromRow, const int fromColumn, const int toRow, const int toColumn);
	void GenerateKnightMoves(std::vector<Move>& moves, const std::string& state, int row, int col);
	void GenerateLinearMoves(std::vector<Move>& moves, const std::string& state, int row, int col, std::vector<std::pair<int, int>> directions);
	void GenerateBishopMoves(std::vector<Move>& moves, const std::string& state, int row, int col);
	void GenerateRookMoves(std::vector<Move>& moves, const std::string& state, int row, int col);
	void GenerateQueenMoves(std::vector<Move>& moves, const std::string& state, int row, int col);
	void GenerateKingMoves(std::vector<Move>& moves, const std::string& state, int row, int col);
	void GeneratePawnMoves(std::vector<Move>& moves, const std::string& state, int row, int col, char color);
	int EvaluateBoard(const std::string& state);
	int negamax(std::string state, int depth, int alpha, int beta, int color);

	ChessBoard m_Board;
	std::vector<Move> _moves;
	bool m_BlackCastleKingSide = true;
	bool m_BlackCastleQueenSide = true;
	bool m_WhiteCastleKingSide = true;
	bool m_WhiteCastleQueenSide = true;
	std::string m_EnPassantSquare = "";
	bool m_GameOver = false;
	int m_WinningPlayer = -1;
};

#endif  //!__CHESS__H__
