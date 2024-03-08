#include "Chess.h"

#include "Logger.h"
#include "GameGlobal.h"
#include "Evaluate.h"

#define stupid_board_indexing(x) (x % 8 * 8) + (x / 8)

std::string indexToNotation(int row, int col);

char pieceNotation(ChessSquare& square) {
	auto* bit = square.bit();
	return bit ? static_cast<char>(bit->gameTag()) : '0';
}

char Chess::pieceNotation(int rank, int file) {
	if (rank < 0 || rank >= ChessBoard::Size || file < 0 || file >= ChessBoard::Size) {
		return '0';
	}
	return ::pieceNotation(m_Board.GetSquareAt(rank, file));
}

char pieceNotation(std::string state, int row, int col) {
	return state[row * ChessBoard::Size + col];
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
	m_EnPassantSquare = "-";

	srand((unsigned int)time(0));
	setNumberOfPlayers(2);
	// this allows us to draw the board correctly
	_gameOptions.rowX = ChessBoard::Size;
	_gameOptions.rowY = ChessBoard::Size;
	_gameOptions.AIPlayer = 1;
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
	_moves = GenerateMoves(stateString(), 'W', true);
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

	if (m_EnPassantSquare != "") {
		if (dstSquare.getNotation() == m_EnPassantSquare) {
			if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'P') {
				m_Board.GetSquareAt(dstSquare.getColumn() + 1, dstSquare.getRow()).destroyBit();
			} else if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'p') {
				m_Board.GetSquareAt(dstSquare.getColumn() - 1, dstSquare.getRow()).destroyBit();
			}
		}
	}

	// write the en passant square if the move was a pawn move of 2 squares
	// i have no clue why, but column and row are reversed, but it works so meh
	if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'p' && dstSquare.getColumn() == 3 && srcSquare.getColumn() == 1) {
		m_EnPassantSquare = indexToNotation(dstSquare.getColumn() - 1, dstSquare.getRow());
	} else if (dstSquare.bit() && dstSquare.bit()->gameTag() == 'P' && dstSquare.getColumn() == 4 && srcSquare.getColumn() == 6) {
		m_EnPassantSquare = indexToNotation(dstSquare.getColumn() + 1, dstSquare.getRow());
	} else {
		m_EnPassantSquare = "";
	}

	_moves = GenerateMoves(stateString(), _gameOptions.currentTurnNo & 1 ? 'B' : 'W', true);
}

int notationToIndex(const std::string& notation) {
	return (notation[0] - 'a') * ChessBoard::Size + ('8' - notation[1]);
}

std::string indexToNotation(int row, int col) {
	std::string notation;
	notation.push_back('a' + static_cast<char>(col));
	notation.push_back('8' - static_cast<char>(row));
	return notation;
}

void Chess::addMoveIfValid(std::vector<Move>& moves, const std::string& state, const int fromRow, const int fromColumn, const int toRow, const int toColumn) {
	if (toRow >= 0 && toRow < ChessBoard::Size && toColumn >= 0 && toColumn < ChessBoard::Size) {
		auto fromNotation = ::pieceNotation(state, fromRow, fromColumn);
		auto toNotation = ::pieceNotation(state, toRow, toColumn);

		if (toNotation == '0' || std::isupper(fromNotation) != std::isupper(toNotation)) {
			moves.push_back({ indexToNotation(fromRow, fromColumn), indexToNotation(toRow, toColumn) });
		}
	}
}

void Chess::GenerateBishopMoves(std::vector<Move>& moves, const std::string& state, int row, int col) {
	static const std::vector<std::pair<int, int>> directions = { { +1, +1 }, { +1, -1 }, { -1, +1 }, { -1, -1 } };
	GenerateLinearMoves(moves, state, row, col, directions);
}

