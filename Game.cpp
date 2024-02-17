#include "Game.h"

float PI = 3.14159;

//-------------------------------------------------------------------------------------------------------------
//                                             GAME RUNTIME AND INIT
//-------------------------------------------------------------------------------------------------------------

Game::Game(const std::string& config)
{
	JConfig(config);
	init();
}

void Game::JConfig(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Error opening file." << std::endl;
	}
	reader.parse(file, data);
	file.close();


	//player config data read in
	sf::Color p_fill = sf::Color{ static_cast<uint8_t>(data["player"]["fill r"].asInt()), static_cast<uint8_t>(data["player"]["fill g"].asInt()),
		static_cast<uint8_t>(data["player"]["fill b"].asInt()), 255 };
	sf::Color p_outline = sf::Color{ static_cast<uint8_t>(data["player"]["outline r"].asInt()), static_cast<uint8_t>(data["player"]["outline g"].asInt()),
		static_cast<uint8_t>(data["player"]["outline b"].asInt()), 255 };
	int p_vertex = (static_cast<int>(data["player"]["vertex"].asInt()));
	int p_speed = static_cast<int>(data["player"]["speed"].asInt());
	float p_radius = (data["player"]["radius"].asFloat());
	float p_thickness = (data["player"]["outline thickness"].asFloat());

	playerStr = PlayerStr(p_fill, p_outline, p_vertex, p_radius, p_speed, p_thickness);


	//enemy config data read in
	sf::Color e_outline = sf::Color{ static_cast<uint8_t>(data["enemy"]["outline r"].asInt()), static_cast<uint8_t>(data["enemy"]["outline g"].asInt()),
	static_cast<uint8_t>(data["enemy"]["outline b"].asInt()), 255 };
	int e_vertMin = (data["enemy"]["vertex min"].asInt());
	int e_vertMax = (data["enemy"]["vertex max"].asInt());
	int e_lifespan = (data["enemy"]["lifespan"].asInt());
	int e_speedMin = (data["enemy"]["speed min"].asInt());
	int e_speedMax = (data["enemy"]["speed max"].asInt());
	float e_radius = (data["enemy"]["radius"].asFloat());
	float e_thickness = (data["enemy"]["outline thickness"].asFloat());

	enemyStr = EnemyStr(e_outline, e_vertMin, e_vertMax, e_lifespan, e_radius, e_speedMin, e_speedMax, e_thickness);
	spawnInt = (data["enemy"]["spawn interval"].asInt());


	//bullet config data read-in
	sf::Color b_fill = sf::Color{ static_cast<uint8_t>(data["bullet"]["fill r"].asInt()), static_cast<uint8_t>(data["bullet"]["fill g"].asInt()),
	static_cast<uint8_t>(data["bullet"]["fill b"].asInt()), 255 };
	sf::Color b_outline = sf::Color{ static_cast<uint8_t>(data["bullet"]["outline r"].asInt()), static_cast<uint8_t>(data["bullet"]["outline g"].asInt()),
		static_cast<uint8_t>(data["bullet"]["outline b"].asInt()), 255 };
	int b_vertex = (data["bullet"]["vertex"].asInt());
	int b_lifespan = (data["bullet"]["lifespan"].asInt());
	int b_speed = (data["bullet"]["speed"].asInt());
	float b_radius = (data["bullet"]["radius"].asFloat());
	float b_thickness = (data["bullet"]["outline thickness"].asFloat());

	bulletStr = BulletStr(b_fill, b_outline, b_vertex, b_lifespan, b_radius, b_speed, b_thickness);


	//laser config data read-in
	sf::Color l_fill = sf::Color{ static_cast<uint8_t>(data["laser"]["fill r"].asInt()), static_cast<uint8_t>(data["laser"]["fill g"].asInt()),
		static_cast<uint8_t>(data["laser"]["fill b"].asInt()), 255 };
	sf::Color l_outline = sf::Color{ static_cast<uint8_t>(data["laser"]["outline r"].asInt()), static_cast<uint8_t>(data["laser"]["outline g"].asInt()),
		static_cast<uint8_t>(data["laser"]["outline b"].asInt()) };
	int l_speed = (data["laser"]["speed"].asInt());
	float l_width = (data["laser"]["width"].asFloat());
	float l_thickness = (data["laser"]["outline thickness"].asFloat());

	laserStr = LaserStr(l_fill, l_outline, l_width, l_speed, l_thickness);
}

void Game::init()
{
	//font and text init
	font.loadFromFile("fonts/SLC_.ttf");


	//sounds and music init
	s.Sounds::sInit();
	music.openFromFile("sounds/weirdmusic.wav");
	music.setPitch(0.9);
	music.setLoop(true);
	music.setVolume(20);
	music.play();


	//random time seeding init
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	srand(static_cast<unsigned int>(millis));


	//window and bounds init
	m_window.create(sf::VideoMode::getDesktopMode(), "Geometry Wars: The Ripoff", sf::Style::Fullscreen);
	m_window.setFramerateLimit(60);

	wWidth = sf::VideoMode::getDesktopMode().width;
	wHeight = sf::VideoMode::getDesktopMode().height;
	wDiag = std::sqrt((wWidth * wWidth) + (wHeight * wHeight));
	

	if (enemyStr.radius > playerStr.radius)
	{
		boundWidth = enemyStr.radius;
	}
	else
	{
		boundWidth = playerStr.radius;
	}

	northBound = 0 + boundWidth;
	eastBound = wWidth - boundWidth;
	southBound = wHeight - boundWidth;
	westBound = 0 + boundWidth;


	//cursor init
	sf::Image cursorImage;
	cursorImage.loadFromFile("graphics/crosshair.png");
	sf::Cursor cursor;
	cursor.loadFromPixels(cursorImage.getPixelsPtr(), cursorImage.getSize(), { 0, 0 });
	m_window.setMouseCursor(cursor);


	//start
	state = Menu;
	ImGui::SFML::Init(m_window);
	spawnUtility();
	spawnPlayer();
}

