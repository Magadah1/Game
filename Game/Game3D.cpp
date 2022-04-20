#include "Game3D.h"

Game3D::Game3D(QGLWidget*parent)
	: QGLWidget(parent)
{
    srand(static_cast<unsigned>(time(nullptr)));

    setWindowIcon(QIcon("IcoG.ico"));

    p = nullptr;

    setMinimumSize(1000, 1000);
    showMaximized();

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


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    pT = auxDIBImageLoad(L"Hero.bmp");
    glGenTextures(1, &texP);

    eT = auxDIBImageLoad(L"Z.bmp");
    glGenTextures(1, &texE);

    wT = auxDIBImageLoad(L"WinFun.bmp");
    glGenTextures(1, &texW);

    lT = auxDIBImageLoad(L"LoseFun.bmp");
    glGenTextures(1, &texL);

    isNew = false;

    newGame = NewGame::SELECTION;

    fun = false;

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

    delete pT->data;
    delete pT;
    delete eT->data;
    delete eT;
    delete wT->data;
    delete wT;
    delete lT->data;
    delete lT;
}

void Game3D::initializeGL()
{
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_LIGHT0);
    float GL_LIGHT0_CORD[] = { 0,0,700, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, GL_LIGHT0_CORD);
    //float GL_LIGHT0_DIR[] = {}




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

    const double bWidth = width() / 8.;

    const double r = (bWidth > height() ? height() / 2. : bWidth / 2) * 2;


    const double centerB = width() / 6.;

    std::pair<float, float> center{ centerB, height() / 2. };
    auto& [x, y] = center;

    if (flags[0]) // продолжить
        if (e->pos().x() >= x - r / 2. && e->pos().x() <= x + r / 2. &&
            e->pos().y() >= y - r / 2. && e->pos().y() <= y + r / 2.)
        {
            if (newGame == NewGame::SELECTION)
            {
                newGame = NewGame::LVL1;
                path = "Levels\\Game_lvl_1.txt";
                loadLVL(path);
                return;
            }
            setState(STATE::GAME);
            return;
        }

    x += centerB;
    if (flags[1]) // сохранить
        if (e->pos().x() >= x - r / 2. && e->pos().x() <= x + r / 2. &&
            e->pos().y() >= y - r / 2. && e->pos().y() <= y + r / 2.)
        {
            saveCurrentLVLState(path);
            setState(STATE::GAME);
            return;
        }

    x += centerB;
    if (flags[2]) // загрузить
        if (e->pos().x() >= x - r / 2. && e->pos().x() <= x + r / 2. &&
            e->pos().y() >= y - r / 2. && e->pos().y() <= y + r / 2.)
        {
            QString path = QFileDialog::getOpenFileName();

            if (path.isEmpty())
                return;

            newGame = NewGame::NONE;

            this->path = path;
            loadLVL(this->path);
            return;
        }

    x += centerB;
    if (flags[3]) // рестарт
        if (e->pos().x() >= x - r / 2. && e->pos().x() <= x + r / 2. &&
            e->pos().y() >= y - r / 2. && e->pos().y() <= y + r / 2.)
        {
            loadLVL(path);
            return;
        }

    x += centerB;
    if (flags[4]) // выход
        if (e->pos().x() >= x - r / 2. && e->pos().x() <= x + r / 2. &&
            e->pos().y() >= y - r / 2. && e->pos().y() <= y + r / 2.)
        {
            close();
            return;
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

        static bool light = true;
        if (e->key() == Qt::Key_L)
            if (light)
            {
                glDisable(GL_LIGHT0);
                light = false;
            }
            else
            {
                glEnable(GL_LIGHT0);
                light = true;
            }
    }

    if (e->key() == Qt::Key_F)
    {
        fun = !fun;
        updateGL();
    }

    static bool full = false;
    if (e->key() == Qt::Key_F11)
        if (full)
        {
            showMaximized();
            full = false;
        }
        else
        {
            showFullScreen();
            full = true;
        }
}

