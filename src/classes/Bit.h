#pragma once

#include "Sprite.h"

class Player;
class BitHolder;

//
// these aren't used yet but will be used for dragging pieces
//
#define kPickedUpScale 1.2f
#define kPickedUpOpacity 255

enum bitz
{
	kBoardZ = 0,
	kPieceZ = 3,
	kPickupUpZ = 9920,
	kMovingZ = 9930
};

class Bit : public Sprite
{
public:
	Bit() : Sprite() {
		_entityType = EntityBit;
	};

	// helper functions
	bool getPickedUp() { return _pickedUp; }
	void setPickedUp(bool yes);

	// am I in a holder? nullptr if I'm not.
	Entity* getHolder();

	// which player owns me
	Player* getOwner() { return _owner; };
	void setOwner(Player* player) { _owner = player; };

	// game defined game tags
	const int gameTag() const { return _gameTag; };
	void setGameTag(int tag) { _gameTag = tag; };

	// move to a position
	void moveTo(const ImVec2& point);
	void update();
	void setOpacity(float opacity) { (void)opacity; };
	bool getMoving() { return _moving; };

private:
	int _restingZ = 0;
	float _restingTransform = 0.0f;
	bool _pickedUp = false;
	Player* _owner = nullptr;
	int _gameTag = 0;
	ImVec2 _destinationPosition{};
	ImVec2 _destinationStep{};
	bool _moving = false;
};
