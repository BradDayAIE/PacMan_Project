#pragma once

#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <functional> 

class Application
{
public:
	Application();
	~Application();

	//Raylib functions for making the game window
	void Run();

	void Load();
	void Unload();

	void Update(float dt);
	void Draw();

	//Movement functions for moving various objects
	//(Move object is for enemies and the player head, Move player is for making the player body follow the head)
	void MoveObject(bool IsPlayer, int WhichOne, int XDir = 0, int YDir = 0);
	void MovePlayer();

	//Functions to stop everything from moving and turn on the win or lose text
	void LoseGame();
	void WinGame();

	//Functions for getting random values
	bool RandomBool(int a, int b);
	int RandomInt(int a, int b);

protected:
private:
	//the size of the game window
	int m_windowWidth = 1000;
	int m_windowHeight = 1000;

	//the amount of rows/coloums 
	int m_cols = 50;
	int m_rows = 50;

	int Size = 0;

	// these are the dirrent state machines for each tile for controlling what is on them, collisions and what to draw
	enum FilledState { FEmpty, FWall, FCoin };
	enum EnemyState { EEmpty, EEnemy };
	enum PlayerState { PEmpty, PPlayer };

	// the player state machine for movement
	enum Direction { Up, Down, Left, Right };

	// this struct is used in a array making all tiles contain the values within
	struct Tile
	{
		//Position
		int X = 0;
		int Y = 0;

		FilledState FState;
		EnemyState EState;
		PlayerState PState;
	};

	//this is the player struct which is used within the playerbody vector to ensur the player can move
	struct Player
	{
		//Position
		int X = 0;
		int Y = 0;

		//PrePosition
		int PreX = 0;
		int PreY = 0;

		//Input
		Direction DirectionInput;

		//is this the head
		bool Head = false;
	};

	//this is the enemy struct for the enemys vector 
	struct Enemey
	{
		//Position
		int X = 0;
		int Y = 0;
	};

	//is a pointer for the tiles
	Tile* Tiles = nullptr;

	//these vectors are for the player and enemys on the grid
	std::vector<Enemey> Enemys;
	std::vector<Player> PlayerBody;

	//the timer that decides when the player can move
	float PTurnTimer = 0;
	float PTurnTimerMax = 0.5f;

	//the timer that decides when the enemy's can move
	float ETurnTimer = 0;
	float ETurnTimerMax = 0.8f;

	//how long the player must be to move
	float WinAmount = 30;

	//is it the enemy turn or the player turn
	bool PTurn = true;
	bool ETurn = true;

	//a check that is used to insure the player dosn't die instantly because it has no length
	bool GameStarted = true;

	//stop the player and enmys from moving if true
	bool StopMoving = false;
	bool EStopMoving = false;

	//if on it draws the lose/win text
	bool Lose = false;
	bool Win = false;
};
