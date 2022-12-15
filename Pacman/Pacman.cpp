#include "Garden Apocalypse.h"
#include <time.h>
#include <sstream>
#include <iostream>






Player1::Player1(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPlayerFrameTime(250), _cMunchieFrameTime(500), _cEnemyFrameTime(120)

{
	//initialise audio
	_pop = new SoundEffect();
	_death = new SoundEffect();
	_cherryMove = new SoundEffect();


	srand(time(NULL));
	//initialise collectables

	collectables = new Collectable[MUNCHIECOUNT];
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		collectables[i]._collectableFrameCount = 0;
		collectables[i]._collectableCurrentFrameTime = 0;
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
		Worm->_collectableCount = 0;
		
	//initialise Moving enemys
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		enemy1[i] = new MovingEnemy();
		enemy1[i]->xDirection = rand() % 2;
		enemy1[i]->yDirection = rand() % 2;
		enemy1[i]->speed = 0.2f;
		enemy1[i]->_enemyCurrentFrameTime = rand() % 100;

	}
	

	//Initialise important Game aspects
	Audio::Initialise();
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
		delete collectables[i]._collectableBlueTexture;
		delete collectables;
		delete collectables[i]._collectableRect;
	}
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		delete enemy1[i];
		delete enemy1[i]->texture;
		delete enemy1[i]->sourceRect;
		delete enemy1[i]->position;
	}
	delete _pop;
	delete _death;
	delete _gameBackground;
	delete _cherryMove;
}

void Player1::LoadContent()
{
	//load audio
	_pop->Load("Audio/pop.wav");
	_death->Load("Audio/matches.wav"); //https://www.soundjay.com/nature/sounds/matches-1.mp3 remember to cite the source
	_cherryMove->Load("Audio/wscream_2.wav"); //https://opengameart.org/content/girly-scream // remember to cite

	// Load Player
	Worm->_playerTexture = new Texture2D();
	Worm->_playerTexture->Load("Textures/wormcreature2.png", false);
	Worm->_playerPosition = new Vector2(350.0f, 350.0f);
	Worm->_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	
	Texture2D* collectableTexture = new Texture2D();
	collectableTexture->Load("Textures/bug.png", false);
	// Load Collectable
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		collectables[i]._collectableBlueTexture = new Texture2D();
		collectables[i]._collectableBlueTexture = collectableTexture;
		collectables[i]._collectableRect = new Rect(0.0f, 0.0f, 12, 12);
		collectables[i]._collectablePosition = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		if (collectables[i]._collectablePosition->X > 900)
		{
			collectables[i]._collectablePosition->X -= 100;
		}
		if (collectables[i]._collectablePosition->X < 100)
		{
			collectables[i]._collectablePosition->X += 100;
		}
	}
	
	//load gameplay background
	_gameBackground = new Texture2D();
	_gameBackground->Load("Textures/grund.png", false);
	_gameBackgroundRect = new Rect(0.0f, 0.0f, 1024, 768);
	_gamebackgroundPosition = new Vector2(0,0);

	//load cherry
	cherry->_collectableBlueTexture = new Texture2D();
	cherry->_collectableBlueTexture->Load("Textures/cherry.png", true);
	cherry->_collectablePosition = new Vector2(70.0f, 100.0f);
	cherry->_collectableRect = new Rect(0.0f, 0.0f, 32, 32);
	// Set string positions
	_stringPosition = new Vector2(100.0f, 25.0f);
	_bugStringPosition = new Vector2(110.0f, 50.0f);


	//Set Menu Paramters
	background->_menuBackground = new Texture2D();
	background->_menuBackground->Load("Textures/Transparency.png", false);
	background->_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	background->_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	//load enemy
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		enemy1[i]->texture = new Texture2D;
		enemy1[i]->texture->Load("Textures/fire.png", false);
		enemy1[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		enemy1[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}

	//load death animation
	Worm->_deathAnimation = new Texture2D;
	Worm->_deathAnimation->Load("Textures/death.png", false);
	
}

void Player1::Update(int elapsedTime)
{
	//check the keyboard state and mouse state
	Input::KeyboardState* state = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	
	if (Worm->_collectableCount >= MUNCHIECOUNT)
	{
		background->_pKeyDown = true;
		background->_paused = true;
	}
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

			UpdateEnemy(enemy1[0], elapsedTime);
			CheckEnemyCollisions();
			UpdateEnemyAnimations(elapsedTime);
			CheckCollectableCollisions();

			
	}

}



