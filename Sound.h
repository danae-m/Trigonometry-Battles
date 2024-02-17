#ifndef SOUND_H
#define SOUND_H

#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Audio/Sound.hpp"

class Sounds
{
	sf::SoundBuffer bulletBuffer;
	sf::Sound bulletSound;
	sf::SoundBuffer eDeathBuffer;
	sf::Sound eDeathSound;
	sf::SoundBuffer pDeathBuffer;
	sf::Sound pDeathSound;
	sf::SoundBuffer laserBuffer;
	sf::Sound laserSound;
	sf::SoundBuffer pulseBuffer;
	sf::Sound pulseSound;
	sf::SoundBuffer blipBuffer;
	sf::Sound blipSound;
	sf::SoundBuffer selectBuffer;
	sf::Sound selectSound;

public:
	Sounds() {};

	void sInit()
	{
		bulletBuffer.loadFromFile("sounds/bullet.wav");
		bulletSound.setBuffer(bulletBuffer);

		eDeathBuffer.loadFromFile("sounds/enemydeath.wav");
		eDeathSound.setBuffer(eDeathBuffer);

		pDeathBuffer.loadFromFile("sounds/playerdeath.wav");
		pDeathSound.setBuffer(pDeathBuffer);

		laserBuffer.loadFromFile("sounds/laser.wav");
		laserSound.setBuffer(laserBuffer);

		pulseBuffer.loadFromFile("sounds/pulsar.wav");
		pulseSound.setBuffer(pulseBuffer);
		pulseSound.setVolume(50);

		blipBuffer.loadFromFile("sounds/menublip.wav");
		blipSound.setBuffer(blipBuffer);

		selectBuffer.loadFromFile("sounds/select.wav");
		selectSound.setBuffer(selectBuffer);
		selectSound.setVolume(40);
	}

	void BulletSound()
	{
		bulletSound.play();
	}

	void EnemyDeathSound()
	{
		eDeathSound.play();
	}

	void PlayerDeathSound()
	{
		pDeathSound.play();
	}

	void LaserSound()
	{
		laserSound.play();
	}

	void PulseSound()
	{
		pulseSound.play();
	}

	void BlipSound()
	{
		blipSound.play();
	}

	void SelectSound()
	{
		selectSound.play();
	}
};

#endif //SOUND_H
