#include "Application.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"


Application::Application()
{

}

Application::~Application()
{

}

void Application::Run()
{
	InitWindow(m_windowWidth, m_windowHeight, "Pac Snake");

	SetTargetFPS(60);
	Load();

	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		float dt = GetFrameTime();
		Update(dt);
		Draw();
	}

	Unload();
	CloseWindow();
}

void Application::Load()
{
	//get a consistant variable for any for loops
	Size = m_cols * m_rows;

	//make all the tiles for trhe map
	Tiles = new Tile[Size];

	//get a random position for the player
	int PlayerPos = RandomInt(0, Size);

	//fill each tile
	for (int i = 0; i < Size; i++)
	{
		//get the correct xy pos for each tile
		int x = i % m_cols;
		int y = i / m_cols;

		Tiles[i].X = x;
		Tiles[i].Y = y;

		//set the fil state
		Tiles[i].FState = FEmpty;
		Tiles[i].EState = EEmpty;
		Tiles[i].PState = PEmpty;

		//Spawn the Player
		if (i == PlayerPos)
		{
			Tiles[i].PState = PPlayer;
			PlayerBody.push_back(Player());
			PlayerBody.at(PlayerBody.size() - 1).X = x;
			PlayerBody.at(PlayerBody.size() - 1).Y = y;

			PlayerBody.at(PlayerBody.size() - 1).PreX = x;
			PlayerBody.at(PlayerBody.size() - 1).PreY = y;

			if (PlayerBody.size() == 0)
			{
				PlayerBody.at(PlayerBody.size()).Head = true;
				PlayerBody.at(PlayerBody.size()).DirectionInput = Up;
			}
		}
		//Create a wall if on the left or right of the screen
		if (x == 0 || x == m_cols - 1)
		{
			Tiles[i].FState = FWall;
		}
		//Create a wall if on the top or bottom of the screen
		else if (y == 0 || y == m_rows - 1)
		{
			Tiles[i].FState = FWall;
		}
		//Create check if a random wall, player, coin or enemy should be created
		else if (Tiles[i].FState == FEmpty)
		{
			//Wall
			if (RandomBool(1, 6))
			{
				if (PlayerPos != i && Tiles[i].FState == FEmpty)
				{
					Tiles[i].FState = FWall;
				}
			}
			//Coin
			else if (RandomBool(1, 8))
			{
				if (PlayerPos != i && Tiles[i].FState == FEmpty)
				{
					Tiles[i].FState = FCoin;
				}
			}
			//Enemy
			else if (RandomBool(1, 30))
			{
				if (PlayerPos != i && Tiles[i].FState == FEmpty)
				{
					Tiles[i].EState = EEnemy;
					Enemys.push_back(Enemey());
					Enemys.at(Enemys.size() - 1).X = x;
					Enemys.at(Enemys.size() - 1).Y = y;
				}
			}
		}
	}
}

void Application::Unload()
{

}