void Game::run()
{
	while (m_window.isOpen())
	{

		switch (state)
		{
		case GameState::Menu:
			GameMenu();
			EntMgr.update();
			sUserInput();
			sRender();
			break;

		case GameState::Settings:
			EntMgr.update();
			sUserInput();
			sRender();
			break;

		case GameState::Running:
			EntMgr.update();
			PulseCooldown(cdStart);
			sEnemySpawner();
			sMovement();
			sCollision();
			sUserInput();
			sLifespan();
			sRender();

			m_currentFrame++;
			break;

		case GameState::DeathPause:
			EntMgr.update();
			sCollision();
			sRender();
			m_currentFrame++;
			break;

		case GameState::Closed:
			music.stop();
			m_window.close();
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------
//                                              SPAWNING
//-------------------------------------------------------------------------------------------------------------

void Game::spawnUtility()
{
	//universal components
	//background
	Vec2 size = { 600, 520 };
	Vec2 position = Vec2{ static_cast<float>(wWidth / 2) - (size.x / 2), static_cast<float>(wHeight / 2) - (size.y / 2) };
	//buttons
	Vec2 buttonSize = { 200, 100 };
	//slider bar
	Vec2 sliderSize = { 200, 8 };
	sf::Color sliderColor = sf::Color{ 150, 150, 150 };
	sf::Color sliderOutlineColor = sf::Color{ 225, 225, 225 };
	sliderRange = Vec2{ position.x + ((size.x - sliderSize.x) / 2), position.x + ((size.x - sliderSize.x) / 2) + sliderSize.x };
	//slider handle
	float handleSize = 10;
	sf::Color handleOutlineColor = sf::Color{ 50, 50, 50 };
	//font
	sf::Color fontColor = sf::Color::White;
	int fontSize = 24;
	int labelSize = 18;


	//score text
	scoreText.setCharacterSize(fontSize);
	scoreText.setFont(font);
	scoreText.setFillColor(fontColor);
	scoreText.setPosition(10, 10);


	//menu background box for Menu and Settings
	auto bg = EntMgr.addEntity("menu");
	bg->cTransform = std::make_shared<CTransform>(position, Vec2{ 0, 0 }, 0.0f);
	bg->cRect = std::make_shared<CRect>(size.x, size.y, menuColor, menuOutlineColor, 3.0f);
	menuBg = bg;
	

	//Menu buttons
	auto play = EntMgr.addEntity("button");
	play->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - buttonSize.x) / 2), position.y + (size.y / 4) - (buttonSize.y / 2) }, Vec2{ 0, 0 }, 0.0f);
	play->cRect = std::make_shared<CRect>(buttonSize.x, buttonSize.y, menuColor, menuOutlineColor, 2.0f);
	play->cClicked = std::make_shared<CClicked>(false);
	playText.setString("Play");
	playText.setFont(font);
	playText.setCharacterSize(fontSize);
	playText.setFillColor(fontColor);
	playText.setPosition(play->cTransform->pos.x + ((buttonSize.x - playText.getLocalBounds().width) / 2),
		(play->cTransform->pos.y + ((buttonSize.y - playText.getLocalBounds().height) / 2) - 7.0f));
	playButton = play;

	auto settings = EntMgr.addEntity("button");
	settings->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - buttonSize.x) / 2), position.y + (size.y / 2) - (buttonSize.y / 2) }, Vec2{ 0, 0 }, 0.0f);
	settings->cRect = std::make_shared<CRect>(buttonSize.x, buttonSize.y, menuColor, menuOutlineColor, 2.0f);
	settings->cClicked = std::make_shared<CClicked>(false);
	settingsText.setString("Settings");
	settingsText.setFont(font);
	settingsText.setCharacterSize(fontSize);
	settingsText.setFillColor(fontColor);
	settingsText.setPosition(settings->cTransform->pos.x + ((buttonSize.x - settingsText.getLocalBounds().width) / 2),
		(settings->cTransform->pos.y + ((buttonSize.y - settingsText.getLocalBounds().height) / 2) - 7.0f));
	settingsButton = settings;

	auto exit = EntMgr.addEntity("button");
	exit->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - buttonSize.x) / 2), position.y + (size.y * 3 / 4) - (buttonSize.y / 2) }, Vec2{0, 0}, 0.0f);
	exit->cRect = std::make_shared<CRect>(buttonSize.x, buttonSize.y, menuColor, menuOutlineColor, 2.0f);
	exit->cClicked = std::make_shared<CClicked>(false);
	exitText.setString("Exit");
	exitText.setCharacterSize(fontSize);
	exitText.setFillColor(fontColor);
	exitText.setFont(font);
	exitText.setPosition(exit->cTransform->pos.x + ((buttonSize.x - exitText.getLocalBounds().width) / 2), 
		(exit->cTransform->pos.y + ((buttonSize.y - exitText.getLocalBounds().height) / 2) - 7.0f));
	exitButton = exit;


	//Settings buttons and sliders
	diffText.setString("Difficulty Settings");
	diffText.setCharacterSize(fontSize);
	diffText.setFillColor(fontColor);
	diffText.setFont(font);
	diffText.setPosition( position.x + ((size.x - diffText.getLocalBounds().width) / 2), position.y + 40);

	colorText.setString("Player Color");
	colorText.setCharacterSize(fontSize);
	colorText.setFillColor(fontColor);
	colorText.setFont(font);
	colorText.setPosition(position.x + ((size.x - colorText.getLocalBounds().width) / 2), position.y + 240);

	//spawn slider
	spText.setString("Enemy Spawn Rate");
	spText.setCharacterSize(labelSize);
	spText.setFillColor(fontColor);
	spText.setFont(font);
	spText.setPosition(position.x + ((size.x - spText.getLocalBounds().width) / 2), position.y + 80);

	auto s1 = EntMgr.addEntity("slider");
	s1->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - sliderSize.x) / 2), position.y + 120 }, Vec2{ 0, 0 }, 0.0f);
	s1->cRect = std::make_shared<CRect>(sliderSize.x, sliderSize.y, sliderColor, sliderOutlineColor, 2.0f);
	s1->cRect->rect.setOrigin(sf::Vector2f{ 0, sliderSize.y / 2 });
	spawnBar = s1;

	auto h1 = EntMgr.addEntity("handle");
	h1->cTransform = std::make_shared<CTransform>(Vec2{ spawnBar->cTransform->pos.x + (spawnBar->cRect->rect.getSize().x / 2), spawnBar->cTransform->pos.y },
		Vec2{ 0, 0 }, 0.0f);
	h1->cShape = std::make_shared<CShape>(handleSize, 32, sf::Color::Black, handleOutlineColor, 2.0f);
	spawnSlider = h1;

	//speed slider
	speedText.setString("Enemy Speed");
	speedText.setCharacterSize(labelSize);
	speedText.setFont(font);
	speedText.setFillColor(fontColor);
	speedText.setPosition( position.x + ((size.x - speedText.getLocalBounds().width) / 2), position.y + 160);

	auto s5 = EntMgr.addEntity("slider");
	s5->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - sliderSize.x) / 2), position.y + 200 }, Vec2{ 0, 0 }, 0.0f);
	s5->cRect = std::make_shared<CRect>(sliderSize.x, sliderSize.y, sliderColor, sliderOutlineColor, 2.0f);
	s5->cRect->rect.setOrigin(sf::Vector2f{ 0, sliderSize.y / 2 });
	speedBar = s5;

	auto h5 = EntMgr.addEntity("handle");
	h5->cTransform = std::make_shared<CTransform>(Vec2{ speedBar->cTransform->pos.x + (speedBar->cRect->rect.getSize().x / 2), speedBar->cTransform->pos.y },
		Vec2{ 0, 0 }, 0.0f);
	h5->cShape = std::make_shared<CShape>(handleSize, 32, sf::Color::Black, handleOutlineColor, 2.0f);
	speedSlider = h5;

	//red slider
	redText.setString("Red");
	redText.setCharacterSize(labelSize);
	redText.setFont(font);
	redText.setFillColor(fontColor);
	redText.setPosition( position.x + ((size.x - redText.getLocalBounds().width) / 2), position.y + 280);

	auto s2 = EntMgr.addEntity("slider");
	s2->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - sliderSize.x) / 2), position.y + 320 }, Vec2{ 0, 0 }, 0.0f);
	s2->cRect = std::make_shared<CRect>(sliderSize.x, sliderSize.y, sliderColor, sliderOutlineColor, 2.0f);
	s2->cRect->rect.setOrigin(sf::Vector2f{ 0, sliderSize.y / 2 });
	redBar = s2;

	auto h2 = EntMgr.addEntity("handle");
	h2->cTransform = std::make_shared<CTransform>(Vec2{ redBar->cTransform->pos.x + redBar->cRect->rect.getSize().x, redBar->cTransform->pos.y },
		Vec2{ 0, 0 }, 0.0f);
	h2->cShape = std::make_shared<CShape>(handleSize, 32, sf::Color::Red, handleOutlineColor, 2.0f);
	redSlider = h2;

	//green slider
	greenText.setString("Green");
	greenText.setCharacterSize(labelSize);
	greenText.setFont(font);
	greenText.setFillColor(fontColor);
	greenText.setPosition( position.x + ((size.x - greenText.getLocalBounds().width) / 2), position.y + 360);

	auto s3 = EntMgr.addEntity("slider");
	s3->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - sliderSize.x) / 2), position.y + 400 }, Vec2{ 0, 0 }, 0.0f);
	s3->cRect = std::make_shared<CRect>(sliderSize.x, sliderSize.y, sliderColor, sliderOutlineColor, 2.0f);
	s3->cRect->rect.setOrigin(sf::Vector2f{ 0, sliderSize.y / 2 });
	greenBar = s3;

	auto h3 = EntMgr.addEntity("handle");
	h3->cTransform = std::make_shared<CTransform>(Vec2{ greenBar->cTransform->pos.x, greenBar->cTransform->pos.y }, Vec2{ 0, 0 }, 0.0f);
	h3->cShape = std::make_shared<CShape>(handleSize, 32, sf::Color::Green, handleOutlineColor, 2.0f);
	greenSlider = h3;

	//blue slider
	blueText.setString("Blue");
	blueText.setCharacterSize(labelSize);
	blueText.setFont(font);
	blueText.setFillColor(fontColor);
	blueText.setPosition( position.x + ((size.x - blueText.getLocalBounds().width) / 2), position.y + 440 );

	auto s4 = EntMgr.addEntity("slider");
	s4->cTransform = std::make_shared<CTransform>(Vec2{ position.x + ((size.x - sliderSize.x) / 2), position.y + 480 }, Vec2{ 0, 0 }, 0.0f);
	s4->cRect = std::make_shared<CRect>(sliderSize.x, sliderSize.y, sliderColor, sliderOutlineColor, 2.0f);
	s4->cRect->rect.setOrigin(sf::Vector2f{ 0, sliderSize.y / 2 });
	blueBar = s4;

	auto h4 = EntMgr.addEntity("handle");
	h4->cTransform = std::make_shared<CTransform>(Vec2{ blueBar->cTransform->pos.x, blueBar->cTransform->pos.y }, Vec2{ 0, 0 }, 0.0f);
	h4->cShape = std::make_shared<CShape>(handleSize, 32, sf::Color::Blue, handleOutlineColor, 2.0f);
	blueSlider = h4;
	
	//save button
	auto save = EntMgr.addEntity("slider");
	save->cTransform = std::make_shared<CTransform>(Vec2{ (position.x + size.x) - (buttonSize.x / 2) - 10, (position.y + size.y) - (buttonSize.y / 2) - 10 }, Vec2{0, 0}, 0.0f);
	save->cRect = std::make_shared<CRect>(buttonSize.x / 2, buttonSize.y / 2, menuColor, menuOutlineColor, 2.0f);
	save->cClicked = std::make_shared<CClicked>(false);
	saveText.setString("Save");
	saveText.setFont(font);
	saveText.setCharacterSize(labelSize);
	saveText.setFillColor(fontColor);
	saveText.setPosition(save->cTransform->pos.x + (((buttonSize.x / 2) - saveText.getLocalBounds().width) / 2),
		(save->cTransform->pos.y + (((buttonSize.y / 2) - saveText.getLocalBounds().height) / 2) - 7.0f));
	saveButton = save;


	//Cooldown Indicator
	auto a = EntMgr.addEntity("box");
	a->cTransform = std::make_shared<CTransform>(Vec2{ 15, 48 }, Vec2{ 0, 0 }, 0.0f);
	a->cRect = std::make_shared<CRect>(100, 24, sf::Color{ 200, 200, 200 }, sf::Color{ 150, 150, 150 }, 2.0f);

	auto b = EntMgr.addEntity("box");
	b->cTransform = std::make_shared<CTransform>(Vec2{ 20, 53 }, Vec2{ 0, 0 }, 0.0f);
	b->cRect = std::make_shared<CRect>(90, 14, sf::Color{ 100, 100, 100, 100 }, sf::Color{ 150, 150, 150 }, 1.0f);

	auto c = EntMgr.addEntity("box");
	c->cTransform = std::make_shared<CTransform>(Vec2{ 20, 53 }, Vec2{ 0, 0 }, 0.0f);
	c->cRect = std::make_shared<CRect>(90, 14, sf::Color{ 0, 190, 45 }, sf::Color{ 0, 120, 30 }, 1.0f);

	fillBar = c;
}

