#include "Game.h"

Game::Game(QGLWidget*parent)
    : QGLWidget(parent)
{
    srand(static_cast<unsigned>(time(nullptr)));
    ui.setupUi(this);

    buttonsMENU();
    state = STATE::MENU;
    p = nullptr;

    // TEST
    loadLVL("lvl_3.txt");

    gameTimer = new QTimer();
    gameTimer->start(1000);
    connect(gameTimer, &QTimer::timeout, this, &Game::gameStep);

    moveTimer = new QTimer();
    moveTimer->start(50);
    connect(moveTimer, &QTimer::timeout, this, &Game::moveStep);

    addMoveTimer = new QTimer();
    addMoveTimer->start(1500);
    connect(addMoveTimer, &QTimer::timeout, this, &Game::addMove);
}

Game::~Game()
{
    if (p)
        delete p;

    for (auto&& en : enemies)
        if (en)
            delete en;
}

void Game::on_select_clicked()
{
    path = QFileDialog::getOpenFileName();

    if (!path.isEmpty())
        loadLVL(path);
}

void Game::on_cont_clicked()
{
    buttonsGAME();
    updateGL();
}

void Game::on_restart_clicked()
{
    if (!path.isEmpty())
        loadLVL(path);
}

void Game::on_save_clicked()
{
    if (!path.isEmpty())
        saveCurrentLVLState(path);
}

void Game::on_exit_clicked()
{
    close();
}

void Game::initializeGL()
{
    move(0, 0);
}

void Game::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glOrtho(0, w, 0, h, -1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void Game::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 0);

    switch (state)
    {
    case Game::STATE::GAME:
        drawGAME();
        break;
    case Game::STATE::PAUSE:
        drawPAUSE();
        break;
    case Game::STATE::MENU:
        drawMENU();
        break;
    case Game::STATE::WIN:
        drawWIN();
        break;
    case Game::STATE::LOSE:
        drawLOSE();
        break;
    default:
        break;
    }
}

void Game::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {

        if (state == STATE::GAME)
        {
            buttonsPAUSE();
            state = STATE::PAUSE;
            gameTimer->stop();
            moveTimer->stop();
            addMoveTimer->stop();
            setWindowTitle("PAUSE");
        }
        else if (state == STATE::PAUSE)
        {
            buttonsGAME();
            state = STATE::GAME;
            gameTimer->start(1000);
            moveTimer->start(50);
            addMoveTimer->start(1500);
            setWindowTitle("GAME");
        }
    }
    if (state == STATE::GAME)
    {
        if (*p)
        {
            Entity::MDIR dir{};
            if (e->key() == Qt::Key_Up)
                dir = Entity::MDIR::UP;
            else if (e->key() == Qt::Key_Right)
                dir = Entity::MDIR::RIGHT;
            else if (e->key() == Qt::Key_Left)
                dir = Entity::MDIR::LEFT;
            else if (e->key() == Qt::Key_Down)
                dir = Entity::MDIR::DOWN;

            p->addMove(dir, lvlMovable);
            
        }
        else
            buttonsLOSE();
    }

    updateGL();
}

void Game::drawPAUSE()
{
   /* glColor3f(0.1, 0.33, 0.95);

    glBegin(GL_TRIANGLES);
    {
        glVertex2f(0, 100);
        glVertex2f(100, 100);
        glVertex2f(100, 0);
    }
    glEnd();*/
}

void Game::drawGAME()
{
   for (size_t i = 0; i < lvl.size(); ++i)
       for (size_t j = 0; j < lvl[i].size(); ++j)
       {
           if (lvl[i][j] == 'x')
               glColor3f(1, 0, 0);
           else if (lvl[i][j] == 'e')
               glColor3f(1, 1, 0);
           else if (lvl[i][j] == 's')
               glColor3f(0, 1, 0);
           else if (lvl[i][j] == 'v')
               glColor3f(1, 0, 1);
           else
               glColor3f(0, 1, 1 );

           glBegin(GL_QUADS);
           {
               glVertex2f(j * cellSize, i * cellSize);
               glVertex2f((j + 1) * cellSize, i * cellSize);
               glVertex2f((j + 1) * cellSize, (i + 1) * cellSize);
               glVertex2f(j * cellSize, (i + 1) * cellSize);
           }
           glEnd();
       }

   p->draw();

   for (auto&& en : enemies)
       en->draw();
}

void Game::drawMENU()
{
   /* glColor3f(1, 0, 1);

    glBegin(GL_QUADS);
    {
        glVertex2f(0, 0);
        glVertex2f(0, 200);
        glVertex2f(100, 200);
        glVertex2f(100, 0);
    }
    glEnd();*/
}

void Game::drawLOSE()
{
    // test
    setWindowTitle("YOU LOSE!");

    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
    {
        glVertex2d(0, 0);
        glVertex2d(300, 300);
    }
    glEnd();
}

