#include "Bonus.h"

Bonus::Bonus(const std::vector<std::vector<bool>>& lvl, BTYPE type, int cellSize) noexcept
	: type(type), cellSize(cellSize), heroPar(0)
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

void Bonus::draw(std::pair<double, double> t0, double l) const noexcept
{
	auto [x0, y0] = t0;
	glPushMatrix();
	{
		glTranslated(x0 + pos.x(), y0 + pos.y(), 10 * l);
		glRotated(heroPar * 3, 0, 0, 1);
		switch (type)
		{
		case BTYPE::TIME:
		{
			l /= 4;
			GLUquadricObj* obj;
			obj = gluNewQuadric();
			int r = 20, h = 5, k = 5;

			glColor3f(0 / 255.0, 0 / 255.0, 0 / 255.0);

			glRotated(90, 1, 0, 0);

			for (int i = 0; i < 12; ++i)        //часы
			{
				glRotated(30, 0, 0, 1);
				glTranslated(0, r * l * 0.83, 0);

				Entity::Draw_Parallepiped(0, 0, 0, r * l * 0.07, r * l * 0.3, h * l / 1.99, 10, 10);

				glTranslated(0, -r * l * 0.83, 0);
			}

			glColor3f(100 / 255.0, 100 / 255.0, 100 / 255.0);

			for (int i = 0; i < 60; ++i)        //минуты
			{
				glRotated(6, 0, 0, 1);
				glTranslated(0, r * l * 0.85, 0);

				if (i % 5)
				{
					Entity::Draw_Parallepiped(0, 0, 0, r * l * 0.02, r * l * 0.1, h * l / 1.99, 10, 10);
				}

				glTranslated(0, -r * l * 0.85, 0);
			}

			glTranslated(0, 0, -1 * h * l / 2);
			gluCylinder(obj, r * l, r * l, h * l, 20, 20);     //черная обводка


			glColor3f(255 / 255.0, 255 / 255.0, 255 / 255.0);   //белый фон часиков

			glTranslated(0, 0, h * l / 4);
			gluDisk(obj, 0, r * l, 20, 20);


			glTranslated(0, 0, 2 * l * h / 4);
			gluDisk(obj, 0, r * l, 20, 20);
			gluDeleteQuadric(obj);
		}
			break;
		case BTYPE::HP:
		{
			glColor3f(255 / 255.0, 36 / 255.0, 0 / 255.0);
			Entity::Draw_Parallepiped(0, 0, 0, 3 * l, 3 * l, 10 * l, 10, 10); //палка номер 1   
			Entity::Draw_Parallepiped(0, 0, 0, 10 * l, 3 * l, 3 * l, 10, 10); //палка номер 2
		}
		break;
		case BTYPE::FREEZE:
		{
			l /= 4;
			glRotated(20, 1, 0, 0);
			GLUquadricObj* obj;
			obj = gluNewQuadric();

			int r = 5, h = 15, k = 2;

			//r /= 4;
			//h /= 4;
			//k /= 4;

			glColor3f(250 / 255.0, 210 / 255.0, 176 / 255.0); //песок

			gluCylinder(obj, r * l / 5, r * l, h * l, 20, 20);
			glTranslated(0, 0, h * l);

			glColor3f(52 / 255.0, 34 / 255.0, 17 / 255.0);   //верхнее основание

			gluDisk(obj, 0, r * l * k, 20, 20);
			gluCylinder(obj, r * l * k, r * l * k, h * l * 0.25, 20, 20);
			glTranslated(0, 0, h * l * 0.25);
			gluDisk(obj, 0, r * l * k, 20, 20);

			glTranslated(0, 0, h * l * -1.25);
			glRotated(180, 1, 0, 0);

			glColor3f(250 / 255.0, 210 / 255.0, 176 / 255.0); //песок

			gluCylinder(obj, r * l / 5, r * l, h * l, 20, 20);
			glTranslated(0, 0, h * l);

			glColor3f(52 / 255.0, 34 / 255.0, 17 / 255.0);     //нижнее основание

			gluDisk(obj, 0, r * l * k, 20, 20);
			gluCylinder(obj, r * l * k, r * l * k, h * l * 0.25, 20, 20);
			glTranslated(0, 0, h * l * 0.25);
			gluDisk(obj, 0, r * l * k, 20, 20);

			glRotated(180, 1, 0, 0);

			glTranslated(r * l * k * 0.8, 0, 0);               //палки
			gluCylinder(obj, r * l * k * 0.1, r * l * k * 0.1, h * l * 2.5, 20, 20);
			glTranslated(r * l * k * -1.6, 0, 0);
			gluCylinder(obj, r * l * k * 0.1, r * l * k * 0.1, h * l * 2.5, 20, 20);

			gluDeleteQuadric(obj);
		}
			break;
		case BTYPE::IMMUNITY:
		{
			l /= 4;
			GLUquadricObj* obj;
			obj = gluNewQuadric();
			int r = 20, h = 5, k = 5;

			glColor3f(192 / 255.0, 192 / 255.0, 192 / 255.0);

			glRotated(90, 1, 0, 0);
			Entity::Draw_Parallepiped(0, 0, 0, r * l * 1.9, r * l * 0.2, h * l / 1.99, 10, 10);             //полоса 1
			Entity::Draw_Parallepiped(0, 0, 0, r * l * 0.2, r * l * 1.9, h * l / 1.99, 10, 10);         //полоса 2
			glTranslated(0, 0, -1 * h * l / 2);
			gluCylinder(obj, r * l, r * l, h * l, 20, 20);      //обод щита

			glTranslated(0, 0, h * l / 2);
			gluSphere(obj, r * l / k, 20, 20);                  //сердцевина щита
			glTranslated(0, 0, -1 * h * l / 2);

			glColor3f(65 / 255.0, 43 / 255.0, 21 / 255.0);

			glTranslated(0, 0, h * l / 4);
			gluDisk(obj, 0, r * l, 20, 20);                      //основание щита    

			glColor3f(192 / 255.0, 192 / 255.0, 192 / 255.0);
			glTranslated(0, 0, -l / 100);
			gluDisk(obj, 0, r * l / k * 1.5, 20, 20);
			glTranslated(0, 0, l / 100);

			glColor3f(65 / 255.0, 43 / 255.0, 21 / 255.0);
			glTranslated(0, 0, 2 * l * h / 4);
			gluDisk(obj, 0, r * l, 20, 20);                      //основание щита  

			glColor3f(192 / 255.0, 192 / 255.0, 192 / 255.0);
			glTranslated(0, 0, l / 100);
			gluDisk(obj, 0, r * l / k * 1.5, 20, 20);
			gluDeleteQuadric(obj);
		}
			break;
		default:
			break;
		}
		if (abs(heroPar) >= 360)
			heroPar = 0;
		heroPar++;
	}
	glPopMatrix();
}

Bonus::BTYPE Bonus::getType() const noexcept
{
	return type;
}
