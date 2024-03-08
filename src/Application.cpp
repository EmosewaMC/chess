#include "Application.h"
#include "imgui/imgui.h"
#include "classes/Chess.h"

#include "Logger.h"
#include "GameGlobal.h"

namespace GameGlobal {
	std::unique_ptr<Logger> logger = nullptr;
};

namespace {
	Chess game;
	int gameWinner = -1;
}

void ClassGame::GameStartUp() {
	GameGlobal::logger = std::make_unique<Logger>("Chess_" + std::to_string(time(0)) + ".log");
	LOG("Started up game", LogLevel::INFO);
	game.Reset();
	game.setStateString("k7/8/7R/8/8/8/8/1R6 W - - 0 1");
	gameWinner = -1;
}

//
// game render loop
// this is called by the main render loop in main.cpp
//
void ClassGame::RenderGame() {
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::Begin("Settings");
	ImGui::Text("Current Player Number: %d", game.getCurrentPlayer()->playerNumber());
	ImGui::Text("Current Board State: %s", game.stateString().c_str());
	if (game.checkForDraw()) {
		ImGui::Text("Game Over!");
		ImGui::Text("Draw!");
	} else {
		if (gameWinner != -1) {
			ImGui::Text("Game Over!");
			ImGui::Text("Winner: %d", gameWinner);
		}
	}
	if (ImGui::Button("Reset Game")) {
		game.stopGame();
		game.Reset();
		gameWinner = -1;
	}
	ImGui::End();
	if (game.gameHasAI() && game.getCurrentPlayer()->isAIPlayer()) {
		game.updateAI();
	}
	ImGui::Begin("GameWindow");
	game.drawFrame();
	ImGui::End();
}

void ClassGame::EndOfTurn() {
	Player* winner = game.checkForWinner();
	if (winner) {
		gameWinner = winner->playerNumber();
	}
}