void Game::drawWIN()
{
   /* glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    {
        glVertex2d(0, 0);
        glVertex2d(300, 300);
    }
    glEnd();
    setWindowTitle(QString("YOU WIN! TIME = {%0}").arg(gameTime));*/
}

void Game::loadLVL(QString path)
{
    try
    {
        std::ifstream INfile(path.toStdString());
        if (!INfile)
            throw std::exception("Error in loading lvl!");
        std::string tempW;

        bool isNew = true;
        INfile >> tempW;
        if (tempW != "NEW")
            throw std::exception("Did not find field \'NEW\'!");
        INfile >> isNew;

        INfile >> tempW;
        if (tempW != "TIME")
            throw std::exception("Did not find field \'TIME\'!");
        INfile >> gameTime;

        INfile >> tempW;
        if (tempW != "SIZE")
            throw std::exception("Did not find field \'SIZE\'!");
        size_t w, h;
        INfile >> w >> h;
        if (w > 20 || h > 20)
            throw std::exception("Size is too large! (Must be not greater than (20,20))");

        INfile >> tempW;
        if (tempW != "FIELD")
            throw std::exception("Did not find field \'FIELD\'!");

        coinCount = 0;
        lvl.clear();
        lvlMovable.clear();
        spawnPoints.clear();
        enSpawnPoints.clear();
        lvl.resize(w);
        lvlMovable.resize(w);
        for (size_t i = 0; i < w; ++i)
        {
            lvl[i].resize(h);
            lvlMovable[i].resize(h);
            for (size_t j = 0; j < h; ++j)
            {
                char c;
                INfile >> c;
                if (isValidCharacter(c))
                {
                    lvl[i][j] = c;
                    if (c == 's')
                        spawnPoints.push_back({ w - i - 1, j });
                    if (c == 'e')
                        enSpawnPoints.push_back({ w - i - 1, j });
                    if (c == 'o')
                        ++coinCount;

                    if (isWall(c))
                        lvlMovable[i][j] = false;
                    else
                        lvlMovable[i][j] = true;
                }
                else
                    throw std::exception("Found not valid character!");
            }
        }
        std::reverse(lvl.begin(), lvl.end());
        std::reverse(lvlMovable.begin(), lvlMovable.end());

        INfile >> tempW;
        if (tempW != "PLAYER")
            throw std::exception("Did not find field \'PLAYER\'!");

        int hp{}, maxhp{};
        if (!isNew)
        {
            INfile >> tempW;
            if (tempW != "HP")
                throw std::exception("Did not find field \'HP\'! Is it realy current state?");
            INfile >> hp;
        }

        INfile >> tempW;
        if (tempW != "MAXHP")
            throw std::exception("Did not find field \'MAXHP\'!");
        INfile >> maxhp;

        size_t i{}, j{};
        if (!isNew)
        {
            INfile >> tempW;
            if (tempW != "POS")
                throw std::exception("Did not find field \'POS\'! Is it realy current state?");

            INfile >> i >> j;
        }

        INfile >> tempW;
        if (tempW != "ENEMIES")
            throw std::exception("Did not find field \'ENEMIES\'!");
        size_t en{};
        INfile >> en;

        if (en > enSpawnPoints.size())
            throw std::exception("TOO MUSH ENEMIES!");
        if (!spawnPoints.size())
            throw std::exception("THERE IS NO SPAWN POINT FOR PLAYER!");

        for (auto&& e : enemies)
            if (e) 
                delete e;
        enemies.clear();
        enemies.resize(en);

        
        for (size_t i = 0; i < enemies.size(); ++i)
        {
            INfile >> tempW;
            if (tempW != "TYPE")
                throw std::exception("Did not find field \'TYPE\'!");
            int type;
            INfile >> type;

            Enemy::ETYPE t = static_cast<Enemy::ETYPE>(type);
            if (!isNew)
            {
                INfile >> tempW;
                if (tempW != "POS")
                    throw std::exception("Did not find field \'POS\'! Is it realy current state?");

                size_t ii, jj;

                INfile >> ii >> jj;
                enemies[i] = new Enemy({ static_cast<int>(ii * cellSize + cellSize / 2), static_cast<int>(jj * cellSize + cellSize / 2)}, cellSize, t);
            }
            else
            {
                enemies[i] = new Enemy({ static_cast<int>(enSpawnPoints[i].second * cellSize + cellSize / 2), static_cast<int>(enSpawnPoints[i].first * cellSize + cellSize / 2) }, cellSize, t);
            }
        }

        auto spawnPoint = isNew ? spawnPoints[rand() % spawnPoints.size()] : std::pair<size_t, size_t>(i, j);

        if (p)
            delete p;

        p = new Player({ static_cast<int>(spawnPoint.second * cellSize + cellSize / 2), static_cast<int>(spawnPoint.first * cellSize + cellSize / 2) },
            cellSize,
            (isNew ? maxhp : hp), maxhp);

        buttonsGAME();
        resize(h* cellSize, w* cellSize);
        resizeGL(h* cellSize, w* cellSize);

        update();

        INfile.close();
    }
    catch (const std::exception& e)
    {
        setWindowTitle(e.what());
    }
    catch (...)
    {
        setWindowTitle("SOME DIFFERENT ERROR!");
    }
}

