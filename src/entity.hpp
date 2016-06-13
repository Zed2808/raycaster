#ifndef ENTITY_HPP
#define ENTITY_HPP

/*
 * Base Entity
 */
class Entity {
	public:
		// map coordinates
		double x;
		double y;

		// texture
		int texture;

		// sprite scaling
		double scaleX;
		double scaleY;
		double moveY;
};

/*
 * Enemies
 */
class Imp : public Entity {
	public:
		Imp(double x, double y);
};

/*
 * Explosive barrel
 */
class Barrel : public Entity {
	private:
		int health;
	public:
		Barrel(double x, double y, int texture);
		void damage(int damage);
};

#endif