void Game::spawnPlayer()
{
	auto entity = EntMgr.addEntity("player");
	float vel = static_cast<float>(playerStr.speed);
	pFill = playerStr.fillC;
	pOut = playerStr.outlC;
	entity->cTransform = std::make_shared<CTransform>(Vec2(wWidth / 2, wHeight / 2), Vec2{ vel, vel }, 0.0f);
	entity->cShape = std::make_shared<CShape>(playerStr.radius, playerStr.vertex, pFill, pOut, playerStr.thickness);
	entity->cInput = std::make_shared<CInput>();
	entity->cScore = std::make_shared<CScore>(0);

	killCountdown = 99;

	m_player = entity;
}

void Game::spawnEnemy(Vec2 position)
{
	auto entity = EntMgr.addEntity("enemy");

	int vert = (enemyStr.vertMin + (rand() % (enemyStr.vertMax - enemyStr.vertMin + 1)));
	int speedTemp = (enemyStr.speedMin + (rand() % (enemyStr.speedMax - enemyStr.speedMin + 1)));
	Vec2 velocity = { static_cast<float>(speedTemp), static_cast<float>(speedTemp) };
	uint8_t red = (rand() % 256);
	uint8_t green = (rand() % 256);
	uint8_t blue = (rand() % 256);
	sf::Color fillColor = { red, green, blue, static_cast <uint8_t>(255) };

	entity->cTransform = std::make_shared<CTransform>(position, velocity, speedTemp, 0.0f); //dw
	entity->cShape = std::make_shared<CShape>(enemyStr.radius, vert, fillColor, enemyStr.outlC, enemyStr.thickness);
	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	int speed = 3;

	//gather former enemy components
	float radius = (e->cShape->circle.getRadius() / 2);
	int vertex = e->cShape->circle.getPointCount();
	sf::Color fill = e->cShape->circle.getFillColor();
	sf::Color outline = e->cShape->circle.getOutlineColor();
	float thickness = e->cShape->circle.getOutlineThickness();
	Vec2 position = e->cTransform->getPosition();

	float angle = ((360 / vertex) * (PI / 180));
	float theta = 0;
	int vertCount = vertex;
	std::vector<float> angles;
	angles.push_back(theta);
	vertCount -= 1;
	if ((vertex % 2) == 0)
	{
		angles.push_back(PI);
		vertCount -= 1;
	}

	for (int i = 0; i < vertCount / 2; i++)
	{
		theta += angle;
		angles.push_back(theta);
		angles.push_back(theta * -1);
	}


	//update components for all new small entities
	for (int i = 0; i < vertex; i++)
	{
		auto entity = EntMgr.addEntity("small enemy");

		Vec2 velocity = GetVelocity(angles[i], speed);
		entity->cTransform = std::make_shared<CTransform>(position, velocity, speed, 0.0f);
		entity->cShape = std::make_shared<CShape>(radius, vertex, fill, outline, thickness);
		entity->cLifespan = std::make_shared<CLifespan>(enemyStr.lifespan);
	}
}