void Game::saveCurrentLVLState(QString path)
{
    std::ofstream OUTfile((path.left(path.size() - 5) + "_current.txt").toStdString());

    if (!OUTfile)
    {
        setWindowTitle("Error during saving lvl!");
        return; 
    }

    OUTfile << "NEW 0\n";
    OUTfile << "TIME " << gameTime << '\n';
    OUTfile << "SIZE " << lvl.size() << ' ' << lvl[0].size();
    OUTfile << "\nFIELD\n";
    for (auto r = lvl.rbegin(); r != lvl.rend(); ++r)
    {
        for (auto&& c : *r)
            OUTfile << c;
        OUTfile << '\n';
    }
    OUTfile << "PLAYER\n";
    OUTfile << "HP " << p->getHP() << '\n';
    OUTfile << "MAXHP " << p->getMaxHP() << '\n';
    OUTfile << "POS " << p->getPos().y() / cellSize << ' ' << p->getPos().x() / cellSize << '\n';
    OUTfile << "ENEMIES " << enemies.size() << '\n';
    for (auto&& en : enemies)
    {
        OUTfile << "TYPE " << static_cast<int>(en->getType()) << '\n';
        OUTfile << "POS " << en->getPos().x() / cellSize << ' ' << en->getPos().y() / cellSize << '\n';
    }
}

void Game::buttonsPAUSE()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    ui.cont->setVisible(true);
    ui.select->setVisible(true);
    ui.restart->setVisible(true);
    ui.save->setVisible(true);
    ui.exit->setVisible(true);
    state = STATE::PAUSE;
}

void Game::buttonsGAME()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    ui.cont->setVisible(false);
    ui.select->setVisible(false);
    ui.restart->setVisible(false);
    ui.save->setVisible(false);
    ui.exit->setVisible(false);
    state = STATE::GAME;
}

void Game::buttonsMENU()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    ui.cont->setVisible(false);
    ui.select->setVisible(true);
    ui.restart->setVisible(false);
    ui.save->setVisible(false);
    ui.exit->setVisible(true);
    state = STATE::MENU;
}

void Game::buttonsWIN()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    ui.cont->setVisible(false);
    ui.select->setVisible(true);
    ui.restart->setVisible(true);
    ui.save->setVisible(false);
    ui.exit->setVisible(true);
    state = STATE::WIN;
}

void Game::buttonsLOSE()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    ui.cont->setVisible(false);
    ui.select->setVisible(false);
    ui.restart->setVisible(false);
    ui.save->setVisible(false);
    ui.exit->setVisible(true);
    state = STATE::LOSE;
}

/// <summary>
/// x - стена
/// e - возможная позиция спавна врага
/// o - поле с монеткой
/// v - просто пустое поле
/// s - возможная позиция спавна игрока
/// </summary>
/// <param name="c"></param>
/// <returns></returns>
bool Game::isValidCharacter(const char& c) const noexcept
{
    constexpr char valid_characters[] = { 'x', 'e', 'o', 's', 'v'};

    for (auto&& el : valid_characters)
        if (c == el) 
            return true;
    return false;
}


bool Game::isWall(const char& c) const noexcept
{
    constexpr char walls[] = { 'x' };

    for (auto&& el : walls)
        if (c == el)
            return true;
    return false;
}


void Game::gameStep()
{
    if (state != STATE::GAME)
        return;

    for (auto&& en : enemies)
        if (en->getType() == Enemy::ETYPE::ONE)
            en->addMove(lvlMovable, p->getIndexPos());

    gameTime -= 1;
    if (gameTime <= 0)
    {
        state = STATE::LOSE;
        buttonsLOSE();
        updateGL();
    }

    int sum{};
    for (auto&& line : lvl)
        sum += std::count(line.begin(), line.end(), 'o');

    if (!sum)
    {
        buttonsWIN();

        // test
        setWindowTitle("YOU WON!");
    }
}

void Game::moveStep()
{
    if (state != STATE::GAME)
        return;

    if (p)
    {
        if (p->isReadyToMove())
            p->move();
        auto [x, y] = p->getIndexPos();
        if (lvl[y][x] == 'o')
            lvl[y][x] = 'v';
    }

    for (auto&& en : enemies)
        if (en->isReadyToMove())
            en->move();

    updateGL();
}

void Game::addMove()
{
    if (state != STATE::GAME)
        return;

    for (auto&& en : enemies)
    {
        /*if (en->getType() == Enemy::ETYPE::THREE)
            en->resetMoves();*/
        if (en->getType() != Enemy::ETYPE::ONE)
            en->addMove(lvlMovable, p->getIndexPos());
        
        if (en->getIndexPos() == p->getIndexPos())
        {
            p->DownHP();
            if (!*p)
            {
                buttonsLOSE();
            }
        }
    }
}
