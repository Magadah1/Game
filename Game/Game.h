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
#include "Bonus.h"

class Game : public QGLWidget
{
    Q_OBJECT

public:
    Game(QGLWidget* parent = Q_NULLPTR);
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
    void moveEnemy();
    void movePlayer();
    void addMove();
    void addFastMove();
    void checkCollision();
    void createBonus();
    void freezeBonus();
    void immunityBonus();

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
    std::vector<Bonus*> bonuses;

    void handleBonus(Bonus::BTYPE type) noexcept;

    QTimer* gameTimer;
    QTimer* moveEnemyTimer;
    QTimer* movePlayerTimer;
    QTimer* addMoveTimer;
    QTimer* addFastMoveTimer;
    QTimer* checkCollisionTimer;
    QTimer* createBonusTimer;
    QTimer* freezeBonusTimer;
    QTimer* immunityBonusTimer;

    static const unsigned gameTimerStep = 1000;
    static const unsigned moveEnemyTimerStep = 100;
    static const unsigned movePlayerTimerStep = 50;
    static const unsigned addMoveTimerStep = 1000;
    static const unsigned addFastMoveTimerStep = 300;
    static const unsigned checkCollisionTimerStep = 750;
    static const unsigned createBonusTimerStep = 15000;
    static const unsigned freezeBonusTimerStep = 6500;
    static const unsigned immunityBonusTimerStep = 9000;

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
