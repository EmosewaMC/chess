#include "Chess.h"

#include "Logger.h"
#include "GameGlobal.h"

std::string indexToNotation(int row, int col);

namespace {
	char pieceNotation(ChessSquare& square) {
		auto* bit = square.bit();
		return bit ? static_cast<char>(bit->gameTag()) : '0';
	}
};

char Chess::pieceNotation(int rank, int file) {
	if (rank < 0 || rank >= ChessBoard::Size || file < 0 || file >= ChessBoard::Size) {
		return '0';
	}
	return ::pieceNotation(m_Board.GetSquareAt(rank, file));
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
	m_BlackCastleKingSide = true;
	m_BlackCastleQueenSide = true;
	m_WhiteCastleKingSide = true;
	m_WhiteCastleQueenSide = true;

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
		m_Board[i * 8 + 1].setBit(PieceForPlayer(1, 'p', "assets/chess/b_pawn.png"));
		m_Board[i * 8 + 1].bit()->setPosition(m_Board[i * 8 + 1].getPosition());
		m_Board[i * 8 + 6].setBit(PieceForPlayer(0, 'P', "assets/chess/w_pawn.png"));
		m_Board[i * 8 + 6].bit()->setPosition(m_Board[i * 8 + 6].getPosition());
	}

	// setup the rooks
	m_Board[0].setBit(PieceForPlayer(1, 'r', "assets/chess/b_rook.png"));
	m_Board[0].bit()->setPosition(m_Board[0].getPosition());
	m_Board[7].setBit(PieceForPlayer(0, 'R', "assets/chess/w_rook.png"));
	m_Board[7].bit()->setPosition(m_Board[7].getPosition());
	m_Board[56].setBit(PieceForPlayer(1, 'r', "assets/chess/b_rook.png"));
	m_Board[56].bit()->setPosition(m_Board[56].getPosition());
	m_Board[63].setBit(PieceForPlayer(0, 'R', "assets/chess/w_rook.png"));
	m_Board[63].bit()->setPosition(m_Board[63].getPosition());

	// setup the knights
	m_Board[8].setBit(PieceForPlayer(1, 'n', "assets/chess/b_knight.png"));
	m_Board[8].bit()->setPosition(m_Board[8].getPosition());
	m_Board[15].setBit(PieceForPlayer(0, 'N', "assets/chess/w_knight.png"));
	m_Board[15].bit()->setPosition(m_Board[15].getPosition());
	m_Board[48].setBit(PieceForPlayer(1, 'n', "assets/chess/b_knight.png"));
	m_Board[48].bit()->setPosition(m_Board[48].getPosition());
	m_Board[55].setBit(PieceForPlayer(0, 'N', "assets/chess/w_knight.png"));
	m_Board[55].bit()->setPosition(m_Board[55].getPosition());

	// setup the bishops
	m_Board[16].setBit(PieceForPlayer(1, 'b', "assets/chess/b_bishop.png"));
	m_Board[16].bit()->setPosition(m_Board[16].getPosition());
	m_Board[23].setBit(PieceForPlayer(0, 'B', "assets/chess/w_bishop.png"));
	m_Board[23].bit()->setPosition(m_Board[23].getPosition());
	m_Board[40].setBit(PieceForPlayer(1, 'b', "assets/chess/b_bishop.png"));
	m_Board[40].bit()->setPosition(m_Board[40].getPosition());
	m_Board[47].setBit(PieceForPlayer(0, 'B', "assets/chess/w_bishop.png"));
	m_Board[47].bit()->setPosition(m_Board[47].getPosition());

	// setup the queens
	m_Board[24].setBit(PieceForPlayer(1, 'q', "assets/chess/b_queen.png"));
	m_Board[24].bit()->setPosition(m_Board[24].getPosition());
	m_Board[31].setBit(PieceForPlayer(0, 'Q', "assets/chess/w_queen.png"));
	m_Board[31].bit()->setPosition(m_Board[31].getPosition());

	// setup the kings
	m_Board[32].setBit(PieceForPlayer(1, 'k', "assets/chess/b_king.png"));
	m_Board[32].bit()->setPosition(m_Board[32].getPosition());
	m_Board[39].setBit(PieceForPlayer(0, 'K', "assets/chess/w_king.png"));
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
	auto& srcSquare = static_cast<ChessSquare&>(src);
	auto& dstSquare = static_cast<ChessSquare&>(dst);
	Game::bitMovedFromTo(bit, src, dst);

	// promotion
	if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'P' && dstSquare.getColumn() == 0) {
		dst.setBit(PieceForPlayer(getCurrentPlayer()->playerNumber(), 'Q', "assets/chess/w_queen.png"));
		dst.bit()->setPosition(dst.getPosition());
	}

	if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'p' && dstSquare.getColumn() == 7) {
		dst.setBit(PieceForPlayer(getCurrentPlayer()->playerNumber(), 'q', "assets/chess/b_queen.png"));
		dst.bit()->setPosition(dst.getPosition());
	}

	// white castling
	if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'K') {
		// move the king side rook
		if (m_WhiteCastleKingSide && dstSquare.getRow() == 6) {
			m_Board[47].setBit(PieceForPlayer(getCurrentPlayer()->playerNumber(), 'R', "assets/chess/w_rook.png"));
			m_Board[47].bit()->setPosition(m_Board[47].getPosition());
			m_Board[63].destroyBit();
		}

		if (m_WhiteCastleQueenSide && dstSquare.getRow() == 2) {
			m_Board[31].setBit(PieceForPlayer(getCurrentPlayer()->playerNumber(), 'R', "assets/chess/w_rook.png"));
			m_Board[31].bit()->setPosition(m_Board[31].getPosition());
			m_Board[7].destroyBit();
		}
		m_WhiteCastleKingSide = false;
		m_WhiteCastleQueenSide = false;
	}

	// black side castling
	if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'k') {
		// move the king side rook
		if (m_BlackCastleKingSide && dstSquare.getRow() == 6) {
			m_Board[40].setBit(PieceForPlayer(getCurrentPlayer()->playerNumber(), 'r', "assets/chess/b_rook.png"));
			m_Board[40].bit()->setPosition(m_Board[40].getPosition());
			m_Board[56].destroyBit();
		}

		if (m_BlackCastleQueenSide && dstSquare.getRow() == 2) {
			m_Board[24].setBit(PieceForPlayer(getCurrentPlayer()->playerNumber(), 'r', "assets/chess/b_rook.png"));
			m_Board[24].bit()->setPosition(m_Board[24].getPosition());
			m_Board[0].destroyBit();
		}
		m_BlackCastleKingSide = false;
		m_BlackCastleQueenSide = false;
	}
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
		auto fromNotation = pieceNotation(fromRow, fromColumn);
		auto toNotation = pieceNotation(toRow, toColumn);

		if (toNotation == '0' || std::isupper(fromNotation) != std::isupper(toNotation)) {
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

	// castling
	if (pieceNotation(row, col) == 'K') {
		if (m_WhiteCastleKingSide) {
			if (pieceNotation(row, col + 1) == '0' && pieceNotation(row, col + 2) == '0') {
				if (pieceNotation(row, col + 3) == 'R') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col + 2) });
				}
			}
		}

		if (m_WhiteCastleQueenSide) {
			if (pieceNotation(row, col - 1) == '0' && pieceNotation(row, col - 2) == '0' && pieceNotation(row, col - 3) == '0') {
				if (pieceNotation(row, col - 4) == 'R') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col - 2) });
				}
			}
		}
	} else {
		if (m_BlackCastleKingSide) {
			if (pieceNotation(row, col + 1) == '0' && pieceNotation(row, col + 2) == '0') {
				if (pieceNotation(row, col + 3) == 'r') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col + 2) });
				}
			}
		}

		if (m_BlackCastleQueenSide) {
			if (pieceNotation(row, col - 1) == '0' && pieceNotation(row, col - 2) == '0' && pieceNotation(row, col - 3) == '0') {
				if (pieceNotation(row, col - 4) == 'r') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col - 2) });
				}
			}
		}
	}
}