void Chess::GenerateRookMoves(std::vector<Move>& moves, const std::string& state, int row, int col) {
	static const std::vector<std::pair<int, int>> directions = { { +1, 0 }, { -1, 0 }, { 0, +1 }, { 0, -1 } };
	GenerateLinearMoves(moves, state, row, col, directions);
}

void Chess::GenerateQueenMoves(std::vector<Move>& moves, const std::string& state, int row, int col) {
	GenerateBishopMoves(moves, state, row, col);
	GenerateRookMoves(moves, state, row, col);
}

void Chess::GenerateKnightMoves(std::vector<Move>& moves, const std::string& state, int row, int col) {
	static const int rowOffsets[] = { -2, -2, +2, +2, +1, +1, -1, -1 };
	static const int colOffsets[] = { +1, -1, +1, -1, +2, -2, +2, -2 };

	for (int i = 0; i < 8; i++) {
		int toRow = row + rowOffsets[i];
		int toCol = col + colOffsets[i];
		addMoveIfValid(moves, state, row, col, toRow, toCol);
	}
}

void Chess::GenerateKingMoves(std::vector<Move>& moves, const std::string& state, int row, int col) {
	static const int rowOffsets[] = { -1, -1, -1,  0,  0, +1, +1, +1 };
	static const int colOffsets[] = { -1,  0, +1, -1, +1, -1,  0, +1 };

	for (int i = 0; i < 8; i++) {
		int toRow = row + rowOffsets[i];
		int toCol = col + colOffsets[i];
		addMoveIfValid(moves, state, row, col, toRow, toCol);
	}

	// castling
	if (::pieceNotation(state, row, col) == 'K') {
		if (m_WhiteCastleKingSide) {
			if (::pieceNotation(state, row, col + 1) == '0' && ::pieceNotation(state, row, col + 2) == '0') {
				if (::pieceNotation(state, row, col + 3) == 'R') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col + 2) });
				}
			}
		}

		if (m_WhiteCastleQueenSide) {
			if (::pieceNotation(state, row, col - 1) == '0' && ::pieceNotation(state, row, col - 2) == '0' && ::pieceNotation(state, row, col - 3) == '0') {
				if (::pieceNotation(state, row, col - 4) == 'R') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col - 2) });
				}
			}
		}
	} else {
		if (m_BlackCastleKingSide) {
			if (::pieceNotation(state, row, col + 1) == '0' && ::pieceNotation(state, row, col + 2) == '0') {
				if (::pieceNotation(state, row, col + 3) == 'r') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col + 2) });
				}
			}
		}

		if (m_BlackCastleQueenSide) {
			if (::pieceNotation(state, row, col - 1) == '0' && ::pieceNotation(state, row, col - 2) == '0' && ::pieceNotation(state, row, col - 3) == '0') {
				if (::pieceNotation(state, row, col - 4) == 'r') {
					moves.push_back({ indexToNotation(row, col), indexToNotation(row, col - 2) });
				}
			}
		}
	}
}

void Chess::GenerateLinearMoves(std::vector<Move>& moves, const std::string& state, int row, int col, std::vector<std::pair<int, int>> directions) {
	for (auto& direction : directions) {
		int toRow = row + direction.first;
		int toCol = col + direction.second;
		while (toRow >= 0 && toRow < ChessBoard::Size && toCol >= 0 && toCol < ChessBoard::Size) {
			if (::pieceNotation(state, toRow, toCol) != '0') {
				addMoveIfValid(moves, state, row, col, toRow, toCol);
				break;
			}
			addMoveIfValid(moves, state, row, col, toRow, toCol);
			toRow += direction.first;
			toCol += direction.second;
		}
	}
}