void Game::spawnBullet(const Vec2& target)
{
	auto entity = EntMgr.addEntity("bullet");

	Vec2 position = m_player->cTransform->getPosition();
	Vec2 velocity = GetVelocity(Vec2{target.x, target.y}.angle(position), bulletStr.speed);

	entity->cTransform = std::make_shared<CTransform>(position, velocity, 0.0f);
	entity->cShape = std::make_shared<CShape>(bulletStr.radius, bulletStr.vertex, bulletStr.fillC, bulletStr.outlC, bulletStr.thickness);
	entity->cLifespan = std::make_shared<CLifespan>(bulletStr.lifespan);
	s.Sounds::BulletSound();
}

void Game::spawnLaser(const Vec2& target)
{
	auto entity = EntMgr.addEntity("laser");

	Vec2 position = m_player->cTransform->getPosition();
	float angle = (Vec2{ target.x, target.y }.angle(position) * (180 / PI));

	entity->cTransform = std::make_shared<CTransform>(position, Vec2{ 0, 0 }, angle);
	entity->cRect = std::make_shared<CRect>(1.0f, laserStr.width, laserStr.fillC, laserStr.outlC, laserStr.thickness);
	entity->cRect->rect.setOrigin(0, laserStr.width / 2);

	laserLength = laserStr.speed;
	laserHalfLength = laserLength / 2;
	laserInterval = (wDiag / laserHalfLength);

	s.Sounds::LaserSound();
}

