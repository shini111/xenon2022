#include "Engine.h"

#undef main

GameEngine::Engine engine;
Input input;

float globalRotation = 90.0f;

bool CompanionListBool[2] = { false,false };

class Enemy : public GameObject {
public:
	int healthPoints = 1;



	void showDamageFeedback() {
		modulate.r = 255;
		modulate.g = 0;
		modulate.b = 0;
	}

	void hideDamageFeedback() {
		modulate.r = 255;
		modulate.g = 255;
		modulate.b = 255;
	}

	void checkDamageFeedback() {
		if (damageFeedbackTimer > 0) {
			damageFeedbackTimer -= damageFeedbackDuration * damageFeedbackSpeed * engine.deltaTime;

			if (int(damageFeedbackTimer) % 2 == 0) {
				showDamageFeedback();
			}
			else {
				hideDamageFeedback();
			}

		}
		else
		{
			hideDamageFeedback();
			damageFeedbackTimer = 0;
		}
	}

	void TakeDamage(int paramFirePower) {
		healthPoints -= paramFirePower;



		if (healthPoints <= 0) {
			Destroy();

		}
		else {

			showDamageFeedback();
			damageFeedbackTimer = damageFeedbackDuration;

		}
	}
private:
	float damageFeedbackTimer = 0;
	float damageFeedbackDuration = 5;
	float damageFeedbackSpeed = 2;

};

class explosion : public GameObject {
public:

	void OnStart() override {

		int textureDimentions[2] = { 5,2 };

		animation = Animation("resources/graphics/explode64.bmp", 0.1f, textureDimentions, false, {});
	}

	void OnAnimationFinish() override {
		Destroy();
	}

};


class missile : public GameObject {
public:
	float moveSpeed = -250.0f;

	int firePower = 0;
	int missileDamage = 1;

	void OnStart() override {
		int textureDimentions[2] = { 2,3 };

		switch (firePower) {
		case 0:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,0),AnimationCoord(1,0) });
			break;
		case 1:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,1),AnimationCoord(1,1) });
			break;
		case 2:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,2),AnimationCoord(1,2) });
			break;
		default:
			animation = Animation("resources/graphics/missile.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,0),AnimationCoord(1,0) });
		}

		collisionBoxSize.w = collisionBoxSize.h = 16.0f;

		objectGroup = "bullet";

		rotation = globalRotation;

	}

	int getMissileDamage() {
		int damage = 1;

		switch (firePower) {
		case 0:
			damage = 1;
			break;
		case 1:
			damage = 2;
			break;
		case 2:
			damage = 4;
			break;

		}

		return damage;
	}

	void OnUpdate() override {
		position.x -= moveSpeed * engine.deltaTime;

		if (position.x > 640) {
			Destroy();
		}
	}

};

class rusher : public Enemy {
public:
	float moveSpeed = -150.0f;
	void OnStart() override {

		healthPoints = 2;

		int textureDimentions[2] = { 4,6 };

		animation = Animation("resources/graphics/rusher.bmp", 0.05f, textureDimentions, true, {});
		objectGroup = "enemy";

		collisionBoxSize.w = 48.0f;
		collisionBoxSize.h = 32.0f;

		rotation = globalRotation;
	}

	void OnUpdate() override {
		position.x += moveSpeed * engine.deltaTime;

		if (position.x < -64) {
			Destroy();
		}

		checkDamageFeedback();
	}

	void OnCollideEnter(GameObject& contact) override {
		if (contact.objectGroup == "bullet") {



			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);

			if (missile* missileContact = dynamic_cast<missile*>(&contact)) {

				int missileFirePower = missileContact->getMissileDamage();
				TakeDamage(missileFirePower);
			}

			contact.Destroy();
		}
	}

};

class drone : public Enemy {
public:
	float moveSpeed = -60.0f;
	float sinOffset = 0.0f;
	int packID = 0;

	void OnStart() override {

		healthPoints = 1;

		int textureDimentions[2] = { 8,2 };

		animation = Animation("resources/graphics/drone.bmp", 0.1f, textureDimentions, true, {});
		objectGroup = "enemy";

		sinOffset -= (packID * 32);
	}