void Player1::Draw(int elapsedTime)
{


	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Player X: " << Worm->_playerPosition->X << " Y: " << Worm->_playerPosition->Y;
	std::stringstream bugsCollectedstr;
	bugsCollectedstr << " Bugs collected: " << Worm->_collectableCount;

	SpriteBatch::BeginDraw(); // Starts Drawing

	SpriteBatch::Draw(_gameBackground, _gameBackgroundRect, nullptr);

		// Draw Blue Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(collectables[i]._collectableBlueTexture, collectables[i]._collectablePosition, collectables[i]._collectableRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
	}

	//draw cherry
	SpriteBatch::Draw(cherry->_collectableBlueTexture, cherry->_collectablePosition, cherry->_collectableRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

	//draw player
	if (!Worm->dead)
	{
		SpriteBatch::Draw(Worm->_playerTexture, Worm->_playerPosition, Worm->_playerSourceRect); // Draws Pacman
	}
	else
	{
		SpriteBatch::Draw(Worm->_deathAnimation, Worm->_playerPosition, Worm->_playerSourceRect);
	}
	//draw enemies
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		SpriteBatch::Draw(enemy1[i]->texture, enemy1[i]->position, enemy1[i]->sourceRect);
	}

	
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::White);// Draws String
	SpriteBatch::DrawString(bugsCollectedstr.str().c_str(), _bugStringPosition, Color::Green);
		if (background->_paused)
		{
			std::stringstream menuStream;
			
			menuStream << "paused!";
			if (Worm->_collectableCount >= MUNCHIECOUNT)
			{
				menuStream.str("You Win!");
			}
			SpriteBatch::Draw(background->_menuBackground, background->_menuRectangle, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), background->_menuStringPosition, Color::White);
		}

		
		SpriteBatch::EndDraw(); // Ends Drawing

		
}

//Update methods

void Player1::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState)
{
	float playerSpeed = (_cPacmanSpeed * elapsedTime) * Worm->speedMultiplier;
	if ((!background->_paused) && (!Worm->dead))
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
				Audio::Play(_cherryMove);
				
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
	 {
		 Worm->_playerPosition->X = 0 - Worm->_playerSourceRect->Width;
	 }
	 else if (Worm->_playerPosition->X < 0 - Worm->_playerSourceRect->Width)
	 {
		 Worm->_playerPosition->X = Graphics::GetViewportWidth();
	 }

	 if (Worm->_playerPosition->Y > Graphics::GetViewportHeight())
	 {
		 Worm->_playerPosition->Y = 0 - Worm->_playerSourceRect->Height;
	 }
	 else if (Worm->_playerPosition->Y < 0 - Worm->_playerSourceRect->Height)
	 {
		 Worm->_playerPosition->Y = Graphics::GetViewportHeight();
	 }
}



void Player1::UpdatePlayerAnimation(int elapsedTime)
{
		//runs player animations based on the framerate
	Worm->_playerCurrentFrameTime += elapsedTime;
	int animationFrames;
	if (Worm->dead == true)
	{
		animationFrames = 5;
	}
	else
	{
		animationFrames = 2;
	}

		if (Worm->_playerCurrentFrameTime > _cPlayerFrameTime)
		{
			if (Worm->_playerFrame < 4)
			{
				Worm->_playerFrame++;
				if (Worm->_playerFrame >= animationFrames)
				{
					Worm->_playerFrame = 0;
				}
			}
			Worm->_playerCurrentFrameTime = 0;
		}
		if (!Worm->dead)
		{
			Worm->_playerSourceRect->Y = Worm->_playerSourceRect->Height * Worm->_playerDirection;
		}
		Worm->_playerSourceRect->X = Worm->_playerSourceRect->Width * Worm->_playerFrame;
}



void Player1::UpdateCollectableAnimation(int elapsedTime, int i)
{
	
		collectables[i]._collectableCurrentFrameTime += elapsedTime;

		if (collectables[i]._collectableCurrentFrameTime > _cMunchieFrameTime)
		{
			collectables[i]._collectableFrameCount++;

			if (collectables[i]._collectableFrameCount >= 2)
				collectables[i]._collectableFrameCount = 0;
			collectables[i]._collectableCurrentFrameTime = 0;
		}

		collectables[i]._collectableRect->Y = collectables[i]._collectableRect->Height * collectables[i]._collectableFrameCount;
	
}

