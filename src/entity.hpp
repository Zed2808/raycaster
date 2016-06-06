#ifndef ENTITY_HPP
#define ENTITY_HPP

class Entity {
	public:
		double x;
		double y;
		int texture;
};

class Enemy : public Entity {
	public:
		Enemy(double x, double y, int texture);
};

#endif