	void OnUpdate() override {

		position.x += moveSpeed * engine.deltaTime;

		sinOffset += 2 * engine.deltaTime;
		position.y = position.y - sin(sinOffset);


		if (position.x < -32) {
			Destroy();
		}

		checkDamageFeedback();

	}

	void OnCollideEnter(GameObject& contact) override {
		if (contact.objectGroup == "bullet") {

			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);

			if (missile* missileContact = dynamic_cast<missile*>(&contact)) {

				int missileFirePower = missileContact->getMissileDamage();

				TakeDamage(missileFirePower);
			}

			contact.Destroy();

		}
	}

};

class enemyProjectile : public GameObject {
public:
	float moveSpeed = -250.0f;


	void OnStart() override {
		int textureDimentions[2] = { 8,1 };

		animation = Animation("resources/graphics/EnWeap6.bmp", 0.1f, textureDimentions, true, {});
		objectGroup = "enemyBullet";

		collisionBoxSize.w = collisionBoxSize.h = 16.0f;
	}

	void OnUpdate() override {
		position.x += moveSpeed * engine.deltaTime;


		if (position.x < -16) {
			Destroy();

		}
	}

};

class metalAsteroid : public GameObject {
public:
	float moveSpeed = -60.0f;
	int asteroidSize = 32;

	void OnStart() override {

		int textureDimentions[2] = { 8,2 };
		animation = Animation("resources/graphics/MAster32.bmp", 0.1f, textureDimentions, true, {});

		objectGroup = "enemy";

		switch (asteroidSize) {
		case 64:
			textureDimentions[0] = 8;
			textureDimentions[1] = 3;
			animation = Animation("resources/graphics/MAster64.bmp", 0.1f, textureDimentions, true, {});
			break;
		case 96:
			textureDimentions[0] = 5;
			textureDimentions[1] = 5;
			animation = Animation("resources/graphics/MAster96.bmp", 0.1f, textureDimentions, true, {});
			break;
		default:
			textureDimentions[0] = 8;
			textureDimentions[1] = 2;
			animation = Animation("resources/graphics/MAster32.bmp", 0.1f, textureDimentions, true, {});
		}

		collisionBoxSize.w = collisionBoxSize.h = asteroidSize;
		rotation = globalRotation;
	}

	void OnUpdate() override {
		position.x += moveSpeed * engine.deltaTime;

		if (position.x < -96) {
			Destroy();

		}

	}

	void OnCollideEnter(GameObject& contact) override {

		if (contact.objectGroup == "bullet") {
			contact.Destroy();

		}
	}


};

class stoneAsteroid : public Enemy {
public:

	struct
	{
		float x = -60.0f;
		float y = 0.0f;
	}moveSpeed;

	int asteroidSize = 32;

	void OnStart() override {

		int textureDimentions[2];

		objectGroup = "enemy";

		switch (asteroidSize) {
		case 64:
			healthPoints = 3;

			textureDimentions[0] = 8;
			textureDimentions[1] = 3;
			animation = Animation("resources/graphics/SAster64.bmp", 0.1f, textureDimentions, true, {});
			break;
		case 96:
			healthPoints = 6;

			textureDimentions[0] = 5;
			textureDimentions[1] = 5;
			animation = Animation("resources/graphics/SAster96.bmp", 0.1f, textureDimentions, true, {});
			break;
		default:
			healthPoints = 1;

			textureDimentions[0] = 8;
			textureDimentions[1] = 2;
			animation = Animation("resources/graphics/SAster32.bmp", 0.1f, textureDimentions, true, {});
		}

		collisionBoxSize.w = collisionBoxSize.h = asteroidSize;
		rotation = globalRotation;
	}

