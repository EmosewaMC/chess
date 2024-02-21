#include "Chess.h"

#include "Logger.h"
#include "GameGlobal.h"

std::string indexToNotation(int row, int col);

std::string Chess::pieceNotation(int row, int col) {
	std::string notation;
	auto* bit = m_Board[col * ChessBoard::Size + row].bit();
	if (bit) {
		notation.push_back(bit->gameTag() > static_cast<int>('Z') ? 'W' : 'B');
		notation.push_back(static_cast<char>(std::toupper(bit->gameTag())));
	} else {
		notation = "00";
	}
	return notation;
}

//
// make a rock, paper, or scissors piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, const char tag, const std::string_view texture) {
	Bit* bit = new Bit();
	// should possibly be cached from player class?
	bit->LoadTextureFromFile(texture.data());
	bit->setOwner(getPlayerAt(playerNumber));
	bit->setGameTag(tag);
	bit->setSize(pieceSize, pieceSize);
	return bit;
}

void Chess::Reset() {
	LOG("", LogLevel::INFO);
	srand((unsigned int)time(0));
	setNumberOfPlayers(2);
	// this allows us to draw the board correctly
	_gameOptions.rowX = ChessBoard::Size;
	_gameOptions.rowY = ChessBoard::Size;
	// setup the board
	for (int y = 0; y < ChessBoard::NumberOfSquares; y++) {
		m_Board[y].initHolder(ImVec2(100 * static_cast<float>(y / ChessBoard::Size) + 100, 100 * static_cast<float>(y % ChessBoard::Size) + 100), "assets/chess/boardsquare.png", y % ChessBoard::Size, y / ChessBoard::Size);
		m_Board[y].setNotation(indexToNotation(y % ChessBoard::Size, y / ChessBoard::Size));
	}

	// setup the pawns
	for (int i = 0; i < ChessBoard::Size; i++) {
		m_Board[i * 8 + 1].setBit(PieceForPlayer(1, 'P', "assets/chess/b_pawn.png"));
		m_Board[i * 8 + 1].bit()->setPosition(m_Board[i * 8 + 1].getPosition());
		m_Board[i * 8 + 6].setBit(PieceForPlayer(0, 'p', "assets/chess/w_pawn.png"));
		m_Board[i * 8 + 6].bit()->setPosition(m_Board[i * 8 + 6].getPosition());
	}

	// setup the rooks
	m_Board[0].setBit(PieceForPlayer(1, 'R', "assets/chess/b_rook.png"));
	m_Board[0].bit()->setPosition(m_Board[0].getPosition());
	m_Board[7].setBit(PieceForPlayer(0, 'r', "assets/chess/w_rook.png"));
	m_Board[7].bit()->setPosition(m_Board[7].getPosition());
	m_Board[56].setBit(PieceForPlayer(1, 'R', "assets/chess/b_rook.png"));
	m_Board[56].bit()->setPosition(m_Board[56].getPosition());
	m_Board[63].setBit(PieceForPlayer(0, 'r', "assets/chess/w_rook.png"));
	m_Board[63].bit()->setPosition(m_Board[63].getPosition());

	// setup the knights
	m_Board[8].setBit(PieceForPlayer(1, 'N', "assets/chess/b_knight.png"));
	m_Board[8].bit()->setPosition(m_Board[8].getPosition());
	m_Board[15].setBit(PieceForPlayer(0, 'n', "assets/chess/w_knight.png"));
	m_Board[15].bit()->setPosition(m_Board[15].getPosition());
	m_Board[48].setBit(PieceForPlayer(1, 'N', "assets/chess/b_knight.png"));
	m_Board[48].bit()->setPosition(m_Board[48].getPosition());
	m_Board[55].setBit(PieceForPlayer(0, 'n', "assets/chess/w_knight.png"));
	m_Board[55].bit()->setPosition(m_Board[55].getPosition());

	// setup the bishops
	m_Board[16].setBit(PieceForPlayer(1, 'B', "assets/chess/b_bishop.png"));
	m_Board[16].bit()->setPosition(m_Board[16].getPosition());
	m_Board[23].setBit(PieceForPlayer(0, 'b', "assets/chess/w_bishop.png"));
	m_Board[23].bit()->setPosition(m_Board[23].getPosition());
	m_Board[40].setBit(PieceForPlayer(1, 'B', "assets/chess/b_bishop.png"));
	m_Board[40].bit()->setPosition(m_Board[40].getPosition());
	m_Board[47].setBit(PieceForPlayer(0, 'b', "assets/chess/w_bishop.png"));
	m_Board[47].bit()->setPosition(m_Board[47].getPosition());

	// setup the queens
	m_Board[24].setBit(PieceForPlayer(1, 'Q', "assets/chess/b_queen.png"));
	m_Board[24].bit()->setPosition(m_Board[24].getPosition());
	m_Board[31].setBit(PieceForPlayer(0, 'q', "assets/chess/w_queen.png"));
	m_Board[31].bit()->setPosition(m_Board[31].getPosition());

	// setup the kings
	m_Board[32].setBit(PieceForPlayer(1, 'K', "assets/chess/b_king.png"));
	m_Board[32].bit()->setPosition(m_Board[32].getPosition());
	m_Board[39].setBit(PieceForPlayer(0, 'k', "assets/chess/w_king.png"));
	m_Board[39].bit()->setPosition(m_Board[39].getPosition());

	// if we have an AI set it up
	if (gameHasAI()) {
		setAIPlayer(_gameOptions.AIPlayer);
	}
	// setup up turns etc.
	startGame();
	GenerateMoves('W');
}

bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src) {
	(void)bit;
	auto& srcSquare = static_cast<ChessSquare&>(src);
	for (auto& move : _moves) {
		if (move.from == srcSquare.getNotation()) {
			return true;
		}
	}
	return false;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
	(void)bit;
	auto& srcSquare = static_cast<ChessSquare&>(src);
	auto& dstSquare = static_cast<ChessSquare&>(dst);
	for (auto& move : _moves) {
		if (move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation()) {
			return true;
		}
	}
	return false;
}

void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
	Game::bitMovedFromTo(bit, src, dst);
	GenerateMoves(_gameOptions.currentTurnNo & 1 ? 'B' : 'W');
}

std::string indexToNotation(int row, int col) {
	std::string notation;
	notation.push_back('a' + static_cast<char>(col));
	notation.push_back('8' - static_cast<char>(row));
	return notation;
}

void Chess::addMoveIfValid(std::vector<Move>& moves, const int fromRow, const int fromColumn, const int toRow, const int toColumn) {
	if (toRow >= 0 && toRow < ChessBoard::Size && toColumn >= 0 && toColumn < ChessBoard::Size) {
		if (pieceNotation(fromRow, fromColumn)[0] != pieceNotation(toRow, toColumn)[0]) {
			moves.push_back({ indexToNotation(fromRow, fromColumn), indexToNotation(toRow, toColumn) });
		}
	}
}

void Chess::GenerateBishopMoves(std::vector<Move>& moves, int row, int col) {
	static const std::vector<std::pair<int, int>> directions = { { +1, +1 }, { +1, -1 }, { -1, +1 }, { -1, -1 } };
	GenerateLinearMoves(moves, row, col, directions);
}

void Chess::GenerateRookMoves(std::vector<Move>& moves, int row, int col) {
	static const std::vector<std::pair<int, int>> directions = { { +1, 0 }, { -1, 0 }, { 0, +1 }, { 0, -1 } };
	GenerateLinearMoves(moves, row, col, directions);
}

void Chess::GenerateQueenMoves(std::vector<Move>& moves, int row, int col) {
	GenerateBishopMoves(moves, row, col);
	GenerateRookMoves(moves, row, col);
}

void Chess::GenerateKnightMoves(std::vector<Move>& moves, int row, int col) {
	static const int rowOffsets[] = { -2, -2, +2, +2, +1, +1, -1, -1 };
	static const int colOffsets[] = { +1, -1, +1, -1, +2, -2, +2, -2 };

	for (int i = 0; i < 8; i++) {
		int toRow = row + rowOffsets[i];
		int toCol = col + colOffsets[i];
		addMoveIfValid(moves, row, col, toRow, toCol);
	}
}

