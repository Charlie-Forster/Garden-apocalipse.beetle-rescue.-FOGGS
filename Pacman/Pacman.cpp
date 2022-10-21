#include "Pacman.h"

#include <sstream>



Player1::Player1(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f)
{
	_frameCount = 0;
	_paused = false;
	_pKeyDown = false;
	

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Player1::~Player1()
{
	delete _playerTexture;
	delete _playerSourceRect;
	delete _collectableBlueTexture;
	delete _collectableInvertedTexture;
	delete _collectableRect;
	delete _menuBackground;
	delete _menuRectangle;
	delete _menuStringPosition;
}

void Player1::LoadContent()
{

	// Load Player
	_playerTexture = new Texture2D();
	_playerTexture->Load("Textures/wormcreature.tga", false);
	_playerPosition = new Vector2(350.0f, 350.0f);
	_playerSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	

	// Load Munchie
	_collectableBlueTexture = new Texture2D();
	_collectableBlueTexture->Load("Textures/Munchie.tga", true);
	_collectableInvertedTexture = new Texture2D();
	_collectableInvertedTexture->Load("Textures/MunchieInverted.tga", true);
	_collectableRect = new Rect(100.0f, 450.0f, 12, 12);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Set Menu Paramters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
}

void Player1::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();


	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	if (keyboardState->IsKeyUp(Input::Keys::P))
	{
		_pKeyDown = false;
	}

	if (!_paused)
	{
		

		// Checks if WASD keys are pressed and moves the player accordingly
		if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			_playerPosition->X += _cPacmanSpeed * elapsedTime;
			
		}
		if (keyboardState->IsKeyDown(Input::Keys::A))
			_playerPosition->X -= _cPacmanSpeed * elapsedTime;

		if (keyboardState->IsKeyDown(Input::Keys::S))
			_playerPosition->Y += _cPacmanSpeed * elapsedTime;

		if (keyboardState->IsKeyDown(Input::Keys::W))
			_playerPosition->Y -= _cPacmanSpeed * elapsedTime;




		//collision with screen edges
		if (_playerPosition->X > Graphics::GetViewportWidth())
			_playerPosition->X = 0 - _playerSourceRect->Width;

		if (_playerPosition->X < 0 - _playerSourceRect->Width)
			_playerPosition->X = Graphics::GetViewportWidth();

		if (_playerPosition->Y > Graphics::GetViewportHeight())
			_playerPosition->Y = 0 - _playerSourceRect->Height;

		if (_playerPosition->Y < 0 - _playerSourceRect->Height)
			_playerPosition->Y = Graphics::GetViewportHeight();
	}
}

void Player1::Draw(int elapsedTime)
{
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _playerPosition->X << " Y: " << _playerPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	

		if (_frameCount < 30)
		{
			// Draws Red Munchie
			SpriteBatch::Draw(_collectableInvertedTexture, _collectableRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
			
			if (!_paused)
			{
				if (keyboardState->IsKeyDown(Input::Keys::D))
				{
					_playerSourceRect->Y = 0;
					_playerSourceRect->X = 32;

				}
				else if (keyboardState->IsKeyDown(Input::Keys::A))
				{
					_playerSourceRect->Y = 64;
					_playerSourceRect->X = 32;
				}
				else if (keyboardState->IsKeyDown(Input::Keys::S))
				{
					_playerSourceRect->Y = 32;
					_playerSourceRect->X = 0;
				}

				else if (keyboardState->IsKeyDown(Input::Keys::W))
				{
					_playerSourceRect->Y = 32;
					_playerSourceRect->X = 0;
				}
				else
				{
					_playerSourceRect->Y = 0;
					_playerSourceRect->X = 0;
				}

				_frameCount++;
			}
		}
		else
		{
			// Draw Blue Munchie
			SpriteBatch::Draw(_collectableBlueTexture, _collectableRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);

			if (!_paused)
			{
				if (keyboardState->IsKeyDown(Input::Keys::D))
				{
					_playerSourceRect->Y = 0;
					_playerSourceRect->X = 0;

				}
				else if (keyboardState->IsKeyDown(Input::Keys::A))
				{
					_playerSourceRect->Y = 64;
					_playerSourceRect->X = 0;
				}
				else if (keyboardState->IsKeyDown(Input::Keys::S))
				{
					_playerSourceRect->Y = 32;
					_playerSourceRect->X = 0;
				}

				else if (keyboardState->IsKeyDown(Input::Keys::W))
				{
					_playerSourceRect->Y = 32;
					_playerSourceRect->X = 0;
				}
				else
				{
					_playerSourceRect->Y = 0;
					_playerSourceRect->X = 0;
				}


				_frameCount++;
			}

			if (_frameCount >= 60)
				_frameCount = 0;
		}

	SpriteBatch::Draw(_playerTexture, _playerPosition, _playerSourceRect); // Draws Pacman
	// Draws String

	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "paused!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	SpriteBatch::EndDraw(); // Ends Drawing
}