	void OnDestroyed() override {
		stoneAsteroid* asteroid1 = new stoneAsteroid();
		stoneAsteroid* asteroid2 = new stoneAsteroid();
		stoneAsteroid* asteroid3 = new stoneAsteroid();

		switch (asteroidSize) {
		case 64:

			asteroid1->position.x = position.x;
			asteroid1->position.y = position.y;
			asteroid1->asteroidSize = 32;
			asteroid1->moveSpeed.y = 32.0f;

			asteroid2->position.x = position.x;
			asteroid2->position.y = position.y;
			asteroid2->asteroidSize = 32;
			asteroid2->moveSpeed.y = -32.0f;

			asteroid3->position.x = position.x;
			asteroid3->position.y = position.y;
			asteroid3->asteroidSize = 32;
			asteroid3->moveSpeed.x = -128.0f;

			engine.getLevel().addObject(asteroid1);
			engine.getLevel().addObject(asteroid2);
			engine.getLevel().addObject(asteroid3);

			break;
		case 96:

			asteroid1->position.x = position.x;
			asteroid1->position.y = position.y;
			asteroid1->asteroidSize = 64;
			asteroid1->moveSpeed.y = 32.0f;

			asteroid2->position.x = position.x;
			asteroid2->position.y = position.y;
			asteroid2->asteroidSize = 64;
			asteroid2->moveSpeed.y = -32.0f;

			asteroid3->position.x = position.x;
			asteroid3->position.y = position.y;
			asteroid3->asteroidSize = 64;
			asteroid3->moveSpeed.x = -128.0f;

			engine.getLevel().addObject(asteroid1);
			engine.getLevel().addObject(asteroid2);
			engine.getLevel().addObject(asteroid3);
			break;
		default:

			break;
		}

	}

	void OnUpdate() override {

		position.x += moveSpeed.x * engine.deltaTime;
		position.y += moveSpeed.y * engine.deltaTime;

		if (position.x < -96) {
			Destroy();
		}

		checkDamageFeedback();

	}

	void OnCollideEnter(GameObject& contact) override {

		if (contact.objectGroup == "bullet") {

			stoneAsteroid* asteroid1 = new stoneAsteroid();
			stoneAsteroid* asteroid2 = new stoneAsteroid();

			switch (asteroidSize) {
			case 64:

				if (missile* missileContact = dynamic_cast<missile*>(&contact)) {

					int missileFirePower = missileContact->getMissileDamage();

					TakeDamage(missileFirePower);
				}

				contact.Destroy();

				break;
			case 96:

				if (missile* missileContact = dynamic_cast<missile*>(&contact)) {

					int missileFirePower = missileContact->getMissileDamage();

					TakeDamage(missileFirePower);
				}

				contact.Destroy();
				break;
			default:

				if (missile* missileContact = dynamic_cast<missile*>(&contact)) {

					int missileFirePower = missileContact->getMissileDamage();

					TakeDamage(missileFirePower);
				}

				contact.Destroy();

			}

			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);

		}
	}


};

class loner : public Enemy {
public:
	float moveSpeed = 70.0f;
	float time = 0.0f;
	float timeCooldown = 2.0f;

	void OnStart() override {

		healthPoints = 3;

		int textureDimentions[2] = { 4,4 };

		animation = Animation("resources/graphics/LonerA.bmp", 0.05f, textureDimentions, true, {});
		objectGroup = "enemy";


		collisionBoxSize.w = collisionBoxSize.h = 64.0f;
		rotation = globalRotation;
	}

	void OnCollideEnter(GameObject& contact) override {
		if (contact.objectGroup == "bullet") {



			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);

			if (missile* missileContact = dynamic_cast<missile*>(&contact)) {

				int missileFirePower = missileContact->getMissileDamage();

				TakeDamage(missileFirePower);
			}

			contact.Destroy();

		}
	}


	void OnUpdate() override {
		time += 1 * engine.deltaTime;

		if (time > timeCooldown) {
			enemyProjectile* enemyProj = new enemyProjectile();
			enemyProj->position.x = position.x - 16;
			enemyProj->position.y = position.y + 24;
			engine.getLevel().addObject(enemyProj);

			time = 0;
		}

		position.y += moveSpeed * engine.deltaTime;

		checkDamageFeedback();

		if (position.y > 480) {
			Destroy();
		}

	}

};


