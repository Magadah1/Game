#include "MainGame.h"

MainGame::MainGame(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	g = new Game;
	setCentralWidget(g);
	g->grabKeyboard();
}

MainGame::~MainGame()
{
	delete g;
}
