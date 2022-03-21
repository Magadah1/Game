#include "Entity.h"

Entity::Entity(QPoint pos, int cellSize) noexcept
	: pos(pos), cellSize(cellSize), nextPos(pos)
{
}

void Entity::resetMoves() noexcept
{
	if (dirQ.empty())
		return;

	std::queue<std::pair<int, MDIR>> m;
	m.push(dirQ.front());
	m.swap(dirQ);
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