void Game::spawnLaserBullet(std::shared_ptr<Entity> e)
{
	auto b = EntMgr.addEntity("laserbullet");

	float b_radius = (laserStr.width);
	Vec2 lb_pos = e->cTransform->pos;
	float lb_angle = e->cTransform->angle * (PI / 180);
	Vec2 velocity = GetVelocity(lb_angle, laserInterval / 2);
	b->cTransform = std::make_shared<CTransform>(lb_pos, velocity, 0.0f);
	b->cShape = std::make_shared<CShape>(b_radius, bulletStr.vertex, sf::Color::Transparent, sf::Color::Transparent, bulletStr.thickness);
}

void Game::spawnPulseAttack()
{
	Vec2 position = m_player->cTransform->getPosition();

	for (auto e : EntMgr.getEntities("enemy"))
	{
		Vec2 p = e->cTransform->getPosition();
		if (p.dist(position) < 90000)
		{
			e->cTransform->velocity = GetVelocity(p.angle(position), e->cTransform->speed);
		}
	}
	for (auto se : EntMgr.getEntities("small enemy"))
	{
		Vec2 p = se->cTransform->getPosition();
		if (p.dist(position) < 40000)
		{
			se->cTransform->velocity = GetVelocity(p.angle(position), se->cTransform->speed);
		}
	}

	auto entity = EntMgr.addEntity("pulsar");

	entity->cTransform = std::make_shared<CTransform>(position, Vec2{ 0, 0 }, 0.0f);
	entity->cShape = std::make_shared<CShape>(5.0f, 32, sf::Color::Transparent, sf::Color::Transparent, 0.0f);

	particles.resetAllParticles();

	alpha = 255;
	pulseFrames = 10;
	pulseReady = false;
	cdStart = m_currentFrame;
	pulseCooldown = 300;

	s.PulseSound();
}

//-------------------------------------------------------------------------------------------------------------
//                                             GAME UTILITIES
//-------------------------------------------------------------------------------------------------------------

void Game::LaserUpdate(std::shared_ptr<Entity> e)
{
	--laserLength;

	if (laserLength > 0)
	{
		spawnLaserBullet(e);
		if (laserLength > laserHalfLength)
		{
			sf::Vector2f size = e->cRect->rect.getSize();
			e->cRect->rect.setSize(sf::Vector2f{ (size.x + laserInterval), size.y });
		}
	}
	else
	{
		EntMgr.destroyEntity(e);
		for (auto b : EntMgr.getEntities("laserbullet"))
		{
			EntMgr.destroyEntity(b);
		}
	}
}

void Game::PulseUpdate(std::shared_ptr<Entity> e)
{
	if (pulseFrames > 0)
	{
		particles.setEmitter(sf::Vector2f{ e->cTransform->pos.x, e->cTransform->pos.y });
		sf::Time elapsed = m_deltaClock.restart();
		particles.update(elapsed);
		float r = e->cShape->circle.getRadius();
		alpha -= 15;
		e->cShape->circle.setRadius(r + 30);
		e->cShape->circle.setOrigin(r + 30, r + 30);
		sf::Color c = sf::Color{ e->cShape->circle.getFillColor().r, e->cShape->circle.getFillColor().g, e->cShape->circle.getFillColor().b, static_cast<uint8_t>(alpha)};
		e->cShape->circle.setFillColor(c);
		--pulseFrames;
	}
	else if (pulseFrames == 0)
	{
		EntMgr.destroyEntity(e);
	}
}

void Game::PulseCooldown(int cdStart)
{
	if (cdStart + 1 == m_currentFrame)
	{
		fillBar->cRect->rect.setSize(sf::Vector2f{ 0, fillBar->cRect->rect.getSize().y });
		fillBar->cRect->rect.setFillColor(sf::Color::Red);
		fillBar->cRect->rect.setOutlineColor(sf::Color::Red);
	}
	else if (cdStart + pulseCooldown > m_currentFrame)
	{
		float fillLength = (((static_cast<float>(m_currentFrame) - cdStart) / pulseCooldown) * 90);
		fillBar->cRect->rect.setSize(sf::Vector2f{ fillLength, fillBar->cRect->rect.getSize().y });
		if (m_currentFrame == cdStart + 100)
		{
			fillBar->cRect->rect.setFillColor(sf::Color{ 230, 110, 0 });
			fillBar->cRect->rect.setOutlineColor(sf::Color{ 230, 110, 0 });
		}
		if (m_currentFrame == cdStart + 200)
		{
			fillBar->cRect->rect.setFillColor(sf::Color::Yellow);
			fillBar->cRect->rect.setOutlineColor(sf::Color::Yellow);
		}
	}
	else if (cdStart + pulseCooldown == m_currentFrame)
	{
		fillBar->cRect->rect.setSize(sf::Vector2f{ 90, fillBar->cRect->rect.getSize().y });
		fillBar->cRect->rect.setFillColor(sf::Color{ 0, 190, 45 });
		fillBar->cRect->rect.setOutlineColor(sf::Color{ 0, 120, 30 });
		pulseReady = true;
	}
}

Vec2 Game::GetVelocity(float angle, int speed)
{
	if (angle <= 0 && angle >= (PI / -2))
	{
		return Vec2{ speed * cos(angle), speed * sin(angle) };
	}
	if (angle < (PI / -2) && angle >= (-1 * PI))
	{
		angle = PI - angle;
		return Vec2{ -1 * speed * cos(angle), speed * sin(angle) };
	}
	if (angle > (PI / 2) && angle <= PI)
	{
		angle = angle - PI;
		return Vec2{ -1 * speed * cos(angle), -1 * speed * sin(angle) };
	}
	if (angle > 0 && angle <= (PI / 2))
	{
		angle = (2 * PI) - angle;
		return Vec2{ speed * cos(angle), -1 * speed * sin(angle) };
	}
}