void Chess::GeneratePawnMoves(std::vector<Move>& moves, const std::string& state, int row, int col, char color) {
	// first add the forward moves
	int forward = color == 'W' ? -1 : 1;
	if (row + forward >= 0 && row + forward < ChessBoard::Size && ::pieceNotation(state, row + forward, col) == '0') {
		addMoveIfValid(moves, state, row, col, row + forward, col);
		if ((row == 1 && ::pieceNotation(state, row + forward * 2, col) == '0') || (row == 6 && ::pieceNotation(state, row + forward * 2, col) == '0')) {
			addMoveIfValid(moves, state, row, col, row + forward * 2, col);
		}
	}

	// now add the attack moves
	if (row + forward >= 0 && row + forward < ChessBoard::Size && col + 1 < ChessBoard::Size) {
		if (::pieceNotation(state, row + forward, col + 1) != '0') {
			addMoveIfValid(moves, state, row, col, row + forward, col + 1);
		}
	}

	if (row + forward >= 0 && row + forward < ChessBoard::Size && col - 1 >= 0) {
		if (::pieceNotation(state, row + forward, col - 1) != '0') {
			addMoveIfValid(moves, state, row, col, row + forward, col - 1);
		}
	}

	// en passant
	if (m_EnPassantSquare != "") {
		int targetRow = row + forward;
		int leftCol = col - 1;
		int rightCol = col + 1;
		if (targetRow == 2 && color == 'W') {
			if (m_EnPassantSquare[0] == 'a' + leftCol && m_EnPassantSquare[1] == '8' - targetRow) {
				addMoveIfValid(moves, state, row, col, targetRow, leftCol);
			}
			if (m_EnPassantSquare[0] == 'a' + rightCol && m_EnPassantSquare[1] == '8' - targetRow) {
				addMoveIfValid(moves, state, row, col, targetRow, rightCol);
			}
		} else if (targetRow == 5 && color == 'B') {
			if (m_EnPassantSquare[0] == 'a' + leftCol && m_EnPassantSquare[1] == '8' - targetRow) {
				addMoveIfValid(moves, state, row, col, targetRow, leftCol);
			}
			if (m_EnPassantSquare[0] == 'a' + rightCol && m_EnPassantSquare[1] == '8' - targetRow) {
				addMoveIfValid(moves, state, row, col, targetRow, rightCol);
			}
		}
	}
}

std::vector<Move> Chess::GenerateMoves(std::string state, char color, bool filterMoves/* = false*/) {
	std::vector<Move> moves;
	state.erase(std::remove(state.begin(), state.end(), '\n'), state.end());
	const bool isUpper = color == 'W';
	for (int col = 0; col < ChessBoard::Size; col++) {
		for (int row = 0; row < ChessBoard::Size; row++) {
			char piece = ::pieceNotation(state, row, col);
			if (piece != '0' && static_cast<bool>(std::isupper(piece)) == isUpper) {
				switch (piece) {
				case 'P':
				case 'p':
					GeneratePawnMoves(moves, state, row, col, color);
					break;
				case 'R':
				case 'r':
					GenerateRookMoves(moves, state, row, col);
					break;
				case 'N':
				case 'n':
					GenerateKnightMoves(moves, state, row, col);
					break;
				case 'B':
				case 'b':
					GenerateBishopMoves(moves, state, row, col);
					break;
				case 'Q':
				case 'q':
					GenerateQueenMoves(moves, state, row, col);
					break;
				case 'K':
				case 'k':
					GenerateKingMoves(moves, state, row, col);
					break;
				}
			}
		}
	}

	if (filterMoves) {
		FilterIllegalMoves(moves, state, color);
	}

	return moves;
}