void Application::Update(float dt)
{
	//check if the player has won or lost
	if (PlayerBody.size() > WinAmount)
	{
		WinGame();
	}
	else if (PlayerBody.size() == 1 && GameStarted == false)
	{
		LoseGame();
	}

	//control the players turn timer and allow player to move if 0
	if (PTurnTimer <= 0)
	{
		PTurn = true;
		PTurnTimer = PTurnTimerMax;
	}
	else
	{
		PTurnTimer -= 1 * GetFrameTime();
		PTurn = false;
	}

	//control the enemy turn timer and allow enemy to move if 0
	if (ETurnTimer <= 0)
	{
		ETurn = true;
		ETurnTimer = ETurnTimerMax;
	}
	else
	{
		ETurnTimer -= 1 * GetFrameTime();
		ETurn = false;
	}

	//run the code to move the enemys
	for (int i = 0; i < Enemys.size(); i++)
	{
		// MoveObject (is it the player? 
		//what is it's position in the vector? 
		//what direction to move if it is the player (x,y)?)
		MoveObject(false, i, 0, 0);
	}

	//Player input for player head
	MovePlayer();

	//Update Positions 
	for (int i = 0; i < Size; i++)
	{
		//check if the player or enemy's position has alrady been set to this tile
		//if not this tile has no players or enemys
		bool PSet = false;
		bool ESet = false;

		//if any enemys are at the same x,y pos than they are on this tile
		for (int y = 0; y < Enemys.size(); y++)
		{
			if (Tiles[i].X == Enemys.at(y).X && Tiles[i].Y == Enemys.at(y).Y)
			{
				//set this tile's enemy state to enemy instead of empty
				Tiles[i].EState = EEnemy;
				ESet = true;
			}
		}

		//if any players are at the same x,y pos than they are on this tile
		for (int y = 0; y < PlayerBody.size(); y++)
		{
			if (Tiles[i].X == PlayerBody.at(y).X && Tiles[i].Y == PlayerBody.at(y).Y)
			{
				//set this tile's players state to player instead of empty
				Tiles[i].PState = PPlayer;
				PSet = true;
			}
		}

		//set the states to empty if these values are not set to true
		if (PSet == false)
		{
			Tiles[i].PState = PEmpty;
		}

		if (ESet == false)
		{
			Tiles[i].EState = EEmpty;
		}

		//Collisions
		if (Tiles[i].FState == FEmpty)
		{
			//if it is the players turn
			if (PTurn)
			{
				//if a enemy and player are on this tile
				if (Tiles[i].EState == EEnemy && Tiles[i].PState == PPlayer)
				{
					//is the player body length is greater than 1
					if (PlayerBody.size() > 1)
					{
						for (int x = 0; x < PlayerBody.size(); x++)
						{
							if (Tiles[i].X == PlayerBody.at(x).X && Tiles[i].Y == PlayerBody.at(x).Y)
							{
								//if this is the player head than add to the body, if not than reduce the player size
								if (PlayerBody.at(x).Head)
								{
									PlayerBody.pop_back();
								}
								else 
								{
									PlayerBody.push_back(Player());
									PlayerBody.at(PlayerBody.size() - 1).X = PlayerBody.at(PlayerBody.size() - 2).PreX;
									PlayerBody.at(PlayerBody.size() - 1).Y = PlayerBody.at(PlayerBody.size() - 2).PreY;
								}
							}
						}
					}
					//if player body length is less than one
					else 
					{
						for (int x = 0; x < PlayerBody.size(); x++)
						{
							if (Tiles[i].X == PlayerBody.at(x).X && Tiles[i].Y == PlayerBody.at(x).Y)
							{
								//if this is the player head than add to the body, if not than reduce the player size
								if (!PlayerBody.at(x).Head)
								{
									PlayerBody.push_back(Player());
									PlayerBody.at(PlayerBody.size() - 1).X = PlayerBody.at(PlayerBody.size() - 2).PreX;
									PlayerBody.at(PlayerBody.size() - 1).Y = PlayerBody.at(PlayerBody.size() - 2).PreY;
								}
							}
						}
					}

					//the player has collided with a enemy
					//if we have enemys
					if (Enemys.size() > 0)
					{
						for (int x = 0; x < Enemys.size(); x++)
						{
							if (Tiles[i].X == Enemys.at(x).X && Tiles[i].Y == Enemys.at(x).Y)
							{
								//remove the enemy we hit
								Enemys.erase(Enemys.begin() + x);
							}
						}
					}
				}
			}
		}

		//if we hit a coint then make the player body longer and remove the coin
		if (Tiles[i].FState == FCoin && Tiles[i].PState == PPlayer)
		{
			GameStarted = false;
			PlayerBody.push_back(Player());

			PlayerBody.at(PlayerBody.size() - 1).X = PlayerBody.at(PlayerBody.size() - 2).PreX;
			PlayerBody.at(PlayerBody.size() - 1).Y = PlayerBody.at(PlayerBody.size() - 2).PreY;

			Tiles[i].FState = FEmpty;
		}
	}
}

