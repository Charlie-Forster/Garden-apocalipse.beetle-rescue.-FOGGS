#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

struct Player
{
	// Data to represent Player
	Vector2* _playerPosition;
	Rect* _playerSourceRect;
	Texture2D* _playerTexture;
	int _playerDirection;
	int _playerFrame;
	int _playerCurrentFrameTime;
	bool leftOrRight;
};


struct Collectable
{
	// Data to represent Collectable
	int _munchieFrameCount;
	Rect* _collectableRect;
	Texture2D* _collectableBlueTexture;
	Texture2D* _collectableInvertedTexture;
	
	int _munchieFrame;
	int _munchieCurrentFrameTime;

};

struct Menu
{
	//Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;
};

// Declares the Player1 class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Player1 : public Game
{
private:
	
	//create struct instances
	Player* Worm;
	Collectable* collectable;
	Menu* background;

	// Position for String
	Vector2* _stringPosition;

	//Constant data for game variables
	const float _cPacmanSpeed;
	const int _cPlayerFrameTime;
	const int _cMunchieFrameTime;

	
	

	//input methods
	void Input(int elapsedTime, Input::KeyboardState* state);

	//check methods
	void CheckViewportCollision();
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKeys);

	//Update method
	void UpdatePlayerAnimation(int elapsedTime);
	void UpdateCollectableAnimation(int elapsedTime);
public:
	


	/// <summary> Constructs the Player1 class. </summary>
	Player1(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Player1 class. </summary>
	virtual ~Player1();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

};