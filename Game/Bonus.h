#pragma once

#include <qpoint.h>
#include <QtOpenGL>
#include "glut.h"
#include <vector>


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

	void draw() const noexcept;

	BTYPE getType() const noexcept;

private:
	QPoint pos;

	int cellSize;

	BTYPE type;
};