void Application::Draw()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);
	int tileWidth = m_windowWidth / m_cols;
	int tileHeight = m_windowHeight / m_rows;

	// draw horizontal gridlines
	for (int y = 1; y < m_rows; y++)
	{
		DrawLine(0, y * tileHeight, m_windowWidth, y * tileHeight, LIGHTGRAY);
	}

	// draw vertical gridlines
	for (int x = 1; x < m_cols; x++)
	{
		DrawLine(x * tileWidth, 0, x * tileWidth, m_windowHeight, LIGHTGRAY);
	}

	// Draw the Tiles
	for (int i = 0; i < Size; i++)
	{
		//draw the walls, coins, players and enemys if they are on this tile
		if (Tiles[i].FState == FWall)
		{
			DrawRectangle(Tiles[i].X * tileWidth, Tiles[i].Y * tileHeight, tileWidth, tileHeight, DARKGRAY);
		}

		if (Tiles[i].FState == FCoin) 
		{
			DrawCircle(Tiles[i].X * tileWidth + tileWidth / 2, Tiles[i].Y * tileHeight + tileHeight / 2, 5, GOLD);
		}

		if (Tiles[i].PState == PPlayer)
		{
			//use what player this is as the number drawn on them
			for (int x = 0; x < PlayerBody.size(); x++)
			{
				if (Tiles[i].X == PlayerBody.at(x).X && Tiles[i].Y == PlayerBody.at(x).Y)
				{
					DrawCircle(Tiles[i].X * tileWidth + tileWidth / 2, Tiles[i].Y * tileHeight + tileHeight / 2, 8, GREEN);
					DrawText(std::to_string(x).c_str(), Tiles[i].X * tileWidth + tileWidth / 2, Tiles[i].Y * tileHeight + tileHeight / 2, 5, BLACK);
				}
			}
		}

		if (Tiles[i].EState == EEnemy)
		{
			DrawCircle(Tiles[i].X * tileWidth + tileWidth / 2, Tiles[i].Y * tileHeight + tileHeight / 2, 6, RED);
		}
	}

	//draw the win or lose text if we win
	if (Win) 
	{
		DrawRectangle(m_windowWidth / 3, m_windowHeight / 2.01f, 250, 50, GRAY);
		DrawText("You win!", m_windowWidth / 2.73f, m_windowHeight / 2, 50, BLACK);
	}
	else if (Lose) 
	{
		DrawRectangle(m_windowWidth / 3, m_windowHeight / 2.01f, 300, 50, GRAY);
		DrawText("You Lose!", m_windowWidth / 2.73f, m_windowHeight / 2, 50, BLACK);
	}
	EndDrawing();
}

