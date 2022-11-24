#include "Pacman.h"
#include <time.h>
#include <sstream>






Player1::Player1(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPlayerFrameTime(250), _cMunchieFrameTime(500)

{
	
	//initialise collectables
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		collectables[i] = new Collectable();
		collectables[i]->_collectableFrameCount = 0;
		collectables[i]->_collectableCurrentFrameTime = 0;
	}

	
	//initialise draggable cherry
	cherry = new Collectable();
	cherry->_rKeyDown = false;
	
	//initialise paused background
	background = new Menu();
	background->_paused = false;
	background->_pKeyDown = false;

	//initialise player
	Worm = new Player();
	Worm->_playerDirection = 0;
		Worm->_playerCurrentFrameTime = 0;
		Worm->_playerFrame = 0;
		Worm->leftOrRight = true;
		Worm->speedMultiplier = 1.0f;
		Worm->dead = false;
		
	//initialise Moving enemys
		for (int i = 0; i < ENEMYCOUNT; i++)
		{
			guy1[i] = new MovingEnemy();
		}
		

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
	delete cherry;
	delete cherry->_collectableBlueTexture;
	delete cherry->_collectableRect;
	delete cherry->_collectablePosition;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete collectables[i]->_collectableBlueTexture;
		delete collectables[i];
		delete collectables[i]->_collectableRect;
	}
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		delete guy1[i];
	}
}

void Player1::LoadContent()
{

	// Load Player
	Worm->_playerTexture = new Texture2D();
	Worm->_playerTexture->Load("Textures/wormcreature2.png", false);
	Worm->_playerPosition = new Vector2(350.0f, 350.0f);
	Worm->_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	
	Texture2D* collectableTexture = new Texture2D();
	collectableTexture->Load("Textures/MunchieInverted.png", false);
	// Load Collectable
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		collectables[i]->_collectableBlueTexture = new Texture2D();
		collectables[i]->_collectableBlueTexture = collectableTexture;
		collectables[i]->_collectableRect = new Rect(0.0f, 0.0f, 12, 12);
		collectables[i]->_collectablePosition = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	//load cherry
	cherry->_collectableBlueTexture = new Texture2D();
	cherry->_collectableBlueTexture->Load("Textures/MunchieInverted.png", true);
	cherry->_collectablePosition = new Vector2(70.0f, 100.0f);
	cherry->_collectableRect = new Rect(0.0f, 0.0f, 12, 12);
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
	//check the keyboard state and mouse state
	Input::KeyboardState* state = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();


	//call the pause method
	CheckPaused(state, Input::Keys::P );
	

	//updates animations based on framerate
	if (!background->_paused)
	{
		Input(elapsedTime, state, mouseState);
		CheckViewportCollision();
		UpdatePlayerAnimation(elapsedTime);

		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			UpdateCollectableAnimation(elapsedTime, i);
		}
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

	//draw cherry
	SpriteBatch::Draw(cherry->_collectableBlueTexture, cherry->_collectablePosition, cherry->_collectableRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

	if (!Worm->dead)
	{
		SpriteBatch::Draw(Worm->_playerTexture, Worm->_playerPosition, Worm->_playerSourceRect); // Draws Pacman
	}

    
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);// Draws String
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

void Player1::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	float playerSpeed = (_cPacmanSpeed * elapsedTime) * Worm->speedMultiplier;
	if (!background->_paused)
	{

		bool playerMoving = false;

		// Checks if WASD keys are pressed and moves the player accordingly
		if (state->IsKeyDown(Input::Keys::D))
		{
			Worm->_playerPosition->X += playerSpeed;
			Worm->_playerDirection = 0;
			playerMoving = true;
		}
		if (state->IsKeyDown(Input::Keys::A))
		{
			Worm->_playerPosition->X -= playerSpeed;
			Worm->_playerDirection = 2;
			playerMoving = true;
		}

		if (state->IsKeyDown(Input::Keys::S))
		{
			Worm->_playerPosition->Y += playerSpeed;
				Worm->_playerDirection = 4;
				playerMoving = true;
		}

		if (state->IsKeyDown(Input::Keys::W))
		{
			Worm->_playerPosition->Y -= playerSpeed;
				Worm->_playerDirection = 1;
				playerMoving = true;
		}

		if (!playerMoving)
		{
			Worm->_playerDirection = 3;
		}


		if (mouseState->LeftButton == Input::ButtonState::PRESSED)
		{
			cherry->_collectablePosition->X = mouseState->X;
			cherry->_collectablePosition->Y = mouseState->Y;
		}

		if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
		{
			Worm->speedMultiplier = 2.0f;
		}
		else
		{
			Worm->speedMultiplier = 1.0f;
		}

		if (state->IsKeyDown(Input::Keys::R))
		{
			if (cherry->_rKeyDown == false)
			{
				cherry->_collectablePosition->X = (rand() % Graphics::GetViewportWidth());
				cherry->_collectablePosition->Y = (rand() % Graphics::GetViewportHeight());
				cherry->_rKeyDown = true;
			}
		
		}
		else
		{
			cherry->_rKeyDown = false;
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



void Player1::UpdateCollectableAnimation(int elapsedTime, int i)
{
	
		collectables[i]->_collectableCurrentFrameTime += elapsedTime;

		if (collectables[i]->_collectableCurrentFrameTime > _cMunchieFrameTime)
		{
			collectables[i]->_collectableFrameCount++;

			if (collectables[i]->_collectableFrameCount >= 2)
				collectables[i]->_collectableFrameCount = 0;
			collectables[i]->_collectableCurrentFrameTime = 0;
		}

		collectables[i]->_collectableRect->Y = collectables[i]->_collectableRect->Height * collectables[i]->_collectableFrameCount;
	
}

void Player1::UpdateEnemy(MovingEnemy*, int elapsedTime)
{

}