void Game3D::drawGAME()
{
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

    /*int iMax = lvl[0].size() - static_cast<int>(lvl[0].size() / 2);
    int jMax = lvl.size() - static_cast<int>(lvl.size() / 2);
    int fff = -static_cast<int>((lvl[0].size() / 2));
    int ddd = -static_cast<int>((lvl.size() / 2));
    int i, j;
    for (i = -static_cast<int>((lvl[0].size() / 2)); i < (static_cast<int>(lvl[0].size()) - static_cast<int>(lvl[0].size() / 2)); ++i)
        for (j = -static_cast<int>((lvl.size() / 2)); j < static_cast<int>(lvl.size()) - static_cast<int>(lvl.size() / 2); ++j)
            drawWall({ i + 0.5,j + 0.5,0 }, cellSize / 2., lvl[j + lvl.size() / 2][i + lvl[0].size() / 2]);*/

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
    if (newGame == NewGame::LVL1)
    {
        newGame = NewGame::LVL2;
        path = "Levels\\Game_lvl_2.txt";
        loadLVL(path);
    }
    else if (newGame == NewGame::LVL2)
    {
        newGame = NewGame::LVL3;
        path = "Levels\\Game_lvl_3.txt";
        loadLVL(path);
    }
    else if (newGame == NewGame::LVL3)
        newGame = NewGame::NONE;


    begin2D();
    {
        drawButtons(flags);

        if (fun)
        {
            const double bWidth = width() / 8.;
            const double r = (bWidth > height() ? height() / 2. : bWidth / 2);

            const double x0 = width() / 6.;
            const double y0 = height() / 2. + 1.2 * r;
            const double hx = 4 * x0;

            glBindTexture(GL_TEXTURE_2D, texW);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, 3,
                wT->sizeX,
                wT->sizeY,
                0, GL_RGB, GL_UNSIGNED_BYTE,
                wT->data);
            glEnable(GL_TEXTURE_2D);
            {
                glBindTexture(GL_TEXTURE_2D, texW);
                glBegin(GL_QUADS);
                glColor3d(1, 1, 1);
                glTexCoord2d(0, 0); glVertex2f(x0, y0);
                glTexCoord2d(1, 0); glVertex2f(x0 + hx, y0);
                glTexCoord2d(1, 1); glVertex2f(x0 + hx, height());
                glTexCoord2d(0, 1); glVertex2f(x0, height());
                glEnd();
            }
            glDisable(GL_TEXTURE_2D);
        }
    }
    end2D();
}

void Game3D::drawLOSE()
{
    begin2D();
    {
        drawButtons(flags);
        if (fun)
        {
            const double bWidth = width() / 8.;
            const double r = (bWidth > height() ? height() / 2. : bWidth / 2);

            const double x0 = width() / 6.;
            const double y0 = height() / 2. + 1.2 * r;
            const double hx = 4 * x0;

            glBindTexture(GL_TEXTURE_2D, texL);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, 3,
                lT->sizeX,
                lT->sizeY,
                0, GL_RGB, GL_UNSIGNED_BYTE,
                lT->data);
            glEnable(GL_TEXTURE_2D);
            {
                glBindTexture(GL_TEXTURE_2D, texL);
                glBegin(GL_QUADS);
                glColor3d(1, 1, 1);
                glTexCoord2d(0, 0); glVertex2f(x0, y0);
                glTexCoord2d(1, 0); glVertex2f(x0 + hx, y0);
                glTexCoord2d(1, 1); glVertex2f(x0 + hx, height());
                glTexCoord2d(0, 1); glVertex2f(x0, height());
                glEnd();
            }
            glDisable(GL_TEXTURE_2D);
        }
    }
    end2D();
}

