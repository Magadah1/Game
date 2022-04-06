#include "Entity.h"

Entity::Entity(QPoint pos, int cellSize) noexcept
    : pos(pos), cellSize(cellSize), nextPos(pos), isFun(false), heroPar(0), lastDir(MDIR::UP), textures(0), photo_image(nullptr)
{
}

void Entity::Draw_Parallepiped(float center_x, float center_y, float center_z, float X_side, float Y_side, float Z_side, float split_1, float split_2)
{
    float a = X_side / split_1, b = Y_side / split_2, c = Z_side / split_2;
    glPushMatrix();
    glTranslated(center_x, center_y, center_z);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    for (int i = 0; i < split_2; i++)
        for (int j = 0; j < split_2; j++)
        {
            glVertex3f(-X_side / 2 + j * a, Y_side / 2, -Z_side / 2 + i * c);
            glVertex3f(-X_side / 2 + (j + 1) * a, Y_side / 2, -Z_side / 2 + i * c);
            glVertex3f(-X_side / 2 + (j + 1) * a, Y_side / 2, -Z_side / 2 + (i + 1) * c);
            glVertex3f(-X_side / 2 + j * a, Y_side / 2, -Z_side / 2 + (i + 1) * c);
        }
    glNormal3f(0, -1, 0);
    for (int i = 0; i < split_2; i++)
        for (int j = 0; j < split_2; j++)
        {

            glVertex3f(-X_side / 2 + j * a, -Y_side / 2, -Z_side / 2 + i * c);
            glVertex3f(-X_side / 2 + (j + 1) * a, -Y_side / 2, -Z_side / 2 + i * c);
            glVertex3f(-X_side / 2 + (j + 1) * a, -Y_side / 2, -Z_side / 2 + (i + 1) * c);
            glVertex3f(-X_side / 2 + j * a, -Y_side / 2, -Z_side / 2 + (i + 1) * c);
        }

    //боковушки
    glNormal3f(0, 0, 1);
    for (int i = 0; i < split_1; i++)
        for (int j = 0; j < split_2; j++)
        {
            glVertex3f(-X_side / 2 + j * a, -Y_side / 2 + i * b, -Z_side / 2);
            glVertex3f(-X_side / 2 + (j + 1) * a, -Y_side / 2 + i * b, -Z_side / 2);
            glVertex3f(-X_side / 2 + (j + 1) * a, -Y_side / 2 + (i + 1) * b, -Z_side / 2);
            glVertex3f(-X_side / 2 + j * a, -Y_side / 2 + (i + 1) * b, -Z_side / 2);
        }
    glNormal3f(0, 0, 1);
    for (int i = 0; i < split_1; i++)
        for (int j = 0; j < split_2; j++)
        {
            glVertex3f(-X_side / 2 + j * a, -Y_side / 2 + i * b, Z_side / 2);
            glVertex3f(-X_side / 2 + (j + 1) * a, -Y_side / 2 + i * b, Z_side / 2);
            glVertex3f(-X_side / 2 + (j + 1) * a, -Y_side / 2 + (i + 1) * b, Z_side / 2);
            glVertex3f(-X_side / 2 + j * a, -Y_side / 2 + (i + 1) * b, Z_side / 2);
        }
    glNormal3f(1, 0, 0);
    for (int i = 0; i < split_1; i++)
        for (int j = 0; j < split_2; j++)
        {
            glVertex3f(-X_side / 2, -Y_side / 2 + i * b, -Z_side / 2 + j * c);
            glVertex3f(-X_side / 2, -Y_side / 2 + (i + 1) * b, -Z_side / 2 + j * c);
            glVertex3f(-X_side / 2, -Y_side / 2 + (i + 1) * b, -Z_side / 2 + (j + 1) * c);
            glVertex3f(-X_side / 2, -Y_side / 2 + b * i, -Z_side / 2 + (j + 1) * c);
        }
    glNormal3f(1, 0, 0);
    for (int i = 0; i < split_1; i++)
        for (int j = 0; j < split_2; j++)
        {
            glVertex3f(X_side / 2, -Y_side / 2 + i * b, -Z_side / 2 + j * c);
            glVertex3f(X_side / 2, -Y_side / 2 + (i + 1) * b, -Z_side / 2 + j * c);
            glVertex3f(X_side / 2, -Y_side / 2 + (i + 1) * b, -Z_side / 2 + (j + 1) * c);
            glVertex3f(X_side / 2, -Y_side / 2 + b * i, -Z_side / 2 + (j + 1) * c);
        }
    glEnd();
    glPopMatrix();
}

void Entity::resetMoves() noexcept
{
	if (dirQ.empty())
		return;

	std::queue<std::pair<int, MDIR>> m;
	m.push(dirQ.front());
	m.swap(dirQ);
}

void Entity::fun(bool v) noexcept
{
    isFun = v;
}

bool Entity::getFun() const noexcept
{
    return isFun;
}

bool Entity::isReadyToMove() const noexcept
{
	return !dirQ.empty();
}

QPoint Entity::getPos() const noexcept
{
	return pos;
}

std::pair<int, int> Entity::getIndexPos() const noexcept
{
	return std::pair<int, int>(pos.x() / cellSize, pos.y() / cellSize);
}