#pragma once

#include <QtWidgets/QWidget>

#include "glut.h"
#include <QtOpenGL>

#include "Player.h"
#include "Enemy.h"
#include "Diamond.h"
#include "Bonus.h"

#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

class Game3D : public QGLWidget
{
	Q_OBJECT

public:
	Game3D(QGLWidget *parent = nullptr);
	~Game3D();

	enum class STATE
	{
		GAME,
		PAUSE,
		MENU,
		WIN,
		LOSE
	};

public slots:
	void gameStep();
	void movePlayer();
	void moveEnemy();
	void addMove();
	void addFastMove();
	void checkCollision();
	void createBonus();
	void freezeBonus();
	void immunityBonus();

private:
	QTimer* gameTimer;
	static const unsigned gameTimerStep = 1000;

    QTimer* movePlayerTimer;
	static const unsigned movePlayerTimerStep = 50;

	QTimer* moveEnemyTimer;
	static const unsigned moveEnemyTimerStep = 100;

	QTimer* addMoveTimer;
	static const unsigned addMoveTimerStep = 1000;

	QTimer* addFastMoveTimer;
	static const unsigned addFastMoveTimerStep = 300;

	QTimer* checkCollisionTimer;
	static const unsigned checkCollisionTimerStep = 750;

	QTimer* createBonusTimer;
	static const unsigned createBonusTimerStep = 1500;

	QTimer* freezeBonusTimer;
	static const unsigned freezeBonusTimerStep = 6500;

	QTimer* immunityBonusTimer;
	static const unsigned immunityBonusTimerStep = 9000;

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;

	virtual void wheelEvent(QWheelEvent* wheel) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	QPoint mousePos;

	virtual void keyPressEvent(QKeyEvent* e) override;
private:
	STATE state;

	void drawGAME();
	void drawPAUSE();
	void drawMENU();
	void drawWIN();
	void drawLOSE();

	int gameTime;
	std::vector<std::vector<char>> lvl;
	std::vector<std::vector<bool>> lvlMovable;
	
	Player* p;
	std::vector<Enemy*> enemies;
	std::vector<Bonus*> bonuses;
	std::vector<std::vector<Diamond*>> diamonds;

	bool isValidCharacter(const char& c) const noexcept;
	bool isWall(const char& c) const noexcept;

	void loadLVL(QString path);
	void saveCurrentLVLState(QString path);
	QString path;

	void drawWall(std::tuple<float, float, float> point, double r, char c, double coefH = 1);

	void handleBonus(Bonus::BTYPE type) noexcept;

	static const unsigned short cellSize = 100;
};