void Application::MoveObject(bool IsPlayer, int WhichOne, int XDir, int YDir)
{
	//if we haven't won or lost and this is the player
	if (IsPlayer && !Lose && !Win)
	{
		//go through every tile and check if the player is next to this tile 
		for (int i = 0; i < Size; i++)
		{
			if (Tiles[i].FState != FWall && Tiles[i].PState != PPlayer)
			{
				if (Tiles[i].Y == PlayerBody.at(WhichOne).Y + YDir && Tiles[i].X == PlayerBody.at(WhichOne).X + XDir)
				{
					StopMoving = false;

					PlayerBody.at(WhichOne).PreX = PlayerBody.at(WhichOne).X;
					PlayerBody.at(WhichOne).PreY = PlayerBody.at(WhichOne).Y;

					//if we are trying to move in the y direction and the tile is not a wall then move the player
					if (YDir != 0)
					{
						if (PTurn)
						{
							if (WhichOne + 1 < PlayerBody.size())
							{
								PlayerBody.at(WhichOne + 1).X = PlayerBody.at(WhichOne).PreX;
								PlayerBody.at(WhichOne + 1).Y = PlayerBody.at(WhichOne).PreY;

								if (PlayerBody.at(WhichOne + 1).X == PlayerBody.at(WhichOne).PreX && PlayerBody.at(WhichOne + 1).Y == PlayerBody.at(WhichOne).PreY)
								{
									PlayerBody.at(WhichOne).PreX = PlayerBody.at(WhichOne).X;
									PlayerBody.at(WhichOne).PreY = PlayerBody.at(WhichOne).Y;

									//right
									PlayerBody.at(WhichOne).Y += YDir;
									i = Size;
								}
							}
							else
							{
								PlayerBody.at(WhichOne).PreX = PlayerBody.at(WhichOne).X;
								PlayerBody.at(WhichOne).PreY = PlayerBody.at(WhichOne).Y;

								//right
								PlayerBody.at(WhichOne).Y += YDir;
								i = Size;
							}
						}
					}

					//if we are trying to move in the x direction and the tile is not a wall then move the player
					if (XDir != 0)
					{
						if (PTurn)
						{
							if (WhichOne + 1 < PlayerBody.size())
							{
								PlayerBody.at(WhichOne + 1).X = PlayerBody.at(WhichOne).PreX;
								PlayerBody.at(WhichOne + 1).Y = PlayerBody.at(WhichOne).PreY;

								if (PlayerBody.at(WhichOne + 1).X == PlayerBody.at(WhichOne).PreX && PlayerBody.at(WhichOne + 1).Y == PlayerBody.at(WhichOne).PreY)
								{
									PlayerBody.at(WhichOne).PreX = PlayerBody.at(WhichOne).X;
									PlayerBody.at(WhichOne).PreY = PlayerBody.at(WhichOne).Y;

									//left
									PlayerBody.at(WhichOne).X += XDir;
									i = Size;
								}
							}
							else
							{
								PlayerBody.at(WhichOne).PreX = PlayerBody.at(WhichOne).X;
								PlayerBody.at(WhichOne).PreY = PlayerBody.at(WhichOne).Y;

								//left
								PlayerBody.at(WhichOne).X += XDir;
								i = Size;
							}
						}
					}
				}
			}
			else
			{
				//if we are about to hit a wall stop moving and reduce the player size
				if (Tiles[i].Y == PlayerBody.at(WhichOne).Y + YDir && Tiles[i].X == PlayerBody.at(WhichOne).X + XDir)
				{
					if (PlayerBody.size() > 1)
					{
						if (PTurn)
						{
							PlayerBody.pop_back();
							StopMoving = true;
						}
					}
					else
					{
						LoseGame();
					}
				}
			}
		}
	}
	//if this is the enemy and they can move
	else if(!IsPlayer && !EStopMoving)
	{
	//get a random direction
		static std::random_device device;
		static std::default_random_engine generator{ device() };

		std::uniform_int_distribution<int> distribution(-1, 1);
		std::uniform_int_distribution<int> distribution2(-1, 1);
		int dir = distribution(generator);
		int amount = distribution(generator);

		//if there is a tile in that direction, that the enemy can move to then move there
		for (int i = 0; i < Size; i++)
		{
			if (Tiles[i].FState != FWall && Tiles[i].EState != EEnemy)
			{
				if (dir > 0)
				{
					if (Tiles[i].X == Enemys.at(WhichOne).X + amount && Tiles[i].Y == Enemys.at(WhichOne).Y)
					{
						if (ETurn)
						{
							Enemys.at(WhichOne).X += amount;
							i = Size;
						}
					}
				}
				else if (dir < 0)
				{
					if (Tiles[i].Y == Enemys.at(WhichOne).Y + amount && Tiles[i].X == Enemys.at(WhichOne).X)
					{
						if (ETurn)
						{
							Enemys.at(WhichOne).Y += amount;
							i = Size;
						}
					}
				}
			}
		}
	}
}

