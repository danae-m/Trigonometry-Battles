#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>

#include "json/value.h"
#include "json/json.h"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "Physics.h"
#include "Sound.h"
#include "Utility.h"
#include "Particles.h"

#include <SFML/Audio.hpp>

struct PlayerStr
{ 
	int vertex, speed;
	float radius, thickness;
	sf::Color fillC, outlC;

	PlayerStr() {};
	PlayerStr(sf::Color fillC, sf::Color outlC, int vertex, float radius, int speed, float thickness) :
		fillC(fillC), outlC(outlC), vertex(vertex), radius(radius), speed(speed), thickness(thickness) {};
};

struct EnemyStr
{
	int vertMin, vertMax, lifespan, speedMin, speedMax;
	float radius, thickness;
	sf::Color outlC;

	EnemyStr() {};
	EnemyStr(sf::Color outlC, int vertMin, int vertMax, int lifespan, float radius, int speedMin, int speedMax, float thickness) :
		outlC(outlC), vertMin(vertMin), vertMax(vertMax), lifespan(lifespan), radius(radius), speedMin(speedMin), speedMax(speedMax), thickness(thickness) {};
};

struct BulletStr
{
	int vertex, lifespan, speed;
	float radius, thickness;
	sf::Color fillC, outlC;

	BulletStr() {};
	BulletStr(sf::Color fillC, sf::Color outlC, int vertex, int lifespan, float radius, int speed, float thickness) :
		fillC(fillC), outlC(outlC), vertex(vertex), lifespan(lifespan), radius(radius), speed(speed), thickness(thickness) {};
};

struct LaserStr
{
	int speed;
	float width, thickness;
	sf::Color fillC, outlC;

	LaserStr() {};
	LaserStr(sf::Color fillC, sf::Color outlC, float width, int speed, float thickness) :
		fillC(fillC), outlC(outlC), width(width), speed(speed), thickness(thickness) {};
};

class Game
{
	sf::RenderWindow m_window;
	sf::Clock m_deltaClock;
	sf::RenderTexture renderTexture;
	sf::Shader screenShader;
	//screenShader.setUniform("u_resolution", sf::Glsl::Vec2{ window.getSize() });
	//screenShader.setUniform("u_mouse", sf::Glsl::Vec2{ mouse_position });
	//screenShader.setUniform("u_time", clock.getElapsedTime().asSeconds());
	//sf::RenderTexture PPFX;
	//sf::Vector2f screenSize;
	sf::Font font;
	sf::Text scoreText, playText, settingsText, exitText, diffText, speedText, spText, redText, blueText, greenText, colorText, saveText;
	sf::Color pFill, pOut;
	sf::Color menuColor = sf::Color{ 25, 0, 90 };
	sf::Color menuOutlineColor = sf::Color{ 100, 215, 235 };
	sf::Color menuHoverColor = sf::Color{ 235, 100, 235 };
	sf::Color menuClickColor = sf::Color{ 15, 5, 40 };
	sf::Music music;
	sf::Cursor cursor;
	Vec2 sliderRange;

	Json::Value data;
	Json::Reader reader;

	std::shared_ptr<Entity> fillBar;
	std::shared_ptr<Entity> playButton, settingsButton, exitButton, saveButton;
	std::shared_ptr<Entity> menuBg;
	std::shared_ptr<Entity> redBar, redSlider, greenBar, greenSlider, blueBar, blueSlider;
	std::shared_ptr<Entity> spawnBar, spawnSlider, speedBar, speedSlider;
	std::shared_ptr<Entity> activeHandle;

	std::shared_ptr<Entity> m_player;
	EntityManager EntMgr;
	EnemyStr enemyStr;
	PlayerStr playerStr;
	BulletStr bulletStr;
	LaserStr laserStr;
	Indicator indicator;
	Sounds s;
	ParticleSystem particles;

	enum GameState
	{
		Running,
		DeathPause,
		Menu,
		Settings,
		Closed
	};
	GameState state;

	int spawnInt;
	int wWidth, wHeight, wDiag;
	int boundWidth, northBound, eastBound, southBound, westBound;
	int killCountdown, pulseCooldown, laserLength, laserHalfLength;
	float cdStart = -2;
	bool deathScene = false;
	bool playerFlash = false;
	bool pulseReady = true;
	bool dragging = false;
	float redNew, blueNew, greenNew, spawnNew, speedNew;
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;
	float laserInterval;
	int pulseFrames = 0;
	int alpha = 255;

public:

	Game(const std::string& config);

	void init();
	void JConfig(const std::string& path);
	void run();

	void spawnPlayer();
	void spawnUtility();
	void spawnEnemy(Vec2);
	void spawnSmallEnemies(std::shared_ptr<Entity>);
	void spawnBullet(const Vec2&);
	void spawnLaser(const Vec2&);
	void spawnLaserBullet(std::shared_ptr<Entity>);
	void spawnPulseAttack();

	void LaserUpdate(std::shared_ptr<Entity>);
	void PulseCooldown(int);
	void PulseUpdate(std::shared_ptr<Entity>);
	Vec2 GetVelocity(float angle, int speed);
	void KillPlayer();

	void GameMenu();
	void SettingsMenu();
	void Hover(std::shared_ptr<Entity>);
	void SaveSettings();

	void sMovement();
	void sLifespan();
	void sCollision();
	void sPhysics();
	void sEnemySpawner();
	void sRender();
	void sUserInput();

};

#endif //GAME_H