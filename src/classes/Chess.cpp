#include "Chess.h"

#include "Logger.h"
#include "GameGlobal.h"

std::string indexToNotation(int row, int col);

std::string Chess::pieceNotation(int row, int col) {
	const char* pieces = { "?PNBRQK" };
	std::string notation;
	auto* bit = m_Grid[col * BoardSize + row].bit();
	if (bit) {
		notation.push_back(bit->gameTag() < 128 ? 'W' : 'B');
		notation.push_back(pieces[bit->gameTag() & 127]);
	} else {
		notation = "00";
	}
	return notation;
}

Chess::Chess() {
}

Chess::~Chess() {
}

//
// make a rock, paper, or scissors piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, const ChessPiece tag, const std::string_view texture) {
	Bit* bit = new Bit();
	// should possibly be cached from player class?
	bit->LoadTextureFromFile(texture.data());
	bit->setOwner(getPlayerAt(playerNumber));
	bit->setGameTag((playerNumber * 128) | tag);
	bit->setSize(pieceSize, pieceSize);
	return bit;
}

void Chess::setUpBoard() {
	LOG("", LogLevel::INFO);
	srand((unsigned int)time(0));
	setNumberOfPlayers(2);
	// this allows us to draw the board correctly
	_gameOptions.rowX = BoardSize;
	_gameOptions.rowY = BoardSize;
	// setup the board
	for (int y = 0; y < NumberOfSquares; y++) {
		LOG("square {} row {} col {}", LogLevel::INFO, y, y % BoardSize, y / BoardSize);		
		m_Grid[y].initHolder(ImVec2(100 * static_cast<float>(y / BoardSize) + 100, 100 * static_cast<float>(y % BoardSize) + 100), "assets/chess/boardsquare.png", y % BoardSize, y / BoardSize);
		m_Grid[y].setNotation(indexToNotation(y % BoardSize, y / BoardSize));
	}

	// int f = 0;
	// for (auto& square : m_Grid) {
	// 	square.setBit(PieceForPlayer(f % 2, "assets/chess/w_pawn.png"));
	// 	square.bit()->setPosition(square.getPosition());
	// 	f++;
	// }

	// setup the pawns
	for (int i = 0; i < BoardSize; i++) {
		m_Grid[i * 8 + 1].setBit(PieceForPlayer(1, ChessPiece::Pawn, "assets/chess/b_pawn.png"));
		m_Grid[i * 8 + 1].bit()->setPosition(m_Grid[i * 8 + 1].getPosition());
		m_Grid[i * 8 + 6].setBit(PieceForPlayer(0, ChessPiece::Pawn, "assets/chess/w_pawn.png"));
		m_Grid[i * 8 + 6].bit()->setPosition(m_Grid[i * 8 + 6].getPosition());
	}

	// setup the rooks
	m_Grid[0].setBit(PieceForPlayer(1, ChessPiece::Rook, "assets/chess/b_rook.png"));
	m_Grid[0].bit()->setPosition(m_Grid[0].getPosition());
	m_Grid[7].setBit(PieceForPlayer(0, ChessPiece::Rook, "assets/chess/w_rook.png"));
	m_Grid[7].bit()->setPosition(m_Grid[7].getPosition());
	m_Grid[56].setBit(PieceForPlayer(1, ChessPiece::Rook, "assets/chess/b_rook.png"));
	m_Grid[56].bit()->setPosition(m_Grid[56].getPosition());
	m_Grid[63].setBit(PieceForPlayer(0, ChessPiece::Rook, "assets/chess/w_rook.png"));
	m_Grid[63].bit()->setPosition(m_Grid[63].getPosition());

	// setup the knights
	m_Grid[8].setBit(PieceForPlayer(1, ChessPiece::Knight, "assets/chess/b_knight.png"));
	m_Grid[8].bit()->setPosition(m_Grid[8].getPosition());
	m_Grid[15].setBit(PieceForPlayer(0, ChessPiece::Knight, "assets/chess/w_knight.png"));
	m_Grid[15].bit()->setPosition(m_Grid[15].getPosition());
	m_Grid[48].setBit(PieceForPlayer(1, ChessPiece::Knight, "assets/chess/b_knight.png"));
	m_Grid[48].bit()->setPosition(m_Grid[48].getPosition());
	m_Grid[55].setBit(PieceForPlayer(0, ChessPiece::Knight, "assets/chess/w_knight.png"));
	m_Grid[55].bit()->setPosition(m_Grid[55].getPosition());

	// setup the bishops
	m_Grid[16].setBit(PieceForPlayer(1, ChessPiece::Bishop, "assets/chess/b_bishop.png"));
	m_Grid[16].bit()->setPosition(m_Grid[16].getPosition());
	m_Grid[23].setBit(PieceForPlayer(0, ChessPiece::Bishop, "assets/chess/w_bishop.png"));
	m_Grid[23].bit()->setPosition(m_Grid[23].getPosition());
	m_Grid[40].setBit(PieceForPlayer(1, ChessPiece::Bishop, "assets/chess/b_bishop.png"));
	m_Grid[40].bit()->setPosition(m_Grid[40].getPosition());
	m_Grid[47].setBit(PieceForPlayer(0, ChessPiece::Bishop, "assets/chess/w_bishop.png"));
	m_Grid[47].bit()->setPosition(m_Grid[47].getPosition());

	// setup the queens
	m_Grid[24].setBit(PieceForPlayer(1, ChessPiece::Queen, "assets/chess/b_queen.png"));
	m_Grid[24].bit()->setPosition(m_Grid[24].getPosition());
	m_Grid[31].setBit(PieceForPlayer(0, ChessPiece::Queen, "assets/chess/w_queen.png"));
	m_Grid[31].bit()->setPosition(m_Grid[31].getPosition());

	// setup the kings
	m_Grid[32].setBit(PieceForPlayer(1, ChessPiece::King, "assets/chess/b_king.png"));
	m_Grid[32].bit()->setPosition(m_Grid[32].getPosition());
	m_Grid[39].setBit(PieceForPlayer(0, ChessPiece::King, "assets/chess/w_king.png"));
	m_Grid[39].bit()->setPosition(m_Grid[39].getPosition());

	// if we have an AI set it up
	if (gameHasAI()) {
		setAIPlayer(_gameOptions.AIPlayer);
	}
	// setup up turns etc.
	startGame();
	GenerateMoves('W');
}

