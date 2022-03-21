#pragma once

#include <qpoint.h>
#include <QtOpenGL>
#include "glut.h"
#include <vector>
#include <queue>

class Entity
{
	//Q_OBJECT;
public:
	Entity(QPoint pos, int cellSize) noexcept;

	typedef enum class MOVEMENT_DIRECTION
	{
		UP,
		RIGHT,
		LEFT,
		DOWN
	} MDIR;



	void resetMoves() noexcept;

	bool isReadyToMove() const noexcept;

	QPoint getPos() const noexcept;

	std::pair<int, int> getIndexPos() const noexcept;
public: // virtual

	virtual void draw() const noexcept = 0;

	virtual bool addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept = 0;

	virtual void move() noexcept = 0;

	virtual ~Entity() = default;
protected:
	QPoint pos;

	int cellSize;

	std::queue<std::pair<int, MDIR>> dirQ;

	QPoint nextPos;
};