class powerUpMissile : public GameObject {
public:
	float moveSpeed = -30.0f;


	void OnStart() override {
		int textureDimentions[2] = { 4,2 };

		animation = Animation("resources/graphics/PUWeapon.bmp", 0.1f, textureDimentions, true, {});
		objectGroup = "powerUpMissile";

	}

	void OnUpdate() override {
		position.x += moveSpeed * engine.deltaTime;

		if (position.x < -32) {
			Destroy();
		}

	}

};

class powerUpHeal : public GameObject {
public:
	float moveSpeed = -30.0f;


	void OnStart() override {
		int textureDimentions[2] = { 4,2 };

		animation = Animation("resources/graphics/PUShield.bmp", 0.1f, textureDimentions, true, {});
		objectGroup = "powerUpHeal";

	}

	void OnUpdate() override {
		position.x += moveSpeed * engine.deltaTime;

		if (position.x < -32) {
			Destroy();
		}

	}

};

class powerUpCompanion : public GameObject {
public:
	float moveSpeed = -30.0f;


	void OnStart() override {
		int textureDimentions[2] = { 4,5 };

		animation = Animation("resources/graphics/clone.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,0),AnimationCoord(1,0),AnimationCoord(2,0),AnimationCoord(3,0),
		AnimationCoord(0,1),AnimationCoord(1,1),AnimationCoord(2,1),AnimationCoord(3,1),AnimationCoord(0, 2), AnimationCoord(1, 2), AnimationCoord(2, 2), AnimationCoord(3, 2),
		AnimationCoord(0, 3), AnimationCoord(1, 3), AnimationCoord(2, 3), AnimationCoord(3, 3) });
		objectGroup = "powerUpCompanion";

	}

	void OnUpdate() override {

		position.x += moveSpeed * engine.deltaTime;

		if (position.x < -32) {
			Destroy();
		}
	}

};

class ally : public Pawn {
public:

	int shipHealthMax = 5;
	int shipHealth = 5;

	bool keyPressed = false;

	int firePower = 0;

	int positionOffset = 0;

	struct
	{
		int x = 0;
		int y = 0;
	}bulletOffset;


	void TakeShipDamage() {
		if (damageCooldown <= 0)
		{
			shipHealth -= 1;
			damageCooldown = damageCooldownDefault;
		}
	}

	void checkDamageCooldown() {
		if (damageCooldown > 0)
		{
			damageCooldown -= 1 * engine.deltaTime;
		}
		else {
			damageCooldown = 0;
		}
		//std::cout << damageCooldown << "\n";
	}

	void HealShip() {
		int healAmount = 2;

		shipHealth += healAmount;

		if (shipHealth > shipHealthMax) {
			shipHealth = shipHealthMax;
		}
	}
	void UpgradeFirePower() {
		if (firePower < 2) {
			firePower++;

		}
	}

	void ShootCheck() {

		if (GetAsyncKeyState(input.ShootAction[0]) & 0x8000) {
			if (!keyPressed) {
				missile* bullet = new missile();
				bullet->position.x = position.x + bulletOffset.x;
				bullet->position.y = position.y + bulletOffset.y;
				bullet->firePower = firePower;
				engine.getLevel().addObject(bullet);

				keyPressed = true;
			}
		}
		else
		{
			keyPressed = false;
		}

	};
private:
	float damageCooldownDefault = 1;
	float damageCooldown = 0;
};

class companion : public ally {
public:

	GameObject* playerShip;

	int companionId = 0;
	bool recruted;