void Chess::GenerateLinearMoves(std::vector<Move>& moves, int row, int col, std::vector<std::pair<int, int>> directions) {
	for (auto& direction : directions) {
		int toRow = row + direction.first;
		int toCol = col + direction.second;
		while (toRow >= 0 && toRow < ChessBoard::Size && toCol >= 0 && toCol < ChessBoard::Size) {
			if (pieceNotation(toRow, toCol) != '0') {
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
	if (row + forward >= 0 && row + forward < ChessBoard::Size && pieceNotation(row + forward, col) == '0') {
		addMoveIfValid(moves, row, col, row + forward, col);
		if ((row == 1 && pieceNotation(row + forward * 2, col) == '0') || (row == 6 && pieceNotation(row + forward * 2, col) == '0')) {
			addMoveIfValid(moves, row, col, row + forward * 2, col);
		}
	}

	// now add the attack moves
	if (row + forward >= 0 && row + forward < ChessBoard::Size && col + 1 < ChessBoard::Size) {
		if (pieceNotation(row + forward, col + 1) != '0') {
			addMoveIfValid(moves, row, col, row + forward, col + 1);
		}
	}

	if (row + forward >= 0 && row + forward < ChessBoard::Size && col - 1 >= 0) {
		if (pieceNotation(row + forward, col - 1) != '0') {
			addMoveIfValid(moves, row, col, row + forward, col - 1);
		}
	}

}

void Chess::GenerateMoves(char color) {
	_moves.clear();
	const bool isUpper = color == 'W';
	for (int col = 0; col < ChessBoard::Size; col++) {
		for (int row = 0; row < ChessBoard::Size; row++) {
			char piece = pieceNotation(row, col);
			if (piece != '0' && static_cast<bool>(std::isupper(piece)) == isUpper) {
				switch (piece) {
				case 'P':
				case 'p':
					GeneratePawnMoves(_moves, row, col, color);
					break;
				case 'R':
				case 'r':
					GenerateRookMoves(_moves, row, col);
					break;
				case 'N':
				case 'n':
					GenerateKnightMoves(_moves, row, col);
					break;
				case 'B':
				case 'b':
					GenerateBishopMoves(_moves, row, col);
					break;
				case 'Q':
				case 'q':
					GenerateQueenMoves(_moves, row, col);
					break;
				case 'K':
				case 'k':
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
	std::map<char, std::string> pieceTextures = {
		{ 'P', "assets/chess/w_pawn.png" },
		{ 'p', "assets/chess/b_pawn.png" },
		{ 'R', "assets/chess/w_rook.png" },
		{ 'r', "assets/chess/b_rook.png" },
		{ 'N', "assets/chess/w_knight.png" },
		{ 'n', "assets/chess/b_knight.png" },
		{ 'B', "assets/chess/w_bishop.png" },
		{ 'b', "assets/chess/b_bishop.png" },
		{ 'Q', "assets/chess/w_queen.png" },
		{ 'q', "assets/chess/b_queen.png" },
		{ 'K', "assets/chess/w_king.png" },
		{ 'k', "assets/chess/b_king.png" }
	};

	for (auto& square : m_Board) {
		square.destroyBit();
	}

	int rank = 0;
	int file = 0;
	for (auto c : s) {
		if (rank >= 8) {
			break;
		}

		if (c != '/') {
			if (std::isdigit(c)) {
				file += c - '0';
			} else if (pieceTextures.find(c) != pieceTextures.end()) {
				LOG("piece {} is going at {}", LogLevel::INFO, c, rank + file * 8);
				m_Board[rank + file * 8].setBit(PieceForPlayer(c >= 'a' ? 0 : 1, c, pieceTextures[c]));
				m_Board[rank + file * 8].bit()->setPosition(m_Board[rank + file * 8].getPosition());
				file++;
			}
		}

		if (file >= 8) {
			file = 0;
			rank++;
		}
	}

	LOG("{} rest of stream", LogLevel::INFO, s.substr(s.find(' ') + 1));
	auto globalBoardState = s.substr(s.find(' ') + 1);
	// Parse the current player
	char currentPlayer = globalBoardState[0];
	globalBoardState = globalBoardState.substr(2);
	if (globalBoardState[0] == '-') {
		globalBoardState = globalBoardState.substr(2);
	} else {
		// Parse the castling state
		if (globalBoardState[0] == 'K') {
			m_WhiteCastleKingSide = true;
			globalBoardState = globalBoardState.substr(1);
		} else {
			m_WhiteCastleKingSide = false;
		}

		if (globalBoardState[0] == 'Q') {
			m_WhiteCastleQueenSide = true;
			globalBoardState = globalBoardState.substr(1);
		} else {
			m_WhiteCastleQueenSide = false;
		}

		if (globalBoardState[0] == 'k') {
			m_BlackCastleKingSide = true;
			globalBoardState = globalBoardState.substr(1);
		} else {
			m_BlackCastleKingSide = false;
		}

		if (globalBoardState[0] == 'q') {
			m_BlackCastleQueenSide = true;
			globalBoardState = globalBoardState.substr(1);
		} else {
			m_BlackCastleQueenSide = false;
		}
	}
	if (globalBoardState[0] == ' ') globalBoardState = globalBoardState.substr(1);
	
	// need to parse the en passant state
	if (globalBoardState[0] == '-') {
		globalBoardState = globalBoardState.substr(2);
	} else {
		// parse state based on algebraic notation of the square that can be taken.
	}

	// ignore the half move clock
	globalBoardState = globalBoardState.substr(globalBoardState.find(' ') + 1);

	LOG("Cur ({})", LogLevel::INFO, currentPlayer);

	// Parse the castling state

	GenerateMoves(std::toupper('W'));
}

