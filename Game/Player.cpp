#include "Player.h"

Player::Player(QPoint pos, int cellSize, int health, int maxHealth) noexcept
	: Entity(pos, cellSize), health(health), maxHealth(maxHealth)
{
}

int Player::getHP() const noexcept
{
	return health;
}

int Player::getMaxHP() const noexcept
{
	return maxHealth;
}

void Player::UpHP(int hp) noexcept
{
	health += hp;
	if (health > maxHealth)
		health = maxHealth;
}

void Player::DownHP(int hp) noexcept
{
	health -= hp;
	if (health < 0)
		health = 0;
}

Player::operator bool() const noexcept
{
	return health;
}

void Player::draw(std::pair<double, double> t0, double l) const noexcept
{
	auto [x0, y0] = t0;
    glPushMatrix();
    {
        glTranslatef(x0 + pos.x(), y0 + pos.y(), 0);
        if (!dirQ.empty())
            lastDir = dirQ.front().second;
        glRotatef(180, 0, 0, 1);
        switch (lastDir)
        {
        case MDIR::UP:
            glRotatef(180, 0, 0, 1);
            break;
        case MDIR::RIGHT:
            glRotatef(90, 0, 0, 1);
            break;
        case MDIR::DOWN:
            // 0
            break;
        case MDIR::LEFT:
            glRotatef(-90, 0, 0, 1);
            break;
        default:
            break;
        }

        glTranslated(0, 0, 10 * l);
        glColor3f(231 / 255.0, 142 / 255.0, 165 / 255.0);
        Draw_Parallepiped(0, 0, 0, 10 * l, 16 * l, 8 * l, 10, 10); // Туловище 8 по z на 16 по y, 10 по х
         //Голова: 8 8 8
        glPushMatrix();
        {
            glTranslated(0, 10 * l, 2 * l);
            if (!isFun)
            {
                Draw_Parallepiped(0, 0, 0, 8 * l, 8 * l, 8 * l, 10, 10);
                // Глаза:
                glTranslated(0, 4 * l, 0);
                glColor3f(0 / 255.0, 0 / 255.0, 0 / 255.0);
                Draw_Parallepiped(3.5 * l, 0, 0.5 * l, 0.999 * l, 0.001 * l, 1 * l, 1, 1);
                Draw_Parallepiped(-3.5 * l, 0, 0.5 * l, 0.999 * l, 0.001 * l, 1 * l, 1, 1);
                glColor3f(1, 1, 1);
                Draw_Parallepiped(2.5 * l, 0, 0.5 * l, 0.999 * l, 0.001 * l, 1 * l, 1, 1);
                Draw_Parallepiped(-2.5 * l, 0, 0.5 * l, 0.999 * l, 0.001 * l, 1 * l, 1, 1);
                // Нос: 
                glTranslated(0, 0.5 * l, -1.5 * l);
                glColor3f(231 / 255.0, 150 / 255.0, 165 / 255.0);
                Draw_Parallepiped(0, 0, 0, 4 * l, 1 * l, 3 * l, 1, 1);
                glColor3f(117 / 255.0, 59 / 255.0, 84 / 255.0);
                Draw_Parallepiped(0, 0, 0, 3.999 * l, 1.001 * l, 1 * l, 1, 1);
                glColor3f(231 / 255.0, 116 / 255.0, 165 / 255.0);
                Draw_Parallepiped(0, 0, 0, 2 * l, 1.002 * l, 1 * l, 1, 1);
            }
            else
            {
                // Лицо Андрюши
                glRotated(180, 0, 0, 1);
                Draw_Parallepiped(0, 0, 0, 7.999 * l, 7.999 * l, 7.999 * l, 10, 10);
                GLuint textures;
                glGenTextures(1, &textures);
                glBindTexture(GL_TEXTURE_2D, textures);
                //unsigned int photo_tex;
                AUX_RGBImageRec* photo_image = auxDIBImageLoad(L"D:\\VisualStudio22\\Codes\\Game\\Game\\Hero.bmp");
                glEnable(GL_TEXTURE_2D);
                //glBindTexture(GL_TEXTURE_2D, photo_tex);
                glBegin(GL_QUADS);
                glColor3d(1, 1, 1);
                glTranslated(0, 0, 4 * l);
                glTexCoord2d(0, 0); glVertex3d(-4 * l, -4 * l, -4 * l);
                glTexCoord2d(0, 1); glVertex3d(-4 * l, -4 * l, 4 * l);
                glTexCoord2d(1, 1); glVertex3d(4 * l, -4 * l, 4 * l);
                glTexCoord2d(1, 0); glVertex3d(4 * l, -4 * l, -4 * l);
                glEnd();

                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexImage2D(GL_TEXTURE_2D, 0, 3,
                    photo_image->sizeX,
                    photo_image->sizeY,
                    0, GL_RGB, GL_UNSIGNED_BYTE,
                    photo_image->data);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glDisable(GL_TEXTURE_2D);
                glEnable(GL_DEPTH_TEST);
            }
        }
        glPopMatrix();
        // Ноги
        glPushMatrix();
        {
            glTranslated(0, 0 * l, -7 * l);
            for (int i = 0; i < 4; i++)
            {
                glPushMatrix();
                glColor3f(231 / 255.0, 142 / 255.0, 165 / 255.0);
                if (i == 0)
                {
                    glTranslated(0, -1 * l + abs(heroPar * 0.1 * l), 0);

                    glTranslated(-3 * l, 5 * l, 0);
                }
                else if (i == 1)
                {
                    glTranslated(0, -1 * l + abs(heroPar * 0.1 * l), 0);
                    glTranslated(-3 * l, -7 * l, 0);
                }
                else if (i == 2)
                {
                    glTranslated(0, -(-2 * l + abs(heroPar * 0.1 * l)), 0);
                    glTranslated(3 * l, 5 * l, 0);
                }
                else if (i == 3)
                {
                    glTranslated(0, -(-2 * l + abs(heroPar * 0.1 * l)), 0);
                    glTranslated(3 * l, -7 * l, 0);
                }
                Draw_Parallepiped(0, 0, 0, 4 * l, 4 * l, 6 * l, 5, 5);
                glColor3f(117 / 255.0, 59 / 255.0, 84 / 255.0);
                Draw_Parallepiped(1.49 * l, 0, -2.51 * l, 1 * l, 4.001 * l, 1 * l, 5, 5);
                Draw_Parallepiped(-0.49 * l, 0, -2.51 * l, 1 * l, 4.001 * l, 1 * l, 5, 5);
                glPopMatrix();
            }
        }
        glPopMatrix();
        
    }
    glPopMatrix();
    if (abs(heroPar) == 30)
        heroPar *= -1;
    heroPar += 6;
}

