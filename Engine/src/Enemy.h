#pragma once
#include "Xenon2022.cpp"

class Enemy : public GameObject
{
public:
	int healthPoints = 1;
	void showDamageFeedback();
	void hideDamageFeedback();
	void checkDamageFeedback();
	void TakeDamage(int paramFirePower);
private:
	float damageFeedbackTimer = 0;
	float damageFeedbackDuration = 5;
	float damageFeedbackSpeed = 2;
};