void Application::WinGame() 
{
	StopMoving = true;
	EStopMoving = true;
	Win = true;
}

void Application::LoseGame()
{
	StopMoving = true;
	EStopMoving = true;
	Lose = true;
}

void Application::MovePlayer()
{
	//Inputs
	if (IsKeyPressed(KEY_D))
	{
		PlayerBody.at(0).DirectionInput = Right;
	}
	if (IsKeyPressed(KEY_A))
	{
		PlayerBody.at(0).DirectionInput = Left;
	}
	if (IsKeyPressed(KEY_W))
	{
		PlayerBody.at(0).DirectionInput = Up;
	}
	if (IsKeyPressed(KEY_S))
	{
		PlayerBody.at(0).DirectionInput = Down;
	}

	//Movement
	if (PlayerBody.at(0).DirectionInput == Right)
	{
		MoveObject(true, 0, 1, 0);
	}
	else if (PlayerBody.at(0).DirectionInput == Left)
	{
		MoveObject(true, 0, -1, 0);
	}
	else if (PlayerBody.at(0).DirectionInput == Up)
	{
		MoveObject(true, 0, 0, -1);
	}
	else if (PlayerBody.at(0).DirectionInput == Down)
	{
		MoveObject(true, 0, 0, 1);
	}

	//Body movement
	if (PTurn)
	{
		int num = 1;

		//pull the next part of the player body to this tiles previous location (make sure we go through each part of the body from top to bottom)
		for (int t = 0; t < Size; t++)
		{
			for (int i = 1; i < PlayerBody.size(); i++)
			{
				if (Tiles[t].X == PlayerBody.at(i).X && Tiles[t].Y == PlayerBody.at(i).Y)
				{
					if (PlayerBody.at(i).X == PlayerBody.at(num).X && PlayerBody.at(i).Y == PlayerBody.at(num).Y) 
					{
						if (i + 1 < PlayerBody.size())
						{
							if (PlayerBody.at(i).X != PlayerBody.at(i - 1).X || PlayerBody.at(i).Y != PlayerBody.at(i - 1).Y)
							{
								if (!StopMoving) 
								{
									PlayerBody.at(i + 1).X = PlayerBody.at(i).PreX;
									PlayerBody.at(i + 1).Y = PlayerBody.at(i).PreY;
								}
							}

							if (PlayerBody.at(i + 1).X == PlayerBody.at(i).PreX && PlayerBody.at(i + 1).Y == PlayerBody.at(i).PreY)
							{
								PlayerBody.at(i).PreX = PlayerBody.at(i).X;
								PlayerBody.at(i).PreY = PlayerBody.at(i).Y;
							}
						}
						else
						{
							PlayerBody.at(i).PreX = PlayerBody.at(i).X;
							PlayerBody.at(i).PreY = PlayerBody.at(i).Y;
						}

						if (num != PlayerBody.size() - 1) 
						{
							num++;
							t = 0;
						}
						else 
						{
							t = Size;
						}
					}
				}
			}
		}
	}
}

bool Application::RandomBool(int a, int b)
{
	static std::random_device device;
	static std::default_random_engine generator{ device() };

	std::uniform_int_distribution<int> distribution(a, b);
	int num = distribution(generator);

	if (num == b) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

int Application::RandomInt(int a, int b)
{
	static std::random_device device;
	static std::default_random_engine generator{ device() };

	while (true)
	{
		std::uniform_int_distribution<int> distribution(a, b);
		int num = distribution(generator);

		int x = num % m_cols;
		int y = num / m_cols;

		if (x == 0 || x == m_cols - 1) {}
		else if (y == 0 || y == m_rows - 1) {}
		else 
		{
			return num;
		}
	}
}
