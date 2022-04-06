#pragma once

#include "Entity.h"

class Player :
	public Entity
{
public:
	Player(QPoint pos, int cellSize, int health = 3, int maxHealth = 3) noexcept;

	int getHP() const noexcept;
	int getMaxHP() const noexcept;
	void UpHP(int hp = 1) noexcept;
	void DownHP(int hp = 1) noexcept;

	operator bool() const noexcept;

	virtual void draw(std::pair<double, double> t0, double l) const noexcept override final;

	virtual void move() noexcept override final;

	virtual bool addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept override final;
private:
	int health, maxHealth;
};