void Game::KillPlayer()
{
	if (killCountdown > 0)
	{
		if ((killCountdown % 10) == 0)
		{
			playerFlash = !(playerFlash);

			if (playerFlash)
			{
				m_player->cShape->circle.setOutlineColor(sf::Color::Transparent);
				m_player->cShape->circle.setFillColor(sf::Color::Transparent);
			}
			else
			{
				m_player->cShape->circle.setOutlineColor(pOut);
				m_player->cShape->circle.setFillColor(pFill);
			}
		}
	}
	else if (killCountdown == 0)
	{
		state = Running;
		deathScene = false;
		EntMgr.destroyEntity(m_player);
		spawnPlayer();
	}
}

void Game::GameMenu()
{
	if (playButton->cClicked->click)
	{
		playButton->cClicked->click = false;
		state = Running;
	}
	if (settingsButton->cClicked->click)
	{
		settingsButton->cClicked->click = false;
		state = Settings;
	}
	if (exitButton->cClicked->click)
	{
		exitButton->cClicked->click = false;
		state = Closed;
	}
}

void Game::SaveSettings()
{
	std::ifstream file("config.json");
	Json::Value root;
	file >> root;

	float max = sliderRange.y - sliderRange.x;

	spawnNew = ((1.0f - ((spawnSlider->cShape->circle.getPosition().x - sliderRange.x) / max)) * 90) + 10;
	speedNew = (1 + ((speedSlider->cShape->circle.getPosition().x - sliderRange.x) / max) * 11);
	redNew = ((redSlider->cShape->circle.getPosition().x - sliderRange.x) / max) * 255;
	greenNew = ((greenSlider->cShape->circle.getPosition().x - sliderRange.x) / max) * 255;
	blueNew = ((blueSlider->cShape->circle.getPosition().x - sliderRange.x) / max) * 255;

	root["enemy"]["spawn interval"] = spawnNew;
	root["enemy"]["speed min"] = speedNew;
	root["enemy"]["speed max"] = speedNew + 3;
	root["player"]["outline r"] = redNew;
	root["player"]["outline g"] = greenNew;
	root["player"]["outline b"] = blueNew;

	std::ofstream outFile("config.json");
	outFile << root;
	file.close();
	outFile.close();

	EntMgr.destroyEntity(m_player);
	JConfig("config.json");
	spawnPlayer();
}

void Game::Hover(std::shared_ptr<Entity> b)
{
	if (b->cRect->rect.getOutlineColor() != menuHoverColor)
	{
		b->cRect->rect.setOutlineColor(menuHoverColor);
		s.BlipSound();
	}
}

//-------------------------------------------------------------------------------------------------------------
//                                             GAME SYSTEMS
//-------------------------------------------------------------------------------------------------------------

