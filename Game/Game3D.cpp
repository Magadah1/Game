#include "Game3D.h"

Game3D::Game3D(QGLWidget*parent)
	: QGLWidget(parent)
{
    srand(static_cast<unsigned>(time(nullptr)));
    move(200, 200);

    p = nullptr;

    resize(1000, 1000);

    gameTimer = new QTimer();
    gameTimer->start(gameTimerStep);
    connect(gameTimer, &QTimer::timeout, this, &Game3D::gameStep);

    movePlayerTimer = new QTimer();
    movePlayerTimer->start(movePlayerTimerStep);
    connect(movePlayerTimer, &QTimer::timeout, this, &Game3D::movePlayer);

    moveEnemyTimer = new QTimer();
    moveEnemyTimer->start(moveEnemyTimerStep);
    connect(moveEnemyTimer, &QTimer::timeout, this, &Game3D::moveEnemy);

    addMoveTimer = new QTimer();
    addMoveTimer->start(addMoveTimerStep);
    connect(addMoveTimer, &QTimer::timeout, this, &Game3D::addMove);

    addFastMoveTimer = new QTimer();
    addFastMoveTimer->start(addFastMoveTimerStep);
    connect(addFastMoveTimer, &QTimer::timeout, this, &Game3D::addFastMove);

    checkCollisionTimer = new QTimer();
    checkCollisionTimer->start(checkCollisionTimerStep);
    connect(checkCollisionTimer, &QTimer::timeout, this, &Game3D::checkCollision);

    createBonusTimer = new QTimer();
    createBonusTimer->start(createBonusTimerStep);
    connect(createBonusTimer, &QTimer::timeout, this, &Game3D::createBonus);

    freezeBonusTimer = new QTimer();
    connect(freezeBonusTimer, &QTimer::timeout, this, &Game3D::freezeBonus);

    immunityBonusTimer = new QTimer();
    connect(immunityBonusTimer, &QTimer::timeout, this, &Game3D::immunityBonus);

    setState(STATE::MENU);
}

Game3D::~Game3D()
{
    if (p) delete p;

    for (auto&& en : enemies) if (en) delete en;

    for (auto&& bonus : bonuses) if (bonus) delete bonus;

    for (auto&& diamondLine : diamonds) for (auto&& diamond : diamondLine) if (diamond) delete diamond;

    delete gameTimer;
    delete movePlayerTimer;
    delete moveEnemyTimer;
    delete addMoveTimer;
    delete addFastMoveTimer;
    delete checkCollisionTimer;
    delete createBonusTimer;
    delete freezeBonusTimer;
    delete immunityBonusTimer;
}

void Game3D::initializeGL()
{
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_LIGHT0);




    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

}

void Game3D::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, width(), height());
    glLoadIdentity();
    static const int magic = 2 * (sqrt(w * w + h * h) + 1);
    glOrtho(-w, w, -h, h, -magic, magic);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-60, 1, 0, 0);
}

void Game3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    switch (state)
    {
    case Game3D::STATE::GAME:
        drawGAME();
        break;
    case Game3D::STATE::PAUSE:
        drawPAUSE();
        break;
    case Game3D::STATE::MENU:
        drawMENU();
        break;
    case Game3D::STATE::WIN:
        drawWIN();
        break;
    case Game3D::STATE::LOSE:
        drawLOSE();
        break;
    default:
        break;
    }
}

void Game3D::wheelEvent(QWheelEvent* wheel)
{
    if (state != STATE::GAME)
        return;

    double s = 1;
    if (wheel->delta() > 0)
        s = 1.1;
    else if (wheel->delta() < 0)
        s = 0.9;
    glScalef(s, s, s);
    updateGL();
}

void Game3D::mouseMoveEvent(QMouseEvent* e)
{
    if (state != STATE::GAME)
        return;

    if (e->pos().x() >= 0 && e->pos().x() <= width() && e->pos().y() >= 0 && e->pos().y() <= height())
    {
        glRotatef((e->pos().y() - mousePos.y()) / 5.f, 1, -0.6, 0);
        glRotatef((e->pos().x() - mousePos.x()) / 5.f, 0, 0, 1);
        mousePos = e->pos();
        updateGL();
    }
}

