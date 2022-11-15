#include "Pacman.h"

#include <sstream>






Player1::Player1(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPlayerFrameTime(250), _cMunchieFrameTime(500)

{
	Worm = new Player();

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		collectables[i] = new Collectable();
		collectables[i]->_munchieFrameCount = 0;
		collectables[i]->_munchieCurrentFrameTime = 0;
	}

	background = new Menu();
	
	
	
	background->_paused = false;
	background->_pKeyDown = false;


	
	Worm->_playerDirection = 0;
		Worm->_playerCurrentFrameTime = 0;
		Worm->_playerFrame = 0;
	
		Worm->leftOrRight = true;
		
		

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Player1::~Player1()
{
	delete Worm->_playerTexture;
	delete Worm->_playerSourceRect;
	
	delete background->_menuBackground;
	delete background->_menuRectangle;
	delete background->_menuStringPosition;
	delete Worm;
	delete background;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete collectables[i]->_collectableBlueTexture;
		delete collectables[i];
		delete collectables[i]->_collectableInvertedTexture;
		delete collectables[i]->_collectableRect;
	}
}

void Player1::LoadContent()
{

	// Load Player
	Worm->_playerTexture = new Texture2D();
	Worm->_playerTexture->Load("Textures/wormcreature2.tga", false);
	Worm->_playerPosition = new Vector2(350.0f, 350.0f);
	Worm->_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	

	// Load Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		collectables[i]->_collectableBlueTexture = new Texture2D();
		collectables[i]->_collectableBlueTexture->Load("Textures/MunchieInverted.png", true);
		collectables[i]->_collectableRect = new Rect(0.0f, 0.0f, 12, 12);
		collectables[i]->_collectablePosition = new Vector2(150.0f, 50.0f);
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Set Menu Paramters
	background->_menuBackground = new Texture2D();
	background->_menuBackground->Load("Textures/Transparency.png", false);
	background->_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	background->_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
}

void Player1::Update(int elapsedTime)
{
	//check the keyboard state to set the paramiter variable for the input method and the check paused method
	Input::KeyboardState* state = Input::Keyboard::GetState();


	//call the pause method
	CheckPaused(state, Input::Keys::P );
	

	//updates animations based on framerate
	if (!background->_paused)
	{
		Input(elapsedTime, state);
		CheckViewportCollision();
		UpdatePlayerAnimation(elapsedTime);
		UpdateCollectableAnimation(elapsedTime);
	}

}



void Player1::Draw(int elapsedTime)
{


	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << Worm->_playerPosition->X << " Y: " << Worm->_playerPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing

	
		// Draw Blue Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(collectables[i]->_collectableBlueTexture, collectables[i]->_collectablePosition, collectables[i]->_collectableRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}


		SpriteBatch::Draw(Worm->_playerTexture, Worm->_playerPosition, Worm->_playerSourceRect); // Draws Pacman
		// Draws String

		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
		if (background->_paused)
		{
			std::stringstream menuStream;
			menuStream << "paused!";

			SpriteBatch::Draw(background->_menuBackground, background->_menuRectangle, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), background->_menuStringPosition, Color::Red);
		}
		SpriteBatch::EndDraw(); // Ends Drawing
	

		
}

//Update methods

void Player1::Input(int elapsedTime, Input::KeyboardState* state)
{
	if (!background->_paused)
	{
		bool isMoving = false;

		// Checks if WASD keys are pressed and moves the player accordingly
		if (state->IsKeyDown(Input::Keys::D))
		{
			Worm->_playerPosition->X += _cPacmanSpeed * elapsedTime;

			Worm->_playerDirection = 0;
			isMoving = true;
			Worm->leftOrRight = true;

		}
		if (state->IsKeyDown(Input::Keys::A))
		{
			Worm->_playerPosition->X -= _cPacmanSpeed * elapsedTime;
			Worm->_playerDirection = 2;
			isMoving = true;
			Worm->leftOrRight = false;
		}

		if (state->IsKeyDown(Input::Keys::S))
		{
			Worm->_playerPosition->Y += _cPacmanSpeed * elapsedTime;
			isMoving = true;
			if (Worm->leftOrRight == true)
				Worm->_playerDirection = 0;
			else
				Worm->_playerDirection = 2;
		}

		if (state->IsKeyDown(Input::Keys::W))
		{
			Worm->_playerPosition->Y -= _cPacmanSpeed * elapsedTime;
			isMoving = true;
			if (Worm->leftOrRight == true)
				Worm->_playerDirection = 0;
			else
				Worm->_playerDirection = 2;
		}

		if (isMoving == false)
		{
			if (Worm->leftOrRight == true)
				Worm->_playerDirection = 1;
			else
				Worm->_playerDirection = 3;
		}
	}
}


 void Player1::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	if (keyboardState->IsKeyDown(pauseKey) && !background->_pKeyDown)
	{
		background->_pKeyDown = true;
		background->_paused = !background->_paused;
	}
	if (keyboardState->IsKeyUp(pauseKey))
	{
		background->_pKeyDown = false;
	}
}



void Player1::CheckViewportCollision()
{
	//collision with screen edges
	if (Worm->_playerPosition->X > Graphics::GetViewportWidth())
		Worm->_playerPosition->X = 0 - Worm->_playerSourceRect->Width;

	if (Worm->_playerPosition->X < 0 - Worm->_playerSourceRect->Width)
		Worm->_playerPosition->X = Graphics::GetViewportWidth();

	if (Worm->_playerPosition->Y > Graphics::GetViewportHeight())
		Worm->_playerPosition->Y = 0 - Worm->_playerSourceRect->Height;

	if (Worm->_playerPosition->Y < 0 - Worm->_playerSourceRect->Height)
		Worm->_playerPosition->Y = Graphics::GetViewportHeight();
}



void Player1::UpdatePlayerAnimation(int elapsedTime)
{
		//runs player animations based on the framerate
	Worm->_playerCurrentFrameTime += elapsedTime;

		if (Worm->_playerCurrentFrameTime > _cPlayerFrameTime)
		{
			Worm->_playerFrame++;
			if (Worm->_playerFrame >= 2)
			{
				Worm->_playerFrame = 0;
			}
			Worm->_playerCurrentFrameTime = 0;
		}

		Worm->_playerSourceRect->Y = Worm->_playerSourceRect->Height * Worm->_playerDirection;
		Worm->_playerSourceRect->X = Worm->_playerSourceRect->Width * Worm->_playerFrame;
}



void Player1::UpdateCollectableAnimation(int elapsedTime)
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		collectables[i]->_munchieCurrentFrameTime += elapsedTime;

		if (collectables[i]->_munchieCurrentFrameTime > _cMunchieFrameTime)
		{
			collectables[i]->_munchieFrameCount++;

			if (collectables[i]->_munchieFrameCount >= 1)
				collectables[i]->_munchieFrameCount = 0;
			collectables[i]->_munchieCurrentFrameTime = 0;
		}

		collectables[i]->_collectableRect->Y = collectables[i]->_collectableRect->Height * collectables[i]->_munchieFrameCount;
	}
}