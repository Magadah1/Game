#include "Bonus.h"

Bonus::Bonus(const std::vector<std::vector<bool>>& lvl, BTYPE type, int cellSize) noexcept
	: type(type), cellSize(cellSize)
{
	size_t y = lvl.size(), x = lvl[0].size();

	size_t ry = rand() % y, rx = rand() % x;

	while (!lvl[y][x])
	{
		ry = rand() % y;
		rx = rand() % x;
	}

	pos = QPoint(static_cast<int>(ry * cellSize + cellSize / 2), static_cast<int>(rx * cellSize + cellSize / 2));
}

std::pair<int, int> Bonus::getIndexPos() const noexcept
{
	return std::pair<int, int>(pos.x() / cellSize, pos.y() / cellSize);
}

void Bonus::draw() const noexcept
{
	// test
	switch (type)
	{
	case BTYPE::TIME:
		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex2f(pos.x(), pos.y());
			for (size_t i = 0; i < 50; ++i)
			{
				float a = static_cast<float>(i) / 25. * acos(-1);
				glVertex2f(cos(a) * cellSize * 0.75, sin(a) * cellSize * 0.75);
			}
		}
		glEnd();

		glLineWidth(2);
		glColor3f(0, 0, 0);
		glBegin(GL_LINE_LOOP);
		{
			for (size_t i = 0; i < 50; ++i)
			{
				float a = static_cast<float>(i) / 25. * acos(-1);
				glVertex2f(cos(a) * cellSize * 0.75, sin(a) * cellSize * 0.75);
			}
		}
		glLineWidth(1);

		glBegin(GL_LINES);
		{
			glVertex2f(pos.x(), pos.y());
			glVertex2f(pos.x(), pos.y() + cellSize);

			glVertex2f(pos.x(), pos.y());
			glVertex2f(pos.x() + cellSize / 2, pos.y());
		}
		glEnd();

		break;
	case BTYPE::HP:
		glColor3f(1, 46. / 255, 99. / 255);
		break;
	case BTYPE::FREEZE:
		break;
	default:
		break;
	}
}

Bonus::BTYPE Bonus::getType() const noexcept
{
	return type;
}