void Game3D::mousePressEvent(QMouseEvent* e)
{
    if (state == STATE::GAME)
        if (e->button() == Qt::LeftButton)
            mousePos = e->pos();

    if (flags.none())
        return;

    const double bWidth = width() * 0.8;
    const double bHeigth = height() / 7.;
    const double betweenB = bHeigth / 3.;

    const double x0 = (width() - bWidth) / 2.;
    double y0 = betweenB;

    if (flags[0]) // продолжить
    {
        if (e->pos().x() >= x0 && e->pos().x() <= x0 + bWidth
            && e->pos().y() >= y0 && e->pos().y() <= y0 + bHeigth)
        {
            setState(STATE::GAME);
            return;
        }
    }

    y0 += bHeigth + betweenB;

    if (flags[1]) // сохранить
    {
        if (e->pos().x() >= x0 && e->pos().x() <= x0 + bWidth
            && e->pos().y() >= y0 && e->pos().y() <= y0 + bHeigth)
        {
            saveCurrentLVLState(path);
            setState(STATE::GAME);
            return;
        }
    }

    y0 += bHeigth + betweenB;

    if (flags[2]) // выбор уровня
    {
        if (e->pos().x() >= x0 && e->pos().x() <= x0 + bWidth
            && e->pos().y() >= y0 && e->pos().y() <= y0 + bHeigth)
        {
            QString path = QFileDialog::getOpenFileName();

            if (path.isEmpty())
                return;

            this->path = path;
            loadLVL(this->path);
            return;
        }
    }

    y0 += bHeigth + betweenB;

    if (flags[3]) // рестарт
    {
        if (e->pos().x() >= x0 && e->pos().x() <= x0 + bWidth
            && e->pos().y() >= y0 && e->pos().y() <= y0 + bHeigth)
        {
            loadLVL(path);
            return;
        }
    }

    y0 += bHeigth + betweenB;

    if (flags[4]) // выход
    {
        if (e->pos().x() >= x0 && e->pos().x() <= x0 + bWidth
            && e->pos().y() >= y0 && e->pos().y() <= y0 + bHeigth)
        {
            close();
            return;
        }
    }
}

void Game3D::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {

        if (state == STATE::GAME)
            setState(STATE::PAUSE);
        else if (state == STATE::PAUSE)
            setState(STATE::GAME);
    }

    if (state == STATE::GAME)
    {
        if (*p)
        {
            if (e->key() == Qt::Key_Up)
                p->addMove(Entity::MDIR::UP, lvlMovable);
            else if (e->key() == Qt::Key_Right)
                p->addMove(Entity::MDIR::RIGHT, lvlMovable);
            else if (e->key() == Qt::Key_Left)
                p->addMove(Entity::MDIR::LEFT, lvlMovable);
            else if (e->key() == Qt::Key_Down)
                p->addMove(Entity::MDIR::DOWN, lvlMovable);

            if (e->key() == Qt::Key_Q)
                glTranslatef(0, 0, -20);
            else if (e->key() == Qt::Key_E)
                glTranslatef(0, 0, 20);
            else if (e->key() == Qt::Key_W)
                glTranslatef(0, 20, 0);
            else if (e->key() == Qt::Key_S)
                glTranslatef(0, -20, 0);
            else if (e->key() == Qt::Key_A)
                glTranslatef(-20, 0, 0);
            else if (e->key() == Qt::Key_D)
                glTranslatef(20, 0, 0);

        }
        else
            setState(STATE::LOSE);

        if (e->key() == Qt::Key_Space)
        {
            p->fun(!p->getFun());

            for (auto&& en : enemies)
                en->fun(!en->getFun());
        }
    }
}

