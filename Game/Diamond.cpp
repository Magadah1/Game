#include "Diamond.h"

Diamond::Diamond(QPoint pos, int cellSize) noexcept
	: Entity(pos, cellSize)
{
    heroPar = rand() % 361;
}

void Diamond::draw(std::pair<double, double> t0, double l) const noexcept
{
    auto [x0, y0] = t0;
    l /= 2;
    glPushMatrix();
    {
        glTranslatef(x0 + pos.x(), y0 + pos.y(), 10 * l);

        glRotated(heroPar * 3, 0, 0, 1);
        glColor3f(185 / 255.0, 242 / 255.0, 255 / 255.0);
        GLUquadricObj* quadObj;
        quadObj = gluNewQuadric();
        gluSphere(quadObj, 4.5 * l, 4, 4);
    }
    glPopMatrix();

    if (abs(heroPar) >= 360)
        heroPar = 0;
    heroPar++;
}

bool Diamond::addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept
{
	// еасвхи йняршкэ
	return false;
}

void Diamond::move() noexcept
{
	// еасвхи йняршкэ
}
