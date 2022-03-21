#pragma once

#include <QtWidgets/QWidget>
#include "ui_Game.h"

#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

#include <qfiledialog.h>

#include "glut.h"
#include <QtOpenGL>

#include "Player.h"
#include "Enemy.h"

class Game : public QGLWidget
{
    Q_OBJECT

public:
    Game(QGLWidget*parent = Q_NULLPTR);
    ~Game();

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
    void moveStep();
    void addMove();

    void on_select_clicked();
    void on_cont_clicked();
    void on_restart_clicked();
    void on_save_clicked();
    void on_exit_clicked();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int, int) override;
    virtual void paintGL() override;
    
    virtual void keyPressEvent(QKeyEvent* e) override;

private:
    Ui::GameClass ui;

    std::vector<std::vector<char>> lvl;
    std::vector<std::vector<bool>> lvlMovable;
    std::vector<std::pair<size_t, size_t>> spawnPoints;
    std::vector<std::pair<size_t, size_t>> enSpawnPoints;
    int gameTime;
    int coinCount;

    STATE state;

    Player* p;
    std::vector<Enemy*> enemies;

    QTimer* gameTimer;
    QTimer* moveTimer;
    QTimer* addMoveTimer;


    void drawPAUSE();
    void drawGAME();
    void drawMENU();
    void drawWIN();
    void drawLOSE();

    void buttonsPAUSE();
    void buttonsGAME();
    void buttonsMENU();
    void buttonsWIN();
    void buttonsLOSE();

    void loadLVL(QString path);
    void saveCurrentLVLState(QString path);

    bool isValidCharacter(const char& c) const noexcept;
    bool isWall(const char& c) const noexcept;

    static const unsigned short cellSize = 100;

    QString path;
};