	void OnStart() override {

		playerShip = engine.getLevel().levelObjects[0];

		shipHealthMax = 3;
		shipHealth = 3;
		keyPressed = false;
		firePower = 0;
		positionOffset = -50;

		bulletOffset.x = 16;
		bulletOffset.y = 9;

		recruted = false;

		position.x = 0;
		position.y = 1000.f;

		int textureDimentions[2] = { 4,5 };
		animation = Animation("resources/graphics/clone.bmp", 0.1f, textureDimentions, true, { AnimationCoord(0,0),AnimationCoord(1,0),AnimationCoord(2,0),AnimationCoord(3,0),
		AnimationCoord(0,1),AnimationCoord(1,1),AnimationCoord(2,1),AnimationCoord(3,1),AnimationCoord(0, 2), AnimationCoord(1, 2), AnimationCoord(2, 2), AnimationCoord(3, 2),
		AnimationCoord(0, 3), AnimationCoord(1, 3), AnimationCoord(2, 3), AnimationCoord(3, 3) });
		objectGroup = "companion";
		collisionBoxSize.w = collisionBoxSize.h = 32.0f;
		rotation = globalRotation;
	}

	void SetPosition()
	{
		if (companionId == 1)
		{
			positionOffset = 85;
		}
		position.x = playerShip->position.x;
		position.y = playerShip->position.y + positionOffset;

	}

	void OnUpdate() override {

		if (recruted)
		{
			ShootCheck();
			SetPosition();
			checkDamageCooldown();


			if (shipHealth == 0)
			{
				if (companionId == 1)
				{
					CompanionListBool[1] = false;
				}

				if (companionId == 0)
				{
					CompanionListBool[0] = false;
				}
				firePower = 0;
				recruted = false;
				shipHealth = shipHealthMax;
				position.x = 0.0f;
				position.y = 1000.0f;
			}

		}
	}

	void OnCollideEnter(GameObject& contact) override {
		if (contact.objectGroup == "enemyBullet") {
			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);
			contact.Destroy();
			TakeShipDamage();


		}

		if (contact.objectGroup == "powerUpMissile") {
			UpgradeFirePower();
			contact.Destroy();
		}

		if (contact.objectGroup == "enemy") {
			TakeShipDamage();
		}

		if (contact.objectGroup == "powerUpHeal") {
			HealShip();
			contact.Destroy();
		}
	}

};

companion* companionList[2];

class spaceship : public ally {
public:

	std::string currentAnimation = "";
	int animationState = 0;

	bool isGameOver = false;

	void OnStart() override {
		int textureDimentions[2] = { 7,1 };

		shipHealthMax = 5;
		shipHealth = 5;
		keyPressed = false;
		firePower = 0;

		movementSpeed = 200.0f;

		bulletOffset.x = 64;
		bulletOffset.y = 24;

		animationState = 0;
		objectGroup = "player";

		position.x = 64.0f;
		position.y = 208.0f;

		collisionBoxSize.w = collisionBoxSize.h = 64.0f;
		rotation = globalRotation;
	}

	void RecruitCompanion()
	{
		for (int i = 0; i < 2; i++)
		{
			if (CompanionListBool[i] == false)
			{
				companionList[i]->recruted = true;
				companionList[i]->SetPosition();

				CompanionListBool[i] = true;

				break;
			}
		}

	}

