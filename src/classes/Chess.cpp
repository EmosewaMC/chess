#include "Chess.h"

#include "Logger.h"
#include "GameGlobal.h"

Chess::Chess() {
}

Chess::~Chess() {
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

void Chess::setUpBoard() {
	LOG("", LogLevel::INFO);
	srand((unsigned int)time(0));
	setNumberOfPlayers(2);
	// this allows us to draw the board correctly
	_gameOptions.rowX = BoardSize;
	_gameOptions.rowY = BoardSize;
	// setup the board
	for (int y = 0; y < NumberOfSquares; y++) {
		m_Grid[y].initHolder(ImVec2(100 * static_cast<float>(y / BoardSize) + 100, 100 * static_cast<float>(y % BoardSize) + 100), "assets/chess/boardsquare.png", y % BoardSize, y / BoardSize);
	}

	// int f = 0;
	// for (auto& square : m_Grid) {
	// 	square.setBit(PieceForPlayer(f % 2, "assets/chess/w_pawn.png"));
	// 	square.bit()->setPosition(square.getPosition());
	// 	f++;
	// }

	// setup the pawns
	for (int i = 0; i < BoardSize; i++) {
		m_Grid[i * 8 + 1].setBit(PieceForPlayer(0, 'P', "assets/chess/b_pawn.png"));
		m_Grid[i * 8 + 1].bit()->setPosition(m_Grid[i * 8 + 1].getPosition());
		m_Grid[i * 8 + 6].setBit(PieceForPlayer(1, 'p', "assets/chess/w_pawn.png"));
		m_Grid[i * 8 + 6].bit()->setPosition(m_Grid[i * 8 + 6].getPosition());
	}

	// setup the rooks
	m_Grid[0].setBit(PieceForPlayer(0, 'R', "assets/chess/b_rook.png"));
	m_Grid[0].bit()->setPosition(m_Grid[0].getPosition());
	m_Grid[7].setBit(PieceForPlayer(0, 'r', "assets/chess/w_rook.png"));
	m_Grid[7].bit()->setPosition(m_Grid[7].getPosition());
	m_Grid[56].setBit(PieceForPlayer(1, 'R', "assets/chess/b_rook.png"));
	m_Grid[56].bit()->setPosition(m_Grid[56].getPosition());
	m_Grid[63].setBit(PieceForPlayer(1, 'r', "assets/chess/w_rook.png"));
	m_Grid[63].bit()->setPosition(m_Grid[63].getPosition());

	// setup the knights
	m_Grid[8].setBit(PieceForPlayer(0, 'K', "assets/chess/b_knight.png"));
	m_Grid[8].bit()->setPosition(m_Grid[8].getPosition());
	m_Grid[15].setBit(PieceForPlayer(0, 'k', "assets/chess/w_knight.png"));
	m_Grid[15].bit()->setPosition(m_Grid[15].getPosition());
	m_Grid[48].setBit(PieceForPlayer(1, 'K', "assets/chess/b_knight.png"));
	m_Grid[48].bit()->setPosition(m_Grid[48].getPosition());
	m_Grid[55].setBit(PieceForPlayer(1, 'k', "assets/chess/w_knight.png"));
	m_Grid[55].bit()->setPosition(m_Grid[55].getPosition());

	// setup the bishops
	m_Grid[16].setBit(PieceForPlayer(0, 'B', "assets/chess/b_bishop.png"));
	m_Grid[16].bit()->setPosition(m_Grid[16].getPosition());
	m_Grid[23].setBit(PieceForPlayer(0, 'b', "assets/chess/w_bishop.png"));
	m_Grid[23].bit()->setPosition(m_Grid[23].getPosition());
	m_Grid[40].setBit(PieceForPlayer(1, 'B', "assets/chess/b_bishop.png"));
	m_Grid[40].bit()->setPosition(m_Grid[40].getPosition());
	m_Grid[47].setBit(PieceForPlayer(1, 'b', "assets/chess/w_bishop.png"));
	m_Grid[47].bit()->setPosition(m_Grid[47].getPosition());

	// setup the queens
	m_Grid[24].setBit(PieceForPlayer(0, 'Q', "assets/chess/b_queen.png"));
	m_Grid[24].bit()->setPosition(m_Grid[24].getPosition());
	m_Grid[31].setBit(PieceForPlayer(1, 'q', "assets/chess/w_queen.png"));
	m_Grid[31].bit()->setPosition(m_Grid[31].getPosition());

	// setup the kings
	m_Grid[32].setBit(PieceForPlayer(0, 'K', "assets/chess/b_king.png"));
	m_Grid[32].bit()->setPosition(m_Grid[32].getPosition());
	m_Grid[39].setBit(PieceForPlayer(1, 'k', "assets/chess/w_king.png"));
	m_Grid[39].bit()->setPosition(m_Grid[39].getPosition());

	// if we have an AI set it up
	if (gameHasAI()) {
		setAIPlayer(_gameOptions.AIPlayer);
	}
	// setup up turns etc.
	startGame();
}

bool Chess::actionForEmptyHolder(BitHolder& holder) {
	if (holder.bit()) {
		return false;
	}

	return true;
}

bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src) {

	return false;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) {

	return false;
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
			if (bit) {
				s << (char)bit->gameTag();
			} else {
				s << '0';
			}
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