bool Chess::actionForEmptyHolder(BitHolder& holder) {
	return false;
}

bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src) {
	auto& srcSquare = static_cast<ChessSquare&>(src);
	LOG("src {}", LogLevel::INFO, srcSquare.getNotation());
	for (auto& move : _moves) {
		if (move.from == srcSquare.getNotation()) {
			return true;
		}
	}
	return false;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {
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
	notation.push_back('a' + col);
	notation.push_back('8' - row);
	return notation;
}

void Chess::addMoveIfValid(std::vector<Move>& moves, const int fromRow, const int fromColumn, const int toRow, const int toColumn) {
	if (toRow >= 0 && toRow < Chess::BoardSize && toColumn >= 0 && toColumn < Chess::BoardSize) {
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
		while (toRow >= 0 && toRow < Chess::BoardSize && toCol >= 0 && toCol < Chess::BoardSize) {
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
	LOG("forward: {} row: {} col: {}", LogLevel::INFO, forward, row, col);
	if (row + forward >= 0 && row + forward < Chess::BoardSize && pieceNotation(row + forward, col) == "00") {
		addMoveIfValid(moves, row, col, row + forward, col);
		if ((row == 1 && pieceNotation(row + forward * 2, col) == "00") || (row == 6 && pieceNotation(row + forward * 2, col) == "00")) {
			addMoveIfValid(moves, row, col, row + forward * 2, col);
		}
	}

	// now add the attack moves
	if (row + forward >= 0 && row + forward < Chess::BoardSize && col + 1 < Chess::BoardSize) {
		if (pieceNotation(row + forward, col + 1) != "00") {
			addMoveIfValid(moves, row, col, row + forward, col + 1);
		}
	}

	if (row + forward >= 0 && row + forward < Chess::BoardSize && col - 1 >= 0) {
		if (pieceNotation(row + forward, col - 1) != "00") {
			addMoveIfValid(moves, row, col, row + forward, col - 1);
		}
	}

}

void Chess::GenerateMoves(char color) {
	_moves.clear();
	for (int col = 0; col < BoardSize; col++) {
		for (int row = 0; row < BoardSize; row++) {
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
	for (auto& square : m_Grid) {
		square.destroyBit();
	}
}

//
// helper function for the winner check
//
Player* Chess::ownerAt(int index) {
	if (index < 0 || index > m_Grid.size()) {
		return nullptr;
	}
	if (!m_Grid[index].bit()) {
		return nullptr;
	}
	return m_Grid[index].bit()->getOwner();
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
	static std::string s('0', NumberOfSquares);
	return s;
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString() {
	std::stringstream s;
	s << '\n';
	for (int y = 0; y < BoardSize; y++) {
		for (int x = 0; x < BoardSize; x++) {
			const auto* bit = m_Grid[x * BoardSize + y].bit();
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

}

