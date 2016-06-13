#include "entity.hpp"

/*
 * Enemies
 */
Imp::Imp(double x, double y) {
	this->x = x;
	this->y = y;
	this->texture = 0;
	scaleX = 0.8;
	scaleY = 0.8;
	moveY = -256 * scaleY + 256;
}

/*
 * Explosive Barrel
 */
Barrel::Barrel(double x, double y, int texture) {
	this->x = x;
	this->y = y;
	this->texture = texture;
	scaleX = 1.5;
	scaleY = 1.5;
	moveY = -256 * scaleY + 256;

	health = 20;
}

void Barrel::damage(int damage) {
	health -= damage;
	if(health <= 0) {
		// explode
	}
}
