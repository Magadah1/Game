#include "Bonus.h"

Bonus::Bonus(const std::vector<std::vector<bool>>& lvl, BTYPE type, int cellSize) noexcept
	: type(type), cellSize(cellSize)
{
	size_t y = lvl.size(), x = lvl[0].size();

	size_t ry = rand() % y, rx = rand() % x;

	do
	{
		ry = rand() % y;
		rx = rand() % x;
	} while (!lvl[ry][rx]);

	pos = QPoint(static_cast<int>(rx * cellSize + cellSize / 2), static_cast<int>(ry * cellSize + cellSize / 2));
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
			for (size_t i = 0; i <= 50; ++i)
			{
				float a = static_cast<float>(i) / 25. * acos(-1);
				glVertex2f(cos(a) * cellSize / 2 * 0.75 + pos.x(), pos.y() + sin(a) * cellSize / 2 * 0.75);
			}
		}
		glEnd();

		glColor3f(0, 0, 0);
		glBegin(GL_LINE_LOOP);
		{
			for (size_t i = 0; i <= 50; ++i)
			{
				float a = static_cast<float>(i) / 25. * acos(-1);
				glVertex2f(cos(a) * cellSize / 2 * 0.75 + pos.x(), pos.y() + sin(a) * cellSize / 2 * 0.75);
			}
		}

		glBegin(GL_LINES);
		{
			glVertex2f(pos.x(), pos.y());
			glVertex2f(pos.x(), pos.y() + cellSize / 2 * 0.75);

			glVertex2f(pos.x(), pos.y());
			glVertex2f(pos.x() + cellSize / 4, pos.y());
		}
		glEnd();

		break;
	case BTYPE::HP:
		glColor3f(1, 46. / 255, 99. / 255);
		glBegin(GL_QUADS);
		{
			glVertex2f(pos.x() - cellSize / 2 * 0.8, pos.y() - cellSize / 2 * 0.4);
			glVertex2f(pos.x() - cellSize / 2 * 0.8, pos.y() + cellSize / 2 * 0.4);
			glVertex2f(pos.x() + cellSize / 2 * 0.8, pos.y() + cellSize / 2 * 0.4);
			glVertex2f(pos.x() + cellSize / 2 * 0.8, pos.y() - cellSize / 2 * 0.4);

			glVertex2f(pos.x() - cellSize / 2 * 0.4, pos.y() - cellSize / 2 * 0.8);
			glVertex2f(pos.x() - cellSize / 2 * 0.4, pos.y() + cellSize / 2 * 0.8);
			glVertex2f(pos.x() + cellSize / 2 * 0.4, pos.y() + cellSize / 2 * 0.8);
			glVertex2f(pos.x() + cellSize / 2 * 0.4, pos.y() - cellSize / 2 * 0.8);
		}
		glEnd();

		break;
	case BTYPE::FREEZE:
		glColor3f(176. / 255, 224. / 255, 230. / 255);
		glBegin(GL_QUADS);
		{
			glVertex2f(pos.x() - cellSize * 0.6 / 2, pos.y() - cellSize / 2 * 0.25);
			glVertex2f(pos.x() - cellSize * 0.6 / 2, pos.y() + cellSize / 2 * 0.25);
			glVertex2f(pos.x() + cellSize * 0.6 / 2, pos.y() + cellSize / 2 * 0.25);
			glVertex2f(pos.x() + cellSize * 0.6 / 2, pos.y() - cellSize / 2 * 0.25);

		}
		glEnd();
		break;
	case BTYPE::IMMUNITY:
		glColor3f(46. / 255, 58. / 255, 35. / 255);
		glBegin(GL_TRIANGLES);
		{
			glVertex2f(pos.x(), pos.y() - cellSize / 4);
			glVertex2f(pos.x() - cellSize / 4, pos.y() + cellSize / 4);
			glVertex2f(pos.x() + cellSize / 4, pos.y() + cellSize / 4);
		}
		glEnd();
		break;
	default:
		break;
	}
}

Bonus::BTYPE Bonus::getType() const noexcept
{
	return type;
}
