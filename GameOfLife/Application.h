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

	void Run();

	void Load();
	void Unload();

	void Update(float dt);
	void Draw();

	void MoveObject(bool IsPlayer, int WhichOne, int XDir = 0, int YDir = 0);
	void MovePlayer();

	void LoseGame();
	void WinGame();

	bool RandomBool(int a, int b);
	int RandomInt(int a, int b);

protected:
private:
	int m_windowWidth = 945;
	int m_windowHeight = 945;

	int m_cols = 35;
	int m_rows = 35;

	int Size = 0;

	enum FilledState { FEmpty, FWall, FCoin };
	enum EnemyState { EEmpty, EEnemy };
	enum PlayerState { PEmpty, PPlayer };

	enum Direction { Up, Down, Left, Right };

	struct Tile
	{
		//Position
		int X = 0;
		int Y = 0;

		FilledState FState;
		EnemyState EState;
		PlayerState PState;
	};

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

	struct Enemey
	{
		//Position
		int X = 0;
		int Y = 0;
	};

	Tile* Tiles = nullptr;

	std::vector<Enemey> Enemys;
	std::vector<Player> PlayerBody;

	int WallNum = 0;
	int CoinNum = 0;

	float PTurnTimer = 0;
	float PTurnTimerMax = 0.5f;

	float ETurnTimer = 0;
	float ETurnTimerMax = 0.8f;

	bool PTurn = true;
	bool ETurn = true;

	bool GameStarted = true;

	bool StopMoving = false;
	bool EStopMoving = false;

	bool Lose = false;
	bool Win = false;
};