void Game::sEnemySpawner()
{
	enum Bound
	{
		North, East, South, West
	};
	Vec2 spawnPos;
	int b = (rand() % 4);
	Bound randomBound = static_cast<Bound>(b);
	float r = m_player->cShape->circle.getRadius();
	float randX = (rand() % (eastBound - westBound) + westBound);
	float randY = (rand() % (southBound - northBound) + northBound);

	switch (randomBound)
	{
	case North:
		spawnPos = { randX, static_cast<float>(northBound) };
		break;
	case East:
		spawnPos = { static_cast<float>(eastBound), randY };
		break;
	case South:
		spawnPos = { randX, static_cast<float>(southBound) };
		break;
	case West:
		spawnPos = { static_cast<float>(westBound), randY };
		break;
	default:
		std::cerr << "Enum error." << std::endl;
		break;
	}

	if ((m_currentFrame % spawnInt) == 0)
	{
		spawnEnemy(spawnPos);
	}
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event) && state != Closed)
	{
		//ImGui::SFML::ProcessEvent(m_window, event);

		if (event.type == sf::Event::Closed)
		{
			state = Closed;
		}

		if (state == Running)
		{
			if (event.type == sf::Event::KeyPressed)
			{
				//player movement
				if (event.key.code == sf::Keyboard::W)
				{
					m_player->cInput->up = true;
					if (event.key.code == sf::Keyboard::A)
					{
						m_player->cInput->left = true;
					}
					else if (event.key.code == sf::Keyboard::D)
					{
						m_player->cInput->right = true;
					}
				}
				if (event.key.code == sf::Keyboard::A)
				{
					m_player->cInput->left = true;
				}
				if (event.key.code == sf::Keyboard::S)
				{
					m_player->cInput->down = true;
					if (event.key.code == sf::Keyboard::A)
					{
						m_player->cInput->left = true;
					}
					else if (event.key.code == sf::Keyboard::D)
					{
						m_player->cInput->right = true;
					}
				}
				if (event.key.code == sf::Keyboard::D)
				{
					m_player->cInput->right = true;
				}

				//other player input
				if (event.key.code == sf::Keyboard::Space)
				{
					if (pulseReady)
					{
						spawnPulseAttack();
					}
				}
				if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Enter)
				{
					state = Menu;
				}


				if (event.key.code == sf::Keyboard::Escape)
				{
					state = Closed;
				}
			}
			
			//player cease movement
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::W)
				{
					m_player->cInput->up = false;
				}
				if (event.key.code == sf::Keyboard::A)
				{
					m_player->cInput->left = false;
				}
				if (event.key.code == sf::Keyboard::S)
				{
					m_player->cInput->down = false;
				}
				if (event.key.code == sf::Keyboard::D)
				{
					m_player->cInput->right = false;
				}
			}

			//attacks
			if (event.type == sf::Event::MouseButtonPressed)
			{
				float mbx = static_cast<float>(event.mouseButton.x);
				float mby = static_cast<float>(event.mouseButton.y);

				if (event.mouseButton.button == sf::Mouse::Left)
				{
					spawnBullet(Vec2{ mbx, mby });
				}

				if (event.mouseButton.button == sf::Mouse::Right)
				{
					spawnLaser(Vec2{ mbx, mby });
				}
			}
		}


		if (state == Menu)
		{
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				state = Closed;
			}

			if (event.type == sf::Event::MouseMoved)
			{
				sf::Vector2f mousePosition = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
				for (auto b : EntMgr.getEntities("button"))
				{
					if (b->cRect->rect.getGlobalBounds().contains(mousePosition))
					{
						Hover(b);
					}
					else
					{
						b->cRect->rect.setOutlineColor(menuOutlineColor);
					}
				}
			}

			if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
			{
				sf::Vector2f mousePosition = m_window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

				for (auto b : EntMgr.getEntities("button"))
				{
					if (b->cRect->rect.getGlobalBounds().contains(mousePosition))
					{
						b->cRect->rect.setFillColor(menuClickColor);
					}
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				sf::Vector2f mousePosition = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

				for (auto b : EntMgr.getEntities("button"))
				{
					b->cRect->rect.setFillColor(menuColor);

					if (b->cRect->rect.getGlobalBounds().contains(mousePosition))
					{
						s.SelectSound();
						b->cClicked->setClicked();
					}
				}
			}
		}


		if (state == Settings)
		{
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				state = Menu;
			}
			if (event.type == sf::Event::MouseMoved)
			{
				sf::Vector2f mousePosition = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

				if (saveButton->cRect->rect.getGlobalBounds().contains(mousePosition))
				{
					Hover(saveButton);
				}
				else
				{
					saveButton->cRect->rect.setOutlineColor(menuOutlineColor);
				}
			}
			if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Left)
			{
				sf::Vector2f mousePosition = m_window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
				Vec2 mp = Vec2{ mousePosition.x, mousePosition.y };
				for (auto h : EntMgr.getEntities("handle"))
				{
					Vec2 hp = Vec2{ h->cShape->circle.getPosition().x, h->cShape->circle.getPosition().y };
					float h2 = h->cShape->circle.getRadius();
					if (hp.dist(mp) <= (h2 * h2))
					{
						dragging = true;
						activeHandle = h;
					}
					else if (saveButton->cRect->rect.getGlobalBounds().contains(mousePosition))
					{
						saveButton->cRect->rect.setFillColor(menuClickColor);
					}
				}
			}
			if (dragging && sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				activeHandle->cTransform->pos.x = static_cast<float>(sf::Mouse::getPosition(m_window).x);
				if (activeHandle->cShape->circle.getPosition().x < sliderRange.x) {
					activeHandle->cTransform->pos.x = sliderRange.x;
				}
				else if (activeHandle->cShape->circle.getPosition().x > sliderRange.y) {
					activeHandle->cTransform->pos.x = sliderRange.y;
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (dragging = true)
				{
					dragging = false;
				}
				
				sf::Vector2f mousePosition = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
				saveButton->cRect->rect.setFillColor(menuColor);

				if (saveButton->cRect->rect.getGlobalBounds().contains(mousePosition))
				{
					s.SelectSound();
					SaveSettings();
				}
			}

		}
	}
}

void Game::sMovement()
{
	for (auto e : EntMgr.getEntities())
	{
		if (e->tag() != "player")
		{
			e->cTransform->pos += e->cTransform->velocity;
		}
	}

	if (m_player->cInput->up == true)
	{
		m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
	}
	if (m_player->cInput->right == true)
	{
		m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	}
	if (m_player->cInput->down == true)
	{
		m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
	}
	if (m_player->cInput->left == true)
	{
		m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
	}
}

