#pragma once

#include "Entity.h"

class Diamond :
	public Entity
{
public:
	Diamond(QPoint pos, int cellSize) noexcept;

	virtual void draw(std::pair<double, double> t0, double l) const noexcept override final;

private:
	virtual bool addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept override final;

	virtual void move() noexcept override final;
};

