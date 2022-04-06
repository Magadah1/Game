#pragma once

#include <qpoint.h>
#include <QtOpenGL>
#include "glut.h"
#include <vector>
#include "Entity.h"


class Bonus
{
public:
	typedef enum class BONUS_TYPE
	{
		TIME,
		HP,
		FREEZE,
		IMMUNITY
	} BTYPE;

	Bonus(const std::vector<std::vector<bool>>& lvl, BTYPE type, int cellSize) noexcept;

	std::pair<int, int> getIndexPos() const noexcept;

	void draw(std::pair<double, double> t0, double l) const noexcept;

	BTYPE getType() const noexcept;

private:
	QPoint pos;

	int cellSize;

	BTYPE type;

	mutable int heroPar;
};