void Game3D::drawGAME()
{
    // НУЖНА НОРМАЛЬНАЯ ЛОГИКА ДЛЯ НЕЧЁТНЫХ РАЗМЕРОВ

    double kx = lvl[0].size() * cellSize / 2. / static_cast<double>(width());
    double ky = lvl.size() * cellSize / 2. / static_cast<double>(height());
    double x0 = -width() * kx + ((lvl[0].size() % 2)  ? cellSize / 2. : 0);
    double y0 = -height() * ky + ((lvl.size() % 2) ? cellSize / 2. : 0);

    // ПОЛ
    glColor3f(222 / 255., 169 / 255., 113 / 255.);
    glPushMatrix();
    {
        glBegin(GL_QUADS);
        {
            double rx = cellSize / 2. * lvl[0].size() - cellSize / 2.;
            double ry = cellSize / 2. * lvl.size() - cellSize / 2.;
            glVertex3f(-rx, -ry, -1);
            glVertex3f(-rx, +ry, -1);
            glVertex3f(+rx, +ry, -1);
            glVertex3f(+rx, -ry, -1);
        }
        glEnd();
    }
    glPopMatrix();


    int iMax = lvl[0].size() - static_cast<int>(lvl[0].size() / 2);
    int jMax = lvl.size() - static_cast<int>(lvl.size() / 2);
    for (int i = -static_cast<int>((lvl[0].size() / 2)); i < iMax; ++i)
        for (int j = -static_cast<int>((lvl.size() / 2)); j < jMax; ++j)
                drawWall({ i + 0.5,j + 0.5,0 }, cellSize / 2., lvl[j + lvl.size() / 2][i + lvl[0].size() / 2]);

    p->draw({ x0,y0 }, 4);

    for (auto&& en : enemies)
        en->draw({ x0,y0 }, 4);

    for (auto&& bonus : bonuses)
        bonus->draw({ x0, y0 }, 4);

    for (auto&& dl : diamonds)
        for (auto&& d : dl)
            if (d)
                d->draw({ x0,y0 }, 4);

    begin2D();
    {
        drawBars();
    }
    end2D();
}

void Game3D::drawPAUSE()
{
    begin2D();
    {
        drawButtons(flags);
    }
    end2D();
}

void Game3D::drawMENU()
{
    begin2D();
    {
        drawButtons(flags);
    }
    end2D();
}

void Game3D::drawWIN()
{
    begin2D();
    {
        drawButtons(flags);
    }
    end2D();
}

void Game3D::drawLOSE()
{
    begin2D();
    {
        drawButtons(flags);
    }
    end2D();
}

void Game3D::setState(STATE newState)
{
    state = newState;
    flags.reset();
    switch (state)
    {
    case Game3D::STATE::GAME:
    {
        gameTimer->start(gameTimerStep);
        moveEnemyTimer->start(moveEnemyTimerStep);
        movePlayerTimer->start(movePlayerTimerStep);
        addMoveTimer->start(addMoveTimerStep);
        addFastMoveTimer->start(addFastMoveTimerStep);
        checkCollisionTimer->start(checkCollisionTimerStep);
        createBonusTimer->start(createBonusTimerStep);
    }
        break;
    case Game3D::STATE::PAUSE:
    {
        flags |= 0b11111;
        gameTimer->stop();
        moveEnemyTimer->stop();
        movePlayerTimer->stop();
        addMoveTimer->stop();
        addFastMoveTimer->stop();
        checkCollisionTimer->stop();
        createBonusTimer->stop();
    }
        break;
    case Game3D::STATE::MENU:
    {
        flags |= 0b10100;
        gameTimer->stop();
        moveEnemyTimer->stop();
        movePlayerTimer->stop();
        addMoveTimer->stop();
        addFastMoveTimer->stop();
        checkCollisionTimer->stop();
        createBonusTimer->stop();
    }
        break;
    case Game3D::STATE::WIN:
    {
        flags |= 0b11100;
        gameTimer->stop();
        moveEnemyTimer->stop();
        movePlayerTimer->stop();
        addMoveTimer->stop();
        addFastMoveTimer->stop();
        checkCollisionTimer->stop();
        createBonusTimer->stop();
    }
        break;
    case Game3D::STATE::LOSE:
    {
        flags |= 0b11100;
        gameTimer->stop();
        moveEnemyTimer->stop();
        movePlayerTimer->stop();
        addMoveTimer->stop();
        addFastMoveTimer->stop();
        checkCollisionTimer->stop();
        createBonusTimer->stop();
    }
        break;
    }
    updateGL();
}