void Chess::GenerateKingMoves(std::vector<Move>& moves, int row, int col) {
	static const int rowOffsets[] = { -1, -1, -1,  0,  0, +1, +1, +1 };
	static const int colOffsets[] = { -1,  0, +1, -1, +1, -1,  0, +1 };

	for (int i = 0; i < 8; i++) {
		int toRow = row + rowOffsets[i];
		int toCol = col + colOffsets[i];
		addMoveIfValid(moves, row, col, toRow, toCol);
	}
}

void Chess::GenerateLinearMoves(std::vector<Move>& moves, int row, int col, std::vector<std::pair<int, int>> directions) {
	for (auto& direction : directions) {
		int toRow = row + direction.first;
		int toCol = col + direction.second;
		while (toRow >= 0 && toRow < ChessBoard::Size && toCol >= 0 && toCol < ChessBoard::Size) {
			if (pieceNotation(toRow, toCol) != "00") {
				addMoveIfValid(moves, row, col, toRow, toCol);
				break;
			}
			addMoveIfValid(moves, row, col, toRow, toCol);
			toRow += direction.first;
			toCol += direction.second;
		}
	}
}

void Chess::GeneratePawnMoves(std::vector<Move>& moves, int row, int col, char color) {
	// first add the forward moves
	int forward = color == 'W' ? -1 : 1;
	if (row + forward >= 0 && row + forward < ChessBoard::Size && pieceNotation(row + forward, col) == "00") {
		addMoveIfValid(moves, row, col, row + forward, col);
		if ((row == 1 && pieceNotation(row + forward * 2, col) == "00") || (row == 6 && pieceNotation(row + forward * 2, col) == "00")) {
			addMoveIfValid(moves, row, col, row + forward * 2, col);
		}
	}

	// now add the attack moves
	if (row + forward >= 0 && row + forward < ChessBoard::Size && col + 1 < ChessBoard::Size) {
		if (pieceNotation(row + forward, col + 1) != "00") {
			addMoveIfValid(moves, row, col, row + forward, col + 1);
		}
	}

	if (row + forward >= 0 && row + forward < ChessBoard::Size && col - 1 >= 0) {
		if (pieceNotation(row + forward, col - 1) != "00") {
			addMoveIfValid(moves, row, col, row + forward, col - 1);
		}
	}

}

void Chess::GenerateMoves(char color) {
	_moves.clear();
	for (int col = 0; col < ChessBoard::Size; col++) {
		for (int row = 0; row < ChessBoard::Size; row++) {
			std::string piece = pieceNotation(row, col);
			if (!piece.empty() && piece != "00" && piece[0] == color) {
				switch (piece[1]) {
				case 'P':
					GeneratePawnMoves(_moves, row, col, piece[0]);
					break;
				case 'R':
					GenerateRookMoves(_moves, row, col);
					break;
				case 'N':
					GenerateKnightMoves(_moves, row, col);
					break;
				case 'B':
					GenerateBishopMoves(_moves, row, col);
					break;
				case 'Q':
					GenerateQueenMoves(_moves, row, col);
					break;
				case 'K':
					GenerateKingMoves(_moves, row, col);
					break;
				}
			}
		}
	}
}
//
// free all the memory used by the game on the heap
//
void Chess::stopGame() {
	for (auto& square : m_Board) {
		square.destroyBit();
	}
}

//
// helper function for the winner check
//
Player* Chess::ownerAt(int index) {
	if (index < 0 || index > m_Board.size()) {
		return nullptr;
	}
	if (!m_Board[index].bit()) {
		return nullptr;
	}
	return m_Board[index].bit()->getOwner();
}

Player* Chess::checkForWinner() {
	return nullptr;
}

bool Chess::checkForDraw() {
	return false;
}

//
// state strings
//
std::string Chess::initialStateString() {
	static std::string s('0', ChessBoard::NumberOfSquares);
	return s;
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString() {
	std::stringstream s;
	s << '\n';
	for (int y = 0; y < ChessBoard::Size; y++) {
		for (int x = 0; x < ChessBoard::Size; x++) {
			s << pieceNotation(y, x);
		}
		s << '\n';
	}
	return s.str();
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string& s) {
	(void)s;
}