	void OnUpdate() override {

		int textureDimentions[2] = { 7,1 };


		if (isGameOver == false)
		{
			ShootCheck();
			checkDamageCooldown();

			if (GetKeyState(input.MoveLeft[0]) & 0x8000)
			{
				position.x -= movementSpeed * engine.deltaTime;
			}
			else if (GetKeyState(input.MoveRight[0]) & 0x8000)
			{
				position.x += movementSpeed * engine.deltaTime;

			}


			if (GetKeyState(input.MoveUp[0]) & 0x8000)
			{
				position.y -= movementSpeed * engine.deltaTime;
				animationState = 1;
			}
			else if (GetKeyState(input.MoveDown[0]) & 0x8000)
			{
				position.y += movementSpeed * engine.deltaTime;
				animationState = 2;
			}
			else
			{
				animationState = 0;
			}

		}

		if (animationState == 1 && currentAnimation != "Up")
		{
			currentAnimation = "Up";

			animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false, { AnimationCoord(4,0),AnimationCoord(5,0),AnimationCoord(6,0) });
			animation.spriteIndex = 0;
		}
		else if (animationState == 2 && currentAnimation != "Down")
		{
			currentAnimation = "Down";
			animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false, { AnimationCoord(2,0),AnimationCoord(1,0),AnimationCoord(0,0) });
			animation.spriteIndex = 0;
		}
		else if (animationState == 0 && currentAnimation != "Idle")
		{
			currentAnimation = "Idle";
			animation = Animation("resources/graphics/Ship1.bmp", 0.1f, textureDimentions, false, { AnimationCoord(3,0) });
			animation.spriteIndex = 0;
		}


		if (shipHealth <= 0 && isGameOver == false) {
			isGameOver = true;

			CompanionListBool[0] = false;
			CompanionListBool[1] = false;

			if (companion* companionObject = dynamic_cast<companion*>(engine.getLevel().levelObjects[1])) {
				companionObject->recruted = false;
				companionObject->position.x = 0.0f;
				companionObject->position.y = 1000.0f;
			}

			if (companion* companionObject = dynamic_cast<companion*>(engine.getLevel().levelObjects[2])) {
				companionObject->recruted = false;
				companionObject->position.x = 0.0f;
				companionObject->position.y = 1000.0f;
			}

			position.x = 1000.0f;
			position.y = 1000.0f;
		}
	}

	void OnCollideEnter(GameObject& contact) override {
		if (contact.objectGroup == "enemyBullet") {
			explosion* boom = new explosion();
			boom->position.x = position.x;
			boom->position.y = position.y;
			engine.getLevel().addObject(boom);
			TakeShipDamage();
			contact.Destroy();
		}

		if (contact.objectGroup == "powerUpMissile") {
			UpgradeFirePower();
			contact.Destroy();
		}

		if (contact.objectGroup == "enemy") {
			TakeShipDamage();
			//engine.print("a");
		}

		if (contact.objectGroup == "powerUpHeal") {
			HealShip();
			contact.Destroy();
		}

		if (contact.objectGroup == "powerUpCompanion") {
			RecruitCompanion();
			contact.Destroy();
		}
	}

};