void Game3D::setState(STATE newState)
{
    state = newState;
    flags.reset();
    glDisable(GL_LIGHTING);
    switch (state)
    {
    case Game3D::STATE::GAME:
    {
        glEnable(GL_LIGHTING);
        gameTimer->start(gameTimerStep);
        moveEnemyTimer->start(moveEnemyTimerStep);
        movePlayerTimer->start(movePlayerTimerStep);
        addMoveTimer->start(addMoveTimerStep);
        addFastMoveTimer->start(addFastMoveTimerStep);
        checkCollisionTimer->start(checkCollisionTimerStep);
        createBonusTimer->start(createBonusTimerStep);
        setWindowTitle("Very funny Game");
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
        flags |= 0b10101;
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
            const double hpLength = width() / 3.;
            const double hpHeight = height() / 9.;

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
            const double moneyLength = width() / 3.;
            const double moneyHeight = height() / 9.;

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

        // если меньше 60 секунд осталось, то часы двигаются
        glColor3f(1, 1, 1);
        const double x0 = width() / 2., y0 = height() / 18.;
        const double& r = y0;
        const double _2pi = 2 * acos(-1);
        glBegin(GL_TRIANGLE_FAN);
        {
            
            glVertex2f(x0, y0);
            for (size_t i = 0; i <= 60 && i <= gameTime; ++i)
            {
                double x = x0 + r * cos(i * _2pi / 60. + _2pi / 4.);
                double y = y0 + r * sin(i * _2pi / 60. + _2pi / 4.);
                glVertex2f(x, y);
            }

        }
        glEnd();
        

        // рисуем номер уровня, если выбран режим готовой игры, иначе ничего
        const double h = 2 * r / 3.;
        const double w = r / 6.;
        glBegin(GL_QUADS);
        {
            if (newGame == NewGame::LVL1)
            {
                glColor3f(0, 0.3, 1);

                glVertex2f(x0 - w, y0 - h);
                glVertex2f(x0 + w, y0 - h);
                glVertex2f(x0 + w, y0 + h);
                glVertex2f(x0 - w, y0 + h);
            }
            else if (newGame == NewGame::LVL2)
            {
                glColor3f(0.3, 0.7, 0.4);

                glVertex2f(x0 - w - 1.2 * w, y0 - h);
                glVertex2f(x0 + w - 1.2 * w, y0 - h);
                glVertex2f(x0 + w - 1.2 * w, y0 + h);
                glVertex2f(x0 - w - 1.2 * w, y0 + h);

                glVertex2f(x0 - w + 1.2 * w, y0 - h);
                glVertex2f(x0 + w + 1.2 * w, y0 - h);
                glVertex2f(x0 + w + 1.2 * w, y0 + h);
                glVertex2f(x0 - w + 1.2 * w, y0 + h);
            }
            else if (newGame == NewGame::LVL3)
            {
                glColor3f(0.9, 0.8, 0.6);

                glVertex2f(x0 - w - 2.2 * w, y0 - h);
                glVertex2f(x0 + w - 2.2 * w, y0 - h);
                glVertex2f(x0 + w - 2.2 * w, y0 + h);
                glVertex2f(x0 - w - 2.2 * w, y0 + h);

                glVertex2f(x0 - w, y0 - h);
                glVertex2f(x0 + w, y0 - h);
                glVertex2f(x0 + w, y0 + h);
                glVertex2f(x0 - w, y0 + h);

                glVertex2f(x0 - w + 2.2 * w, y0 - h);
                glVertex2f(x0 + w + 2.2 * w, y0 - h);
                glVertex2f(x0 + w + 2.2 * w, y0 + h);
                glVertex2f(x0 - w + 2.2 * w, y0 + h);
            }
        }
        glEnd();
   
    }
    glPopMatrix();
}

void Game3D::drawButtons(const std::bitset<5>& flags)
{
    auto drawCircle = [&](std::pair<float, float> c, float r, float alpha, int num_s) -> void
    {
        glBegin(GL_POLYGON);
        for (int i = 0; i < num_s; i++)
        {
            float theta = 2.0f * acos(-1) * float(i) / float(num_s);//get the current angle

            float x = r * cosf(alpha + theta);//calculate the x component
            float y = r * sinf(alpha + theta);//calculate the y component

            glVertex2f(x + c.first, y + c.second);//output vertex
        }
        glEnd();
    };

    const double bWidth = width() / 8.;

    const double r = (bWidth > height() ? height() / 2. : bWidth / 2);

    const double centerB = width() / 6.;

    std::pair<float, float> center{ centerB, height() / 2. };
    auto& [x, y] = center;

    if (flags[0]) // продолжить
    {
        if (newGame == NewGame::SELECTION)
            glColor3f(1, 160. / 255, 0);
        else
            glColor3f(0, 0.7, 0);
        drawCircle(center, r, 0, 40);

        glColor3f(0.9, 0.9, 0.9);
        drawCircle(center, r * 0.8, 0, 3);
    }

    x += centerB;
    if (flags[1]) // сохранить
    {
        glColor3f(0.4, 0.4, 0.4);
        drawCircle(center, r, 0, 40);

        glColor3f(0.9, 0.9, 0.9);
        double rr = r * 0.9;
        drawCircle(center, rr, 3.1415926 / 4, 4);

        glColor3f(0.4, 0.4, 0.4);
        glBegin(GL_POLYGON);
        {
            glVertex2f(x + rr / sqrt(2), y + rr / sqrt(2));
            glVertex2f(x + rr * 0.4, y + rr / sqrt(2));
            glVertex2f(x + rr / sqrt(2), y + rr * 0.4);
        }
        glEnd();

        glBegin(GL_POLYGON);
        {
            glVertex2f(x + rr * 0.55, y - rr * 0.1);
            glVertex2f(x + rr * 0.55, y - rr * 0.6);
            glVertex2f(x - rr * 0.55, y - rr * 0.6);
            glVertex2f(x - rr * 0.55, y - rr * 0.1);
        }
        glEnd();

        glBegin(GL_POLYGON);
        {
            glVertex2f(x + rr * 0.3, y + rr * 0.6);
            glVertex2f(x + rr * 0.3, y + rr * 0.1);
            glVertex2f(x - rr * 0.55, y + rr * 0.1);
            glVertex2f(x - rr * 0.55, y + rr * 0.6);
        }
        glEnd();

        glColor3f(0.9, 0.9, 0.9);

        glBegin(GL_POLYGON);
        {
            glVertex2f(x + rr * 0.25, y + rr * 0.55);
            glVertex2f(x + rr * 0.25, y + rr * 0.15);
            glVertex2f(x, y + rr * 0.15);
            glVertex2f(x, y + rr * 0.55);
        }
        glEnd();
    }

    x += centerB;
    if (flags[2]) // выбор
    {
        glColor3f(0, 0, 0.9);
        drawCircle(center, r, 0, 40);

        glColor3f(0.9, 0.9, 0.9);
        drawCircle({ x, y - r * 0.3 }, r * 0.5, acos(-1) / 3 * 2.5, 3);
        drawCircle({ x, y - r * 0.1 }, r * 0.2, acos(-1) / 4, 4);
        drawCircle({ x, y + r * 0.1 }, r * 0.2, acos(-1) / 4, 4);
        drawCircle({ x, y + r * 0.3 }, r * 0.2, acos(-1) / 4, 4);
        drawCircle({ x, y + r * 0.5 }, r * 0.2, acos(-1) / 4, 4);
    }

    x += centerB;
    if (flags[3]) // рестарт
    {
        glColor3f(1, 168. / 255, 0);
        drawCircle(center, r, 0, 40);

        glColor3f(0.9, 0.9, 0.9);

        double temp_r = 0.7 * r;
        glBegin(GL_POLYGON);
        {
            for (int i = 0; i < 35; i++)
            {
                float theta = 2.0f * acos(-1) * float(i) / 50.;//get the current angle

                float temp_x = temp_r * cosf(theta);//calculate the x component
                float temp_y = temp_r * sinf(theta);//calculate the y component

                glVertex2f(x + temp_x, y + temp_y);//output vertex

            }
        }
        glEnd();

        glColor3f(1, 168. / 255, 0);

        glBegin(GL_POLYGON);
        {
            temp_r = 0.5 * r;

            for (int i = 0; i < 35; i++)
            {
                float theta = 2.0f * acos(-1) * float(i) / 50.;//get the current angle


                float temp_x = temp_r * cosf(theta);//calculate the x component
                float temp_y = temp_r * sinf(theta);//calculate the y component

                glVertex2f(x + temp_x, y + temp_y);//output vertex

            }
        }
        glEnd();

        glBegin(GL_POLYGON);
        {

            temp_r = r;

            for (int i = 34; i < 55; i++)
            {
                float theta = 2.0f * acos(-1) * float(i) / 50.;//get the current angle

                float temp_x = temp_r * cosf(theta);//calculate the x component
                float temp_y = temp_r * sinf(theta);//calculate the y component

                glVertex2f(x + temp_x, y + temp_y);//output vertex

            }

            temp_r = r * 0.1;

            for (int i = 34; i < 55; i++)
            {
                float theta = 2.0f * acos(-1) * float(i) / 50.;//get the current angle

                float temp_x = temp_r * cosf(theta);//calculate the x component
                float temp_y = temp_r * sinf(theta);//calculate the y component

                glVertex2f(x + temp_x, y + temp_y);//output vertex

            }

        }
        glEnd();

        glColor3f(0.9, 0.9, 0.9);
        drawCircle({ x + 0.5 * r, y + 0.3 * r }, r * 0.3, acos(-1), 3);
    }

    x += centerB;
    if (flags[4]) // выход
    {
        glColor3f(0.9, 0, 0);
        drawCircle(center, r, 0, 40);

        glColor3f(0.9, 0.9, 0.9);
        glBegin(GL_POLYGON);
        {
            glVertex2f(x - r * 0.6, y - r * 0.4);
            glVertex2f(x + r * 0.4, y + r * 0.6);
            glVertex2f(x + r * 0.6, y + r * 0.4);
            glVertex2f(x - r * 0.4, y - r * 0.6);
        }
        glEnd();

        glBegin(GL_POLYGON);
        {
            glVertex2f(x + r * 0.6, y - r * 0.4);
            glVertex2f(x + r * 0.4, y - r * 0.6);
            glVertex2f(x - r * 0.6, y + r * 0.4);
            glVertex2f(x - r * 0.4, y + r * 0.6);
        }
        glEnd();
    }
}

bool Game3D::isValidCharacter(const char& c) const noexcept
{
    constexpr char valid_characters[] = { 'x', 'g', 'p', 'd', 'b', 'U', 'D', 'C', 'Q', 'e', 'o', 's', 'v', 'O'};

    for (auto&& el : valid_characters)
        if (c == el)
            return true;
    return false;
}

bool Game3D::isWall(const char& c) const noexcept
{
    constexpr char walls[] = { 'x', 'g', 'p', 'd', 'b', 'U', 'D', 'C', 'Q', 'O' };

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

        isNew = true;
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
        if (w > 40 || h > 40)
            throw std::exception("Size is too large! (Must be not greater than (40,40))");

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
                enemies[i] = new Enemy({ static_cast<int>(ii * cellSize + cellSize / 2), static_cast<int>(jj * cellSize + cellSize / 2) }, cellSize, t, eT, texE);
            }
            else
            {
                enemies[i] = new Enemy({ static_cast<int>(enSpawnPoints[i].second * cellSize + cellSize / 2), static_cast<int>(enSpawnPoints[i].first * cellSize + cellSize / 2) }, cellSize, t, eT, texE);
            }
        }

        auto spawnPoint = isNew ? spawnPoints[rand() % spawnPoints.size()] : std::pair<size_t, size_t>(i, j);

        if (p)
            delete p;

        p = new Player({ static_cast<int>(spawnPoint.second * cellSize + cellSize / 2), static_cast<int>(spawnPoint.first * cellSize + cellSize / 2) },
            cellSize,
            (isNew ? maxhp : hp), maxhp, pT, texP);

        for (auto&& bonus : bonuses) if (bonus) delete bonus;
        bonuses.clear();

        glLoadIdentity();
        glRotatef(-60, 1, 0, 0);
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
    std::ofstream OUTfile;
    if (!isNew)
        OUTfile.open(path.toStdString());
    else
        OUTfile.open((path.left(path.size() - 4) + "_current.txt").toStdString());

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
        glColor3f(255 / 255., 127 / 255., 80 / 255.);
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
        case 'O':
            glRotated(180, 0, 0, 1);
            gluCylinder(quadObj, r * l, r * l, h * l, 20, 20);
            glTranslated(0, 0, h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
            glTranslated(0, 0, -h * l);
            gluDisk(quadObj, 0, r * l, 20, 20);
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