void Game3D::begin2D()
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width(), 0, height());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void Game3D::end2D()
{
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

void Game3D::drawBars()
{
    glPushMatrix();
    {
        { // полоска хп
            const double h = p->getHP() / static_cast<double>(p->getMaxHP());
            constexpr double hpLength = 200;
            constexpr double hpHeight = 65;

            glColor3f(1 - h, h, 0);
            glBegin(GL_QUADS);
            {
                glVertex2f(0, 0);
                glVertex2f(h * hpLength, 0);
                glVertex2f(h * hpLength, hpHeight);
                glVertex2f(0, hpHeight);
            }
            glEnd();


            glColor3f(1, 1, 1);
            glLineWidth(3);
            glBegin(GL_LINE_LOOP);
            {
                glVertex2f(0, 0);
                glVertex2f(hpLength, 0);
                glVertex2f(hpLength, hpHeight);
                glVertex2f(0, hpHeight);
            }
            glEnd();
            glLineWidth(1);
        }

        { // прогресс в сборе @монеток@
            const double m = (startMoney - currentMoney) / static_cast<double>(startMoney);
            constexpr double moneyLength = 200;
            constexpr double moneyHeight = 65;

            glColor3f(1 - m, m, 1 - m);
            glBegin(GL_QUADS);
            {
                glVertex2f(width() - moneyLength, 0);
                glVertex2f(width() + moneyLength * (m - 1), 0);
                glVertex2f(width() + moneyLength * (m - 1), moneyHeight);
                glVertex2f(width() - moneyLength, moneyHeight);
            }
            glEnd();

            glColor3f(1, 1, 1);
            glLineWidth(3);
            glBegin(GL_LINE_LOOP);
            {
                glVertex2f(width(), 0);
                glVertex2f(width(), moneyHeight);
                glVertex2f(width() - moneyLength, moneyHeight);
                glVertex2f(width() - moneyLength, 0);
            }
            glEnd();
            glLineWidth(1);
        }

        // если меньше 45 секунд осталось, то рисуем предупреждение!
        if (gameTime <= 45)
        {
            constexpr double triangleLength = 70;
            constexpr double triangleHeight = 100;
            glColor3f(229 / 255., 190 / 255., 1 / 255.);
            glBegin(GL_TRIANGLES);
            {
                glVertex2f(width() / 2., triangleHeight + 15);
                glVertex2f(width() / 2. - triangleLength, 15);
                glVertex2f(width() / 2. + triangleLength, 15);
            }
            glEnd();

            glColor3f(1, 1, 1);
            glLineWidth(3);
            glBegin(GL_LINE_LOOP);
            {
                glVertex2f(width() / 2., triangleHeight + 15);
                glVertex2f(width() / 2. - triangleLength, 15);
                glVertex2f(width() / 2. + triangleLength, 15);
            }
            glEnd();
            glLineWidth(1);
        }
    }
    glPopMatrix();
}

void Game3D::drawButtons(const std::bitset<5>& flags)
{
    const double bWidth = width() * 0.8;
    const double bHeigth = height() / 7.;
    const double betweenB = bHeigth / 3.;

    const double x0 = (width() - bWidth) / 2.;
    double y0 = betweenB;

    glBegin(GL_QUADS);
    {
        if (flags[4]) // выход
        {
            glColor3f(1, 0, 0);
            glVertex2f(x0, y0);
            glVertex2f(x0 + bWidth, y0);
            glVertex2f(x0 + bWidth, y0 + bHeigth);
            glVertex2f(x0, y0 + bHeigth);
        }

        y0 += bHeigth + betweenB;

        if (flags[3]) // рестарт
        {
            glColor3f(0, 1, 0);
            glVertex2f(x0, y0);
            glVertex2f(x0 + bWidth, y0);
            glVertex2f(x0 + bWidth, y0 + bHeigth);
            glVertex2f(x0, y0 + bHeigth);
        }

        y0 += bHeigth + betweenB;

        if (flags[2]) // выбор
        {
            glColor3f(0, 0, 1);
            glVertex2f(x0, y0);
            glVertex2f(x0 + bWidth, y0);
            glVertex2f(x0 + bWidth, y0 + bHeigth);
            glVertex2f(x0, y0 + bHeigth);
        }

        y0 += bHeigth + betweenB;

        if (flags[1]) // сохранить
        {
            glColor3f(1, 0, 1);
            glVertex2f(x0, y0);
            glVertex2f(x0 + bWidth, y0);
            glVertex2f(x0 + bWidth, y0 + bHeigth);
            glVertex2f(x0, y0 + bHeigth);
        }

        y0 += bHeigth + betweenB;

        if (flags[0]) // продолжить
        {
            glColor3f(1, 1, 0);
            glVertex2f(x0, y0);
            glVertex2f(x0 + bWidth, y0);
            glVertex2f(x0 + bWidth, y0 + bHeigth);
            glVertex2f(x0, y0 + bHeigth);
        }
    }
    glEnd();
}

bool Game3D::isValidCharacter(const char& c) const noexcept
{
    constexpr char valid_characters[] = { 'x', 'g', 'p', 'd', 'b', 'U', 'D', 'C', 'Q', 'e', 'o', 's', 'v' };

    for (auto&& el : valid_characters)
        if (c == el)
            return true;
    return false;
}

bool Game3D::isWall(const char& c) const noexcept
{
    constexpr char walls[] = { 'x', 'g', 'p', 'd', 'b', 'U', 'D', 'C', 'Q' };

    for (auto&& el : walls)
        if (c == el)
            return true;
    return false;
}

void Game3D::loadLVL(QString path)
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

        for (auto&& cl : lvl)
            cl.clear();
        lvl.clear();
        for (auto&& clm : lvlMovable)
            clm.clear();
        lvlMovable.clear();
        for (auto&& l : diamonds)
        {
            for (auto&& d : l)
                if (d)
                    delete d;
            l.clear();
        }
        diamonds.clear();

        std::vector<std::pair<size_t, size_t>> spawnPoints;
        std::vector<std::pair<size_t, size_t>> enSpawnPoints;
        lvl.resize(w);
        lvlMovable.resize(w);
        diamonds.resize(w);
        for (size_t i = 0; i < w; ++i)
        {
            lvl[i].resize(h);
            lvlMovable[i].resize(h);
            diamonds[i].resize(h);
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

        for (size_t i = 0; i < w; ++i)
            for (size_t j = 0;j < h; ++j)
                if (lvl[i][j] == 'o')
                    diamonds[i][j] = new Diamond(QPoint(j * cellSize + cellSize / 2, i * cellSize + cellSize / 2), cellSize);
                else
                    diamonds[i][j] = nullptr;

        startMoney = 0;
        for (auto&& line : lvl)
            startMoney += std::count(line.begin(), line.end(), 'o');

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
                enemies[i] = new Enemy({ static_cast<int>(ii * cellSize + cellSize / 2), static_cast<int>(jj * cellSize + cellSize / 2) }, cellSize, t);
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

        for (auto&& bonus : bonuses) if (bonus) delete bonus;
        bonuses.clear();

        setState(STATE::GAME);

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

void Game3D::saveCurrentLVLState(QString path)
{
    std::ofstream OUTfile((path.left(path.size() - 4) + "_current.txt").toStdString());

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

void Game3D::drawWall(std::tuple<float, float, float> point, double r, char c, double coefH)
{
    auto& [x, y, z] = point;
    double l = 1;
    x *= 2 * r * l;
    y *= 2 * r * l;
    z *= 2 * r * l;
    double h = r * coefH;
    glPushMatrix();
    {
        glTranslated(x, y, z);
        /*
        glBegin(GL_QUADS);
        {
            glVertex3f(-r, -r, -1);
            glVertex3f(-r, +r, -1);
            glVertex3f(+r, +r, -1);
            glVertex3f(+r, -r, -1);
        }
        glEnd();*/
        glColor3f(1, 1, 1);
        GLUquadricObj* quadObj = gluNewQuadric();
        switch (c)
        {
        case 'x': // стена
            Entity::Draw_Parallepiped(0, 0, h / 2 * l, 2 * r * l, 2 * r * l, h * l, 5, 5);
            break;
        case 'g': // левая верхняя закругленная
            glRotated(-90, 0, 0, 1);
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            Entity::Draw_Parallepiped(r / 2 * l, 0, h / 2 * l, r * l, r * 2 * l, h * l, 5, 5);
            break;
        case 'p': // правая верхняя закрунленная
            glRotated(180, 0, 0, 1);
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            Entity::Draw_Parallepiped(r / 2 * l, 0, h / 2 * l, r * l, r * 2 * l, h * l, 5, 5);
            break;
        case 'd': // левая нижняя закругленна
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            Entity::Draw_Parallepiped(r / 2 * l, 0, h / 2 * l, r * l, r * 2 * l, h * l, 5, 5);
            break;
        case 'b': // правая нижняя закругленная
            glRotated(90, 0, 0, 1);
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            Entity::Draw_Parallepiped(r / 2 * l, 0, h / 2 * l, r * l, r * 2 * l, h * l, 5, 5);
            break;
        case 'U':
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            break;
        case 'D':
            glRotated(90, 0, 0, 1);
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            break;
        case 'C':
            glRotated(-90, 0, 0, 1);
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            break;
        case 'Q':
            glRotated(180, 0, 0, 1);
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            Entity::Draw_Parallepiped(0, r / 2 * l, h / 2 * l, r * 2 * l, r * l, h * l, 5, 5);
            break;
        default:
            break;
        }
        gluDeleteQuadric(quadObj);
    }
    glPopMatrix();
}

void Game3D::handleBonus(Bonus::BTYPE type) noexcept
{
    if (state != STATE::GAME)
        return;

    switch (type)
    {
    case Bonus::BTYPE::TIME:
        gameTime += 10;
        break;
    case Bonus::BTYPE::HP:
        p->UpHP();
        break;
    case Bonus::BTYPE::FREEZE:
        addMoveTimer->stop();
        addFastMoveTimer->stop();
        moveEnemyTimer->stop();
        freezeBonusTimer->start(freezeBonusTimerStep);
        break;
    case Bonus::BTYPE::IMMUNITY:
        checkCollisionTimer->stop();
        immunityBonusTimer->start(immunityBonusTimerStep);
        break;
    default:
        break;
    }
}

void Game3D::gameStep()
{
    if (state != STATE::GAME)
        return;

    gameTime -= 1;
    if (gameTime <= 0)
        setState(STATE::LOSE);

    currentMoney = 0;
    for (auto&& line : lvl)
        currentMoney += std::count(line.begin(), line.end(), 'o');

    if (!currentMoney)
        setState(STATE::WIN);
}

void Game3D::movePlayer()
{
    if (state != STATE::GAME)
        return;

    if (!p)
        return;

    if (p->isReadyToMove())
        p->move();

    auto [x, y] = p->getIndexPos();
    if (lvl[y][x] == 'o')
    {
        lvl[y][x] = 'v';
        delete diamonds[y][x];
        diamonds[y][x] = nullptr;
    }

    for (auto&& bonus : bonuses)
        if (bonus->getIndexPos() == p->getIndexPos())
        {
            handleBonus(bonus->getType());
            delete bonus;
            bonus = nullptr;
        }

    std::erase(bonuses, nullptr);

    updateGL();
}

void Game3D::moveEnemy()
{
    if (state != STATE::GAME)
        return;

    for (auto&& en : enemies)
        if (en->isReadyToMove())
            en->move();

    updateGL();
}

void Game3D::addMove()
{
    if (state != STATE::GAME)
        return;

    for (auto&& en : enemies)
        if (en->getType() == Enemy::ETYPE::THREE)
            en->addMove(lvlMovable, p->getIndexPos());
}

void Game3D::addFastMove()
{
    if (state != STATE::GAME)
        return;

    for (auto&& en : enemies)
        if (en->getType() != Enemy::ETYPE::THREE)
            en->addMove(lvlMovable, p->getIndexPos());
}

void Game3D::checkCollision()
{
    if (state != STATE::GAME)
        return;

    for (auto&& en : enemies)
        if (en->getIndexPos() == p->getIndexPos())
        {
            p->DownHP();
            if (!*p)
                setState(STATE::LOSE);
        }
}

void Game3D::createBonus()
{
    if (state != STATE::GAME)
        return;

    if (bonuses.size() < 7)
        bonuses.push_back(new Bonus(lvlMovable, static_cast<Bonus::BTYPE>(rand() % 4), cellSize));
}

void Game3D::freezeBonus()
{
    addMoveTimer->start(addMoveTimerStep);
    addFastMoveTimer->start(addFastMoveTimerStep);
    moveEnemyTimer->start(moveEnemyTimerStep);
    freezeBonusTimer->stop();
}

void Game3D::immunityBonus()
{
    checkCollisionTimer->start(checkCollisionTimerStep);
    immunityBonusTimer->stop();
}