int main()
{
	//GameWindow gameWindow("Xenon 2000",640,480);

	GameWindow gameWindow;
	gameWindow.windowName = "Xenon 2000";
	gameWindow.windowWidth = 640;
	gameWindow.windowHeight = 480;

	//Game game;



	// Input Mapping

	input.ShootAction = { VK_SPACE };
	input.MoveDown = { VK_DOWN };
	input.MoveLeft = { VK_LEFT };
	input.MoveRight = { VK_RIGHT };
	input.MoveUp = { VK_UP };

	GameLevel level;

	LevelBackground backgroundLayer1;

	backgroundLayer1.background_path = "resources/graphics/galaxy2.bmp";
	backgroundLayer1.scrollingSpeed = 0;

	LevelBackground backgroundLayer2;

	backgroundLayer2.background_path = "resources/graphics/GAster96.bmp";
	backgroundLayer2.scrollingSpeed = -10;
	backgroundLayer2.scrollingDirection = backgroundLayer2.horizontal;

	//engine.print("print test");

	level.setLayerSize(2);
	level.background[0] = backgroundLayer1;
	level.background[1] = backgroundLayer2;

	engine.setLevel(level);

	spaceship* ship = new spaceship();

	companion* cloneA = new companion();
	companionList[0] = cloneA;
	companion* cloneB = new companion();
	cloneB->companionId = 1;
	companionList[1] = cloneB;

	rusher* enemyA = new rusher();
	enemyA->position.x = 650.0f;
	enemyA->position.y = 150.0f;

	rusher* enemyB = new rusher();
	enemyB->position.x = 600.0f;
	enemyB->position.y = 200.0f;

	rusher* enemyC = new rusher();
	enemyC->position.x = 4150.0f;
	enemyC->position.y = 150.0f;

	rusher* enemyD = new rusher();
	enemyD->position.x = 4100.0f;
	enemyD->position.y = 200.0f;

	loner* lonerA = new loner();
	lonerA->position.x = 340.0f;
	lonerA->position.y = -250.0f;

	loner* lonerB = new loner();
	lonerB->position.x = 240.0f;
	lonerB->position.y = 250.0f;
	lonerB->moveSpeed = 0.0f;

	metalAsteroid* metalAsteroidA = new metalAsteroid();
	metalAsteroidA->position.x = 1500.0f;
	metalAsteroidA->position.y = 150.0f;
	metalAsteroidA->asteroidSize = 96;

	metalAsteroid* metalAsteroidB = new metalAsteroid();
	metalAsteroidB->position.x = 1400.0f;
	metalAsteroidB->position.y = 350.0f;
	metalAsteroidB->asteroidSize = 32;

	stoneAsteroid* stoneAsteroidA = new stoneAsteroid();
	stoneAsteroidA->position.x = 1300.0f;
	stoneAsteroidA->position.y = 200.0f;
	stoneAsteroidA->asteroidSize = 96;

	powerUpMissile* powerUpA = new powerUpMissile();
	powerUpA->position.x = 300.0f;
	powerUpA->position.y = 270.0f;

	powerUpMissile* powerUpB = new powerUpMissile();
	powerUpB->position.x = 650.0f;
	powerUpB->position.y = 80.0f;

	powerUpMissile* powerUpC = new powerUpMissile();
	powerUpC->position.x = 900.0f;
	powerUpC->position.y = 90.0f;

	powerUpHeal* powerUpHealA = new powerUpHeal();
	powerUpHealA->position.x = 1650.0f;
	powerUpHealA->position.y = 190.0f;

	powerUpHeal* powerUpHealB = new powerUpHeal();
	powerUpHealB->position.x = 1050.0f;
	powerUpHealB->position.y = 190.0f;

	powerUpHeal* powerUpHealC = new powerUpHeal();
	powerUpHealC->position.x = 1150.0f;
	powerUpHealC->position.y = 250.0f;

	powerUpCompanion* powerUpCompanionA = new powerUpCompanion();
	powerUpCompanionA->position.x = 1300.0f;
	powerUpCompanionA->position.y = 50.0f;

	powerUpCompanion* powerUpCompanionB = new powerUpCompanion();
	powerUpCompanionB->position.x = 1150.0f;
	powerUpCompanionB->position.y = 400.0f;

	powerUpCompanion* powerUpCompanionC = new powerUpCompanion();
	powerUpCompanionC->position.x = 400.0f;
	powerUpCompanionC->position.y = 32.0f;

	powerUpCompanion* powerUpCompanionD = new powerUpCompanion();
	powerUpCompanionD->position.x = 800.0f;
	powerUpCompanionD->position.y = 32.0f;

	engine.getLevel().addObject(ship);

	engine.getLevel().addObject(cloneA);
	engine.getLevel().addObject(cloneB);

	engine.getLevel().addObject(enemyA);
	engine.getLevel().addObject(enemyB);
	engine.getLevel().addObject(enemyC);
	engine.getLevel().addObject(enemyD);
	engine.getLevel().addObject(lonerA);

	engine.getLevel().addObject(metalAsteroidA);
	engine.getLevel().addObject(metalAsteroidB);
	engine.getLevel().addObject(stoneAsteroidA);

	engine.getLevel().addObject(powerUpA);
	engine.getLevel().addObject(powerUpB);
	engine.getLevel().addObject(powerUpC);


	engine.getLevel().addObject(powerUpHealA);
	engine.getLevel().addObject(powerUpHealB);
	engine.getLevel().addObject(powerUpHealC);

	engine.getLevel().addObject(powerUpCompanionA);
	engine.getLevel().addObject(powerUpCompanionB);
	engine.getLevel().addObject(powerUpCompanionC);
	engine.getLevel().addObject(powerUpCompanionD);

	//engine.getLevel().addObject(lonerB);

	//Drone Pack

	for (int i = 0; i < 7; ++i)
	{

		drone* dronePack = new drone();
		dronePack->position.x = 800.0f + (i * 35);
		dronePack->position.y = 350.0f;
		dronePack->packID = i;

		engine.getLevel().addObject(dronePack);

	}


	engine.Initialize(gameWindow);

}