void Player1::UpdateEnemy(MovingEnemy*, int elapsedTime)
{
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		if (enemy1[i]->xDirection == 0)
		{
			enemy1[i]->position->X += enemy1[i]->speed * elapsedTime; //moves right
		}
		else if (enemy1[i]->xDirection == 1)
		{
			enemy1[i]->position->X -= enemy1[i]->speed * elapsedTime; //moves left
		}
		if (enemy1[i]->yDirection == 0)
		{
			enemy1[i]->position->Y += enemy1[i]->speed * elapsedTime;//move down
		}
		else if (enemy1[i]->yDirection == 1)
		{
			enemy1[i]->position->Y -= enemy1[i]->speed * elapsedTime;//move up
		}

		
		
			if (enemy1[i]->position->X + enemy1[i]->sourceRect->Width >= Graphics::GetViewportWidth())
			{
				enemy1[i]->xDirection = 1; //change direction
			}
			else if (enemy1[i]->position->X <= 0)
			{
				enemy1[i]->xDirection = 0; //change direction
			}

			if (enemy1[i]->position->Y <= 0)
			{
				enemy1[i]->yDirection = 0;//change direction
			}
			else if (enemy1[i]->position->Y + enemy1[i]->sourceRect->Height >= Graphics::GetViewportHeight())
			{
				enemy1[i]->yDirection = 1;//change direction
			}
		
	}
}

void Player1::CheckEnemyCollisions()
{
	int i = 0;

	int pLeft = Worm->_playerPosition->X;
	int pRight = Worm->_playerPosition->X + Worm->_playerSourceRect->Width;
	int pTop = Worm->_playerPosition->Y;
	int pBottom = Worm->_playerPosition->Y + Worm->_playerSourceRect->Height;

	int eLeft = 0;
	int eRight = 0;
	int eTop = 0;
	int eBottom = 0;

	for (i = 0; i < ENEMYCOUNT; i++)
	{
		eLeft = enemy1[i]->position->X;
		eRight = enemy1[i]->position->X + enemy1[i]->sourceRect->Width;
		eTop = enemy1[i]->position->Y;
		eBottom = enemy1[i]->position->Y + enemy1[i]->sourceRect->Height;

		if ((pLeft < eRight) && (pRight > eLeft) && (pBottom > eTop) && (pTop < eBottom))
		{
			if (Worm->dead == false)
			{
				Audio::Play(_death);
			}
			Worm->dead = true;
		}
	}
}

void Player1::CheckCollectableCollisions()
{
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		int pLeft = Worm->_playerPosition->X;
		int pRight = Worm->_playerPosition->X + Worm->_playerSourceRect->Width;
		int pTop = Worm->_playerPosition->Y;
		int pBottom = Worm->_playerPosition->Y + Worm->_playerSourceRect->Width;

		int mLeft = 0;
		int mRight = 0;
		int mTop = 0;
		int mBottom = 0;

		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			mLeft = collectables[i]._collectablePosition->X;
			mRight = collectables[i]._collectablePosition->X + collectables[i]._collectableRect->Width;
			mTop = collectables[i]._collectablePosition->Y;
			mBottom = collectables[i]._collectablePosition->Y + collectables[i]._collectableRect->Height;

			if ((pLeft < mRight) && (pRight > mLeft) && (pBottom > mTop) && (pTop < mBottom))
			{
				Audio::Play(_pop);
				collectables[i]._collectablePosition->Y = Graphics::GetViewportHeight() + 40;
				Worm->_collectableCount++;
			}
		}
    }
}

void Player1::UpdateEnemyAnimations(int elapsedTime)
{
	for (int i = 0; i < ENEMYCOUNT; i++)
	{
		
		enemy1[i]->_enemyCurrentFrameTime += elapsedTime;

		if (enemy1[i]->_enemyCurrentFrameTime > _cEnemyFrameTime)
		{
			enemy1[i]->_enemyFrameCount++;

			if (enemy1[i]->_enemyFrameCount >= 2)
				enemy1[i]->_enemyFrameCount = 0;
			enemy1[i]->_enemyCurrentFrameTime = 0;
		}

		enemy1[i]->sourceRect->X = enemy1[i]->sourceRect->Width * enemy1[i]->_enemyFrameCount;
	}
}