bool Player::addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept
{
	int x = nextPos.x(), y = nextPos.y();

	switch (dir)
	{
	case MDIR::UP:
		y += cellSize;
		break;
	case MDIR::RIGHT:
		x += cellSize;
		break;
	case MDIR::DOWN:
		y -= cellSize;
		break;
	case MDIR::LEFT:
		x -= cellSize;
		break;
	}

	int ix = x / cellSize, iy = y / cellSize;

	if (iy >= lvl.size() || ix >= lvl[0].size() || ix < 0 || iy < 0)
		return false;

	if (lvl[iy][ix] && dirQ.size() < 2)
	{
		dirQ.push({ 0, dir });
		nextPos = QPoint(x, y);
       
		return true;
	}

	return false;
}


void Player::move() noexcept
{
	if (!isReadyToMove())
		return; // check if idiot


	switch (dirQ.front().second)
	{
	case MDIR::UP:
		pos.ry() += cellSize / 10;
		break;
	case MDIR::RIGHT:
		pos.rx() += cellSize / 10;
		break;
	case MDIR::DOWN:
		pos.ry() -= cellSize / 10;
		break;
	case MDIR::LEFT:
		pos.rx() -= cellSize / 10;
		break;
	}
	++dirQ.front().first;

	if (dirQ.front().first >= 10)
		dirQ.pop();
}
