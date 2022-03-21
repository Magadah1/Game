#pragma once

#include <QMainWindow>
#include "ui_MainGame.h"

#include "Game.h"

class MainGame : public QMainWindow
{
	Q_OBJECT

public:
	MainGame(QWidget *parent = Q_NULLPTR);
	~MainGame();


private:
	Ui::MainGame ui;

	Game* g;
};