void Chess::FilterIllegalMoves(std::vector<Move>& moves, std::string state, char color) {
	// find the king square
	int kingRow = -1;
	int kingCol = -1;
	for (int col = 0; col < ChessBoard::Size; col++) {
		for (int row = 0; row < ChessBoard::Size; row++) {
			char piece = ::pieceNotation(state, row, col);
			if (piece == (color == 'W' ? 'K' : 'k')) {
				kingRow = row;
				kingCol = col;
				break;
			}
		}
	}
	int kingPos = kingRow + ChessBoard::Size * kingCol;

	for (auto move = moves.begin(); move != moves.end();) {
		int actualPos = stupid_board_indexing(kingPos);
		bool isLegal = true;

		// make the move
		auto copiedState = state;
		int srcSquare = notationToIndex(move->from);
		int dstSquare = notationToIndex(move->to);
		// Promotion check
		auto dstSquareIndex = stupid_board_indexing(dstSquare);
		auto srcSquareIndex = stupid_board_indexing(srcSquare);
		if (copiedState[srcSquareIndex] == 'k' || copiedState[srcSquareIndex] == 'K') {
			auto intermediate = srcSquareIndex;
			if (std::abs(dstSquareIndex - srcSquareIndex) == 2) {
				intermediate = (dstSquareIndex + srcSquareIndex) / 2;
				copiedState[intermediate] = copiedState[srcSquareIndex];
				copiedState[srcSquareIndex] = '0';
				auto oppositeMoves = GenerateMoves(copiedState, color == 'W' ? 'B' : 'W');
				actualPos = intermediate;
				for (auto& oppositeMove : oppositeMoves) {
					if (stupid_board_indexing(notationToIndex(oppositeMove.to)) == actualPos) {
						isLegal = false;
						break;
					}
				}
			}
			copiedState[dstSquareIndex] = copiedState[intermediate];
			copiedState[intermediate] = '0';
			actualPos = dstSquareIndex;
		} else {
			copiedState[dstSquareIndex] = copiedState[srcSquareIndex];
			copiedState[srcSquareIndex] = '0';
			if (dstSquare % 8 == 0 && copiedState[dstSquareIndex] == 'p') // Black pawn promotion
				copiedState[dstSquareIndex] = 'q';
			if (dstSquare % 8 == 7 && copiedState[dstSquareIndex] == 'P') { // White pawn promotion
				copiedState[dstSquareIndex] = 'Q';
			}
		}
		auto oppositeMoves = GenerateMoves(copiedState, color == 'W' ? 'B' : 'W');
		for (auto& oppositeMove : oppositeMoves) {
			if (stupid_board_indexing(notationToIndex(oppositeMove.to)) == actualPos) {
				isLegal = false;
				break;
			}
		}

		if (isLegal) {
			move++;
		} else {
			move = moves.erase(move);
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
		square.setGameTag('0');
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

	auto globalBoardState = s.substr(s.find(' ') + 1);
	// Parse the current player
	char currentPlayer = globalBoardState[0];
	globalBoardState = globalBoardState.substr(2);
	m_WhiteCastleKingSide = false;
	m_WhiteCastleQueenSide = false;
	m_BlackCastleKingSide = false;
	m_BlackCastleQueenSide = false;
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
		m_EnPassantSquare = globalBoardState.substr(0, 2);
	}
	// ignore the half move clock
	globalBoardState = globalBoardState.substr(globalBoardState.find(' ') + 1);


	// Parse the castling state
	_moves = GenerateMoves(stateString(), static_cast<char>(std::toupper(currentPlayer)), true);
	for (auto move : _moves) LOG("{} to {}", LogLevel::INFO, move.from, move.to);
	_gameOptions.currentTurnNo = currentPlayer == 'W' ? 0 : 1;
}

static int f = 0;
void Chess::updateAI() {
	auto copyState = stateString();
	int bestMoveValue = -999'999;
	Move bestMove;
	copyState.erase(std::remove(copyState.begin(), copyState.end(), '\n'), copyState.end());
	for (auto& move : _moves) {
		f = 0;
		auto state = copyState;
		int srcSquare = notationToIndex(move.from);
		int dstSquare = notationToIndex(move.to);
		state[(dstSquare % 8 * 8) + (dstSquare / 8)] = state[(srcSquare % 8 * 8) + (srcSquare / 8)];
		state[(srcSquare % 8 * 8) + (srcSquare / 8)] = '0';
		int bestValue = -negamax(state, 3, -99'999, 99'999, 1); // replace with negamax call
		LOG("Value {}", LogLevel::INFO, bestValue);
		if (bestValue > bestMoveValue) {
			bestMoveValue = bestValue;
			bestMove = move;
		}
	}
	LOG("{} searches best move {} {}", LogLevel::INFO, f, bestMove.from, bestMove.to);
	if (bestMove.from != "" && bestMove.to != "") {
		// Do the move
		int srcSquare = notationToIndex(bestMove.from);
		int dstSquare = notationToIndex(bestMove.to);
		auto& src = m_Board[srcSquare];
		auto& dst = m_Board[dstSquare];
		auto* bit = src.bit();
		dst.dropBitAtPoint(bit, ImVec2(0, 0));
		src.setBit(nullptr);
		bitMovedFromTo(*bit, src, dst);
	} else {
		LOG("No legal move found", LogLevel::INFO);
	}
}

int Chess::EvaluateBoard(const std::string& state) {
	static const std::map<char, int> scores{
		{ 'P', +100 }, { 'p', -100 },
		{ 'R', +500 }, { 'r', -500 },
		{ 'N', +300 }, { 'n', -300 },
		{ 'B', +300 }, { 'b', -300 },
		{ 'Q', +900 }, { 'q', -900 },
		{ 'K', +2000 }, { 'k', -2000 },
		{ '0', 0 }
	};

	int toReturn = 0;

	for (auto c : state) {
		toReturn += scores.find(c)->second;
	}

	for (int i = 0; i < 64; i++) {
		char piece = state[i];
		int j = FLIP(i);
		switch (piece) {
		case 'N': // Knight
			toReturn += knightTable[j];
			break;
		case 'n':
			toReturn -= knightTable[FLIP(j)];
			break;
		case 'P': // Knight
			toReturn += pawnTable[j];
			break;
		case 'p':
			toReturn -= pawnTable[FLIP(j)];
			break;
		case 'K': // Knight
			toReturn += kingTable[j];
			break;
		case 'k':
			toReturn -= kingTable[FLIP(j)];
			break;
		case 'R': // Knight
			toReturn += rookTable[j];
			break;
		case 'r':
			toReturn -= rookTable[FLIP(j)];
			break;
		case 'Q': // Knight
			toReturn += queenTable[j];
			break;
		case 'q':
			toReturn -= queenTable[FLIP(j)];
			break;
		}
	}

	return toReturn;
}

int Chess::negamax(std::string state, int depth, int alpha, int beta, int color) {
	f++;
	if (depth == 0) return EvaluateBoard(state) * color;

	int bestValue = -99'999;
	auto moves = GenerateMoves(state, color == 1 ? 'W' : 'B', true);
	if (moves.empty()) LOG("No legal moves found", LogLevel::INFO);
	for (const auto& move : moves) {
		auto copiedState = state;
		int srcSquare = notationToIndex(move.from);
		int dstSquare = notationToIndex(move.to);
		// Promotion check
		auto dstSquareIndex = stupid_board_indexing(dstSquare);
		auto srcSquareIndex = stupid_board_indexing(srcSquare);
		copiedState[dstSquareIndex] = copiedState[srcSquareIndex];
		copiedState[srcSquareIndex] = '0';
		if (dstSquare % 8 == 0 && copiedState[dstSquareIndex] == 'p') // Black pawn promotion
			copiedState[dstSquareIndex] = 'q';
		if (dstSquare % 8 == 7 && copiedState[dstSquareIndex] == 'P') { // White pawn promotion
			copiedState[dstSquareIndex] = 'Q';
		}
		bestValue = std::max(bestValue, -negamax(copiedState, depth - 1, -beta, -alpha, -color));
		alpha = std::max(alpha, bestValue);
		if (alpha >= beta) break;
	}
	return bestValue;
}
