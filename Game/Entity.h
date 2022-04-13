#pragma once

#include <qpoint.h>
#include <QtOpenGL>
#include "glut.h"
#include "glaux.h"
#include <vector>
#include <queue>
#pragma comment (lib, "legacy_stdio_definitions.lib")
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

	static void Draw_Parallepiped(float center_x, float center_y, float center_z, float X_side, float Y_side, float Z_side, float split_1, float split_2);

	void resetMoves() noexcept;

	void fun(bool v = true) noexcept;
	bool getFun() const noexcept;

	bool isReadyToMove() const noexcept;

	QPoint getPos() const noexcept;

	std::pair<int, int> getIndexPos() const noexcept;
public: // virtual

	virtual void draw(std::pair<double, double> t0, double l) const noexcept = 0;

	virtual bool addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept = 0;

	virtual void move() noexcept = 0;

	virtual ~Entity() = default;
protected:
	QPoint pos;

	int cellSize;

	bool isFun;

	mutable int heroPar;

	std::queue<std::pair<int, MDIR>> dirQ;

	mutable MDIR lastDir;

	QPoint nextPos;

	GLuint textures;

	AUX_RGBImageRec* photo_image;
};