void Game::sCollision()
{
	//bullet and enemy, small enemy collisions
	for (auto bullet : EntMgr.getEntities("bullet"))
	{
		for (auto enemy : EntMgr.getEntities("enemy"))
		{
			if (Physics::isCollision(bullet, enemy))
			{
				int v = enemy->cShape->circle.getPointCount();
				m_player->cScore->incScore(v * 100);
				EntMgr.destroyEntity(bullet);
				spawnSmallEnemies(enemy);
				EntMgr.destroyEntity(enemy);
				s.EnemyDeathSound();
			}
		}
		for (auto smenemy : EntMgr.getEntities("small enemy"))
		{
			if (Physics::isCollision(bullet, smenemy))
			{
				int v = smenemy->cShape->circle.getPointCount();
				m_player->cScore->incScore(v * 200);
				EntMgr.destroyEntity(bullet);
				EntMgr.destroyEntity(smenemy);
			}
		}
	}

	//player and wall collisions
	if (Physics::isBoundsCollisionX(m_player, westBound, eastBound))
	{
		float posX = m_player->cTransform->pos.x;
		if (posX < wWidth / 2)
		{
			m_player->cTransform->pos.x = westBound - 1;
		}
		else if (posX > wWidth / 2)
		{
			m_player->cTransform->pos.x = eastBound + 1;
		}
	}

	if (Physics::isBoundsCollisionY(m_player, northBound, southBound))
	{
		float posY = m_player->cTransform->pos.y;
		if (posY < wHeight / 2)
		{
			m_player->cTransform->pos.y = northBound + 1;
		}
		else if (posY > wHeight / 2)
		{
			m_player->cTransform->pos.y = southBound - 1;
		}
	}

	//enemy and player, enemy and bounds collisions
	for (auto enemy : EntMgr.getEntities("enemy"))
	{
		if (Physics::isCollision(m_player, enemy))
		{
			s.PlayerDeathSound();
			state = DeathPause;
			deathScene = true;

			for (auto e : EntMgr.getEntities("enemy"))
			{
				EntMgr.destroyEntity(e);
			}
		}
		if (Physics::isBoundsCollisionX(enemy, westBound, eastBound) == true)
		{
			Vec2 pos = enemy->cTransform->getPosition();
			if (pos.x < wWidth / 2)
			{
				enemy->cTransform->pos.x = westBound;
				enemy->cTransform->velocity.x *= -1;
			}
			else if (pos.x > wWidth / 2)
			{
				enemy->cTransform->pos.x = eastBound;
				enemy->cTransform->velocity.x *= -1;
			}
		}
		if (Physics::isBoundsCollisionY(enemy, northBound, southBound) == true)
		{
			Vec2 pos = enemy->cTransform->getPosition();
			if (pos.y < wHeight / 2)
			{
				enemy->cTransform->pos.y = northBound;
				enemy->cTransform->velocity.y *= -1;
			}
			else if (pos.y > wHeight / 2)
			{
				enemy->cTransform->pos.y = southBound;
				enemy->cTransform->velocity.y *= -1;
			}
		}
	}

	//small enemy and player collisions
	for (auto smenemy : EntMgr.getEntities("small enemy"))
	{
		if (Physics::isCollision(m_player, smenemy) == true)
		{
			s.PlayerDeathSound();
			state = DeathPause;
			deathScene = true;

			for (auto e : EntMgr.getEntities("enemy"))
			{
				EntMgr.destroyEntity(e);
			}
			for (auto se : EntMgr.getEntities("small enemy"))
			{
				EntMgr.destroyEntity(se);
			}
		}
	}

	if (deathScene)
	{
		KillPlayer();
		--killCountdown;
	}

	//laser and enemy, small enemy collisions
	for (auto bullet : EntMgr.getEntities("laserbullet"))
	{
		for (auto enemy : EntMgr.getEntities("enemy"))
		{
			if (Physics::isCollision(bullet, enemy))
			{
				int v = enemy->cShape->circle.getPointCount();
				m_player->cScore->incScore(v * 100);
				EntMgr.destroyEntity(bullet);
				spawnSmallEnemies(enemy);
				EntMgr.destroyEntity(enemy);
				s.EnemyDeathSound();
			}
		}
		for (auto smenemy : EntMgr.getEntities("small enemy"))
		{
			if (Physics::isCollision(bullet, smenemy))
			{
				int v = smenemy->cShape->circle.getPointCount();
				m_player->cScore->incScore(v * 200);
				EntMgr.destroyEntity(bullet);
				EntMgr.destroyEntity(smenemy);
			}
		}
	}
}

void Game::sLifespan()
{
	for (auto e : EntMgr.getEntities())
	{
		if (e->cLifespan)
		{
			e->cLifespan->decRemaining();
			if (e->cLifespan->remaining == 0)
			{
				EntMgr.destroyEntity(e);
			}
			else
			{
				float newAlpha = (static_cast<float>(e->cLifespan->remaining) / static_cast<float>(e->cLifespan->total) * 255.0f);
				sf::Color oldF = e->cShape->circle.getFillColor();
				e->cShape->circle.setFillColor(sf::Color{ oldF.r, oldF.g, oldF.b, static_cast<uint8_t>(newAlpha) });
				sf::Color oldO = e->cShape->circle.getOutlineColor();
				e->cShape->circle.setOutlineColor(sf::Color{ oldO.r, oldO.g, oldO.b, static_cast<uint8_t>(newAlpha) });
			}
		}
	}
}

void Game::sRender()
{
	m_window.clear();

	std::string myText = "Score: " + std::to_string(m_player->cScore->score);
	scoreText.setString(myText);

	if (state == Menu)
	{
		for (auto e : EntMgr.getEntities("menu"))
		{
			e->cRect->rect.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
			m_window.draw(e->cRect->rect);
		}
		for (auto b : EntMgr.getEntities("button"))
		{
			b->cRect->rect.setPosition(b->cTransform->pos.x, b->cTransform->pos.y);
			m_window.draw(b->cRect->rect);
		}

		m_window.draw(playText);
		m_window.draw(settingsText);
		m_window.draw(exitText);
	}

	else if (state == Settings)
	{
		for (auto m : EntMgr.getEntities("menu"))
		{
			m->cRect->rect.setPosition(m->cTransform->pos.x, m->cTransform->pos.y);
			m_window.draw(m->cRect->rect);
		}
		for (auto s : EntMgr.getEntities("slider"))
		{
			s->cRect->rect.setPosition(s->cTransform->pos.x, s->cTransform->pos.y);
			m_window.draw(s->cRect->rect);
		}
		for (auto h : EntMgr.getEntities("handle"))
		{
			h->cShape->circle.setPosition(h->cTransform->pos.x, h->cTransform->pos.y);
			m_window.draw(h->cShape->circle);
		}

		m_window.draw(diffText);
		m_window.draw(spText);
		m_window.draw(speedText);
		m_window.draw(colorText);
		m_window.draw(redText);
		m_window.draw(greenText);
		m_window.draw(blueText);
		m_window.draw(saveText);
	}

	else
	{
		for (auto e : EntMgr.getEntities())
		{
			if (e->tag() == "pulsar")
			{
				PulseUpdate(e);
				e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
				m_window.draw(e->cShape->circle);
				m_window.draw(particles);
			}
			else if (e->tag() == "enemy" || e->tag() == "player" || e->tag() == "bullet" || e->tag() == "laserbullet" || e->tag() == "small enemy")
			{
				e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
				e->cTransform->angle += 5.0f;
				e->cShape->circle.setRotation(e->cTransform->angle);
				m_window.draw(e->cShape->circle);
			}
			else if (e->tag() == "laser")
			{
				e->cRect->rect.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
				e->cRect->rect.setRotation(e->cTransform->angle);
				LaserUpdate(e);
				m_window.draw(e->cRect->rect);
			}
			else if (e->tag() == "box")
			{
				e->cRect->rect.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
				m_window.draw(e->cRect->rect);
			}
		}
		m_window.draw(scoreText);
	}
	m_window.display();
}
