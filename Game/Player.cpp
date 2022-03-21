#include "Player.h"

Player::Player(QPoint pos, int cellSize, int health, int maxHealth) noexcept
	: Entity(pos, cellSize), health(health), maxHealth(maxHealth)
{
}

int Player::getHP() const noexcept
{
	return health;
}

int Player::getMaxHP() const noexcept
{
	return maxHealth;
}

void Player::UpHP(int hp) noexcept
{
	health += hp;
	health %= (maxHealth + 1);
}

void Player::DownHP(int hp) noexcept
{
	health -= hp;
	if (health < 0)
		health = 0;
}

Player::operator bool() const noexcept
{
	return health;
}

void Player::draw() const noexcept
{
	// test

	glColor3f(0.33, 0.33, 0.33);
	glBegin(GL_QUADS);
	{
		glVertex2f(pos.x() - cellSize / 2, pos.y() - cellSize / 2);
		glVertex2f(pos.x() - cellSize / 2, pos.y() + cellSize / 2);
		glVertex2f(pos.x() + cellSize / 2, pos.y() + cellSize / 2);
		glVertex2f(pos.x() + cellSize / 2, pos.y() - cellSize / 2);
	}
	glEnd();
}

bool Player::addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept
{
	int x = nextPos.x(), y = nextPos.y();

	switch (dir)
	{
	case MDIR::UP:
		y += cellSize;
		break;
	case MDIR::RIGHT:
		x += cellSize;
		break;
	case MDIR::DOWN:
		y -= cellSize;
		break;
	case MDIR::LEFT:
		x -= cellSize;
		break;
	}

	int ix = x / cellSize, iy = y / cellSize;

	if (iy >= lvl.size() || ix >= lvl[0].size() || ix < 0 || iy < 0)
		return false;

	if (lvl[iy][ix] && dirQ.size() < 1)
	{
		dirQ.push({ 0, dir });
		nextPos = QPoint(x, y);
		return true;	
	}

	return false;
}


void Player::move() noexcept
{
	if (!isReadyToMove())
		return; // check if idiot


	switch (dirQ.front().second)
	{
	case MDIR::UP:
		pos.ry() += cellSize / 10;
		break;
	case MDIR::RIGHT:
		pos.rx() += cellSize / 10;
		break;
	case MDIR::DOWN:
		pos.ry() -= cellSize / 10;
		break;
	case MDIR::LEFT:
		pos.rx() -= cellSize / 10;
		break;
	}
	++dirQ.front().first;

	if (dirQ.front().first >= 10)
		dirQ.pop();
}
