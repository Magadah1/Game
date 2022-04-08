#include "Enemy.h"

Enemy::Enemy(QPoint pos, int cellSize, ETYPE type) noexcept
	: Entity(pos, cellSize), type(type)
{
	glGenTextures(1, &textures);
	photo_image = auxDIBImageLoad(L"Z.bmp");
}

Enemy::ETYPE Enemy::getType() const noexcept
{
	return type;
}

bool Enemy::addMove(const std::vector<std::vector<bool>>& lvl, std::pair<int, int> pPos) noexcept
{

	if (type == ETYPE::ONE)
		return addMove(static_cast<MDIR>(rand() % 4), lvl);
	else
	{
		if (isReadyToMove())
			return false;

		if (pPos == getIndexPos())
			return false;

		//std::vector<std::vector<bool>> reversedLvl = lvl;
		//for (auto&& l : reversedLvl) std::reverse(l.begin(), l.end());
		std::pair<int, int> revPpos = { pPos.second, pPos.first };
		std::pair<int, int> revMpos = { getIndexPos().second, getIndexPos().first };
		//std::reverse(reversedLvl.begin(), reversedLvl.end());

		// lvl = reversedLvl
		std::vector<std::vector<int>> map;
		map.resize(lvl.size());
		for (auto&& line : map)
			line.resize(lvl[0].size());
		for (size_t i = 0; i < map.size(); ++i)
			for (size_t j = 0; j < map[i].size(); ++j)
				map[i][j] = (type == ETYPE::TWO) ? -1 : (lvl[i][j] ? -1 : 9999);

		if (type == ETYPE::TWO)
			for (size_t i = 0; i < map[0].size(); ++i)
				map[0][i] = map[map.size() - 1][i] = map[i][0] = map[i][map[0].size() - 1] = 9999;

		int row = map.size();
		int col = map[0].size();
		std::vector<std::pair<int, int>> oldWave, wave;
		oldWave.push_back(revPpos);
		int nstep = 0;
		map[oldWave[0].first][oldWave[0].second] = nstep; // nstep - 3
		const int dx[] = { 0, 1, 0, -1 };
		const int dy[] = { -1, 0, 1, 0 };
		while (oldWave.size())
		{
			nstep++;
			wave.clear();
			for (std::vector<std::pair<int, int> >::iterator i = oldWave.begin(); i != oldWave.end(); ++i)
			{
				for (int j = 0; j < 4; j++)
				{
					int nx = i->first + dx[j];
					int ny = i->second + dy[j];
					if (map[nx][ny] == -1) // 0
					{
						wave.push_back(std::pair<int, int>(nx, ny));
						map[nx][ny] = nstep;
						if (nx == revMpos.first && ny == revMpos.second)
							goto done;
					}
				}
			}
			oldWave = wave;
		}
	done:
		int x = revMpos.first;
		int y = revMpos.second;
		wave.clear();
		wave.push_back(std::pair<int, int>(x, y));
		map[revPpos.first][revPpos.second] = 0;
		while (map[x][y] != 0)
		{
			for (int i = 0; i < 4; i++)
			{
				int nx = x + dx[i];
				int ny = y + dy[i];
				if (map[x][y] - 1 == map[nx][ny])
				{
					x = nx;
					y = ny;
					wave.push_back(std::pair<int, int>(x, y));
					break;
				}
			}
		}

		std::vector<MDIR> Way;

		for (size_t i = 0; i < wave.size() - 1; ++i)
		{
			int dy1 = wave[i + 1].first - wave[i].first;
			int dx1 = wave[i + 1].second - wave[i].second;

			if (dx1 == -1 && dy1 == 0)
				Way.push_back(MDIR::LEFT);
			else if (dx1 == 1 && dy1 == 0)
				Way.push_back(MDIR::RIGHT);
			else if (dx1 == 0 && dy1 == 1)
				Way.push_back(MDIR::UP);
			else if (dx1 == 0 && dy1 == -1)
				Way.push_back(MDIR::DOWN);

		}

		bool retValue = false;

		for (auto&& Move : Way)
			retValue |= addMove(Move, lvl);

		return retValue;
	}
	
}

void Enemy::draw(std::pair<double, double> t0, double l) const noexcept
{
	auto [x0, y0] = t0;
	glPushMatrix();
	{
		if (!dirQ.empty())
			lastDir = dirQ.front().second;
		
		glTranslatef(x0 + pos.x(), y0 + pos.y(), 0);
		switch (type)
		{
		case ETYPE::ONE:
		{
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

			glTranslated(0, 0, 19 * l);
			glColor3f(66 / 255.0, 170 / 255.0, 255 / 255.0);
			Draw_Parallepiped(0, 0, 0, 8 * l, 4 * l, 10 * l, 10, 10); // Туловище 10 по z на 8 по y, 4 по х
			// Голова:
			glPushMatrix();
			{
				glTranslated(0, 0, 5 * l);
				glColor3f(99 / 255.0, 150 / 255.0, 83 / 255.0);
				if (!isFun)
				{
					Draw_Parallepiped(0, 0, 4 * l, 8 * l, 8 * l, 8 * l, 10, 10);
					// Глаза:
					glColor3f(20 / 255.0, 20 / 255.0, 20 / 255.0);
					glPushMatrix();
					{
						glTranslated(0, -4 * l, 4 * l);
						Draw_Parallepiped(-2 * l, 0, 0, 2 * l, 0.01 * l, 1 * l, 2, 2);
						Draw_Parallepiped(2 * l, 0, 0, 2 * l, 0.01 * l, 1 * l, 2, 2);
					}
					glPopMatrix();
				}
				else
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexImage2D(GL_TEXTURE_2D, 0, 3,
						photo_image->sizeX,
						photo_image->sizeY,
						0, GL_RGB, GL_UNSIGNED_BYTE,
						photo_image->data);
					glEnable(GL_TEXTURE_2D);
					{
						glBindTexture(GL_TEXTURE_2D, textures);
						GLUquadricObj* quadObj;
						quadObj = gluNewQuadric();
						gluQuadricTexture(quadObj, GL_TRUE);
						gluQuadricDrawStyle(quadObj, GLU_FILL);
						glColor3d(1, 1, 1);
						glRotated(5, 0, 1, 0);
						glPushMatrix();
						{
							glTranslated(0, 0, 3.5 * l);
							glRotated(15, 0, 0, 1);
							glRotated(-5, 0, 1, 0);
							gluSphere(quadObj, 4.5 * l, 16, 16);
						}
						glPopMatrix();
						gluDeleteQuadric(quadObj);
					}
					glDisable(GL_TEXTURE_2D);
				}
			}
			glPopMatrix();
			// Руки:
			glPushMatrix();
			{
				glTranslated(0, 0, 5 * l);
				// Плотная на 90 градусов) :
				/*glRotatef(-90, 1, 0, 0);
				glTranslated(0 * l, 2 * l, 2 * l);*/
				glColor3f(66 / 255.0, 170 / 255.0, 255 / 255.0);
				Draw_Parallepiped(-6 * l, 0, -2 * l, 4 * l, 4 * l, 4 * l, 2, 2);
				Draw_Parallepiped(6 * l, 0, -2 * l, 4 * l, 4 * l, 4 * l, 2, 2);
				glTranslated(0, 0, -4 * l);
				glColor3f(99 / 255.0, 150 / 255.0, 83 / 255.0);
				Draw_Parallepiped(-6 * l, 0, -4 * l, 4 * l, 4 * l, 8 * l, 2, 2);
				Draw_Parallepiped(6 * l, 0, -4 * l, 4 * l, 4 * l, 8 * l, 2, 2);
			}
			glPopMatrix();
			// Ноги:
			glPushMatrix();
			{
				glTranslated(0, 0, -5 * l);

				glPushMatrix();
				{
					glRotated(-15 + abs(heroPar), 1, 0, 0);
					glColor3f(127 / 255.0, 0 / 255.0, 255 / 255.0);
					Draw_Parallepiped(-2 * l, 0, -6 * l, 4 * l, 4 * l, 12 * l, 5, 5);
					glTranslated(0, 0, -12 * l);
					glColor3f(102 / 255.0, 92 / 255.0, 85 / 255.0);
					Draw_Parallepiped(-2 * l, 0, -1 * l, 4 * l, 4 * l, 2 * l, 2, 2);
				}
				glPopMatrix();

				glPushMatrix();
				{
					glRotated(-(-15 + abs(heroPar)), 1, 0, 0);
					glColor3f(127 / 255.0, 0 / 255.0, 255 / 255.0);
					Draw_Parallepiped(2 * l, 0, -6 * l, 4 * l, 4 * l, 12 * l, 5, 5);
					glTranslated(0, 0, -12 * l);
					glColor3f(102 / 255.0, 92 / 255.0, 85 / 255.0);
					Draw_Parallepiped(2 * l, 0, -1 * l, 4 * l, 4 * l, 2 * l, 2, 2);
				}
				glPopMatrix();
			}
			glPopMatrix();
			/*if (abs(heroPar) >= 30)
				heroPar *= -1;
			heroPar += 3;*/
		}
			break;

		case ETYPE::TWO:
		{
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
			
			glTranslated(0, 0, 35 * l);
			glColor3f(26 / 255.0, 26 / 255.0, 26 / 255.0);
			Draw_Parallepiped(0, 0, 0, 8 * l, 5 * l, 16 * l, 10, 10); // Туловище
			// Голова
			glPushMatrix();
			{
				glTranslated(0, 0, 8 * l);
				Draw_Parallepiped(0, 0, 0, 8 * l, 8 * l, 8 * l, 10, 10);
				// Глаза
				glColor3f(179 / 255.0, 102 / 255.0, 255 / 255.0);
				glPushMatrix();
				{
					glTranslated(-2 * l, -4 * l, 0);
					Draw_Parallepiped(0, 0, 0, 2 * l, 1 * l, 1 * l, 2, 2);
					glColor3f(139 / 255.0, 0 / 255.0, 255 / 255.0);
					Draw_Parallepiped(0, 0, 0, 1 * l, 1.001 * l, 1 * l, 2, 2);
				}
				glPopMatrix();
				glColor3f(179 / 255.0, 102 / 255.0, 255 / 255.0);
				glPushMatrix();
				{
					glTranslated(2 * l, -4 * l, 0);
					Draw_Parallepiped(0, 0, 0, 2 * l, 1 * l, 1 * l, 2, 2);
					glColor3f(139 / 255.0, 0 / 255.0, 255 / 255.0);
					Draw_Parallepiped(0, 0, 0, 1 * l, 1.001 * l, 1 * l, 2, 2);
				}
				glPopMatrix();
			}
			glPopMatrix();
			glColor3f(26 / 255.0, 26 / 255.0, 26 / 255.0);
			// Руки
			glPushMatrix();
			{
				glTranslated(5 * l, 0, 5 * l);
				glRotated(-15 + abs(heroPar), 1, 0, 0);
				Draw_Parallepiped(0, 0, -15 * l, 2 * l, 2 * l, 30 * l, 10, 10);
			}
			glPopMatrix();
			glPushMatrix();
			{
				glTranslated(-5 * l, 0, 5 * l);
				glRotated(-(-15 + abs(heroPar)), 1, 0, 0);
				Draw_Parallepiped(0, 0, -15 * l, 2 * l, 2 * l, 30 * l, 10, 10);
			}
			glPopMatrix();
			// Ноги
			glPushMatrix();
			{
				glTranslated(2 * l, 0, -2 * l);
				glRotated(-(-15 + abs(heroPar)), 1, 0, 0);
				Draw_Parallepiped(0, 0, -20 * l, 2 * l, 2 * l, 30 * l, 10, 10);
			}
			glPopMatrix();
			glPushMatrix();
			{
				glTranslated(-2 * l, 0, -2 * l);
				glRotated(-15 + abs(heroPar), 1, 0, 0);
				Draw_Parallepiped(0, 0, -20 * l, 2 * l, 2 * l, 30 * l, 10, 10);
			}
			glPopMatrix();
			/*if (abs(heroPar) >= 30)
				heroPar *= -1;
			heroPar += 1;*/
		}
			break;

		case ETYPE::THREE:
		{
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
			glTranslated(0, 0, 8.4 * l);
			glColor3f(192 / 255.0, 192 / 255.0, 192 / 255.0);
			Draw_Parallepiped(0, 0, 0, 6 * l, 15 * l, 6 * l, 5, 5); // Туловище 10 по z на 8 по y, 4 по х
			glPushMatrix();
			{   // Грива
				glTranslated(0 * l, 1.5 * l, 0 * l);
				Draw_Parallepiped(0, 3 * l, 0.5 * l, 8 * l, 6 * l, 7 * l, 5, 5);
				// Голова
				glTranslated(0 * l, 6 * l, 0 * l);
				Draw_Parallepiped(0, 1.5 * l, 0, 6 * l, 3 * l, 6 * l, 5, 5);
				glColor3f(202 / 255.0, 202 / 255.0, 202 / 255.0);
				Draw_Parallepiped(-2 * l, 0.5 * l, 4 * l, 2 * l, 1 * l, 2 * l, 5, 5);
				Draw_Parallepiped(2 * l, 0.5 * l, 4 * l, 2 * l, 1 * l, 2 * l, 5, 5);
				// Глаза
				glTranslated(0 * l, 3 * l, 0 * l);
				glColor3f(255 / 255.0, 255 / 255.0, 255 / 255.0);
				Draw_Parallepiped(-2 * l, 0, 0.5 * l, 2 * l, 0.01 * l, 1 * l, 5, 5);
				Draw_Parallepiped(2 * l, 0, 0.5 * l, 2 * l, 0.01 * l, 1 * l, 5, 5);
				glColor3f(0 / 255.0, 0 / 255.0, 0 / 255.0);
				Draw_Parallepiped(-1.5 * l, 0, 0.5 * l, 1 * l, 0.1 * l, 1 * l, 5, 5);
				Draw_Parallepiped(1.5 * l, 0, 0.5 * l, 1 * l, 0.1 * l, 1 * l, 5, 5);
				glColor3f(255 / 255.0, 0 / 255.0, 0 / 255.0);
				Draw_Parallepiped(-1.5 * l, 0, 0.5 * l, 0.5 * l, 0.2 * l, 0.5 * l, 5, 5);
				Draw_Parallepiped(1.5 * l, 0, 0.5 * l, 0.5 * l, 0.2 * l, 0.5 * l, 5, 5);
				glColor3f(255 / 255.0, 178 / 255.0, 102 / 255.0);
				Draw_Parallepiped(0, 0, -1 * l, 6 * l, 0.001 * l, 4 * l, 5, 5);
				// Нос
				glColor3f(192 / 255.0, 192 / 255.0, 192 / 255.0);
				Draw_Parallepiped(0, 1.5 * l, -1.5 * l, 3 * l, 3 * l, 3 * l, 5, 5);
				glColor3f(0 / 255.0, 0 / 255.0, 0 / 255.0);
				Draw_Parallepiped(0, 1.5 * l, -2.5 * l, 3.01 * l, 3.01 * l, 1.01 * l, 5, 5);
				glTranslated(0 * l, 2.5 * l, 0 * l);
				Draw_Parallepiped(0, 0 * l, -0.5 * l, 1.01 * l, 1.01 * l, 1.01 * l, 5, 5);
			}
			glPopMatrix();
			// Хвост
			glPushMatrix();
			{
				glColor3f(192 / 255.0, 192 / 255.0, 192 / 255.0);
				glTranslated(0 * l, -8 * l, 0 * l);
				glRotatef(37, 1, 0, 0);
				Draw_Parallepiped(0, -2 * l, 0 * l, 2 * l, 7 * l, 2 * l, 5, 5);

			}
			glPopMatrix();
			// Ноги
			glTranslated(0, 0, -3 * l);
			glPushMatrix();
			{
				glPushMatrix();
				{
					glTranslated(-2 * l, 6.5 * l, 0 * l);
					glRotated(-(-22.5 + abs(heroPar)), 1, 0, 0);
					Draw_Parallepiped(0, 0, -2.5 * l, 2 * l, 2 * l, 6 * l, 5, 5);
				}
				glPopMatrix();
				glPushMatrix();
				{
					glTranslated(-2 * l, -6.5 * l, 0 * l);
					glRotated(-(-22.5 + abs(heroPar)), 1, 0, 0);
					Draw_Parallepiped(0, 0, -2.5 * l, 2 * l, 2 * l, 6 * l, 5, 5);
				}
				glPopMatrix();
				glPushMatrix();
				{
					glTranslated(2 * l, 6.5 * l, 0 * l);
					glRotated(-22.5 + abs(heroPar), 1, 0, 0);
					Draw_Parallepiped(0, 0, -2.5 * l, 2 * l, 2 * l, 6 * l, 5, 5);
				}
				glPopMatrix();
				glPushMatrix();
				{
					glTranslated(2 * l, -6.5 * l, 0 * l);
					glRotated(-22.5 + abs(heroPar), 1, 0, 0);
					Draw_Parallepiped(0, 0, -2.5 * l, 2 * l, 2 * l, 6 * l, 5, 5);
				}
				glPopMatrix();

			}
			glPopMatrix();
			/*if (abs(heroPar) >= 45)
				heroPar *= -1;
			heroPar+=5;*/
		}
			break;
		case ETYPE::FOUR:
		{
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
			glTranslated(0, 0, 25 * l);
			glColor3f(0 / 255.0, 204 / 255.0, 0 / 255.0);
			// Голова 8 8 8
			Draw_Parallepiped(0, 0, 0, 8 * l, 8 * l, 8 * l, 5, 5);
			glPushMatrix();
			{
				glTranslated(0, 4.001 * l, 0);
				// Глаза
				glColor3f(50 / 255.0, 50 / 255.0, 50 / 255.0);
				Draw_Parallepiped(-2 * l, 0, 1 * l, 2 * l, 0.01 * l, 2 * l, 5, 5);
				Draw_Parallepiped(2 * l, 0, 1 * l, 2 * l, 0.01 * l, 2 * l, 5, 5);
				// Зрачки
				glColor3f(0 / 255.0, 0 / 255.0, 0 / 255.0);
				Draw_Parallepiped(-1.5 * l, 0, 0.5 * l, 1 * l, 0.1 * l, 1 * l, 5, 5);
				Draw_Parallepiped(1.5 * l, 0, 0.5 * l, 1 * l, 0.1 * l, 1 * l, 5, 5);
				glColor3f(0 / 255.0, 204 / 255.0, 0 / 255.0);
				// Рот
				glColor3f(0 / 255.0, 0 / 255.0, 0 / 255.0);
				Draw_Parallepiped(0, 0, -2 * l, 2 * l, 0.1 * l, 2 * l, 5, 5);
				Draw_Parallepiped(0, 0, -2.5 * l, 4 * l, 0.1 * l, 1 * l, 5, 5);
				glColor3f(50 / 255.0, 50 / 255.0, 50 / 255.0);
				Draw_Parallepiped(0, 0, -1.5 * l, 2 * l, 0.01 * l, 3 * l, 5, 5);
				Draw_Parallepiped(0, 0, -2 * l, 4 * l, 0.01 * l, 2 * l, 5, 5);
				Draw_Parallepiped(-1.5 * l, 0, -2.5 * l, 1 * l, 0.01 * l, 3 * l, 5, 5);
				Draw_Parallepiped(1.5 * l, 0, -2.5 * l, 1 * l, 0.01 * l, 3 * l, 5, 5);
				glColor3f(0 / 255.0, 204 / 255.0, 0 / 255.0);
			}
			glPopMatrix();
			// Туловище:
			glTranslated(0, 0, -4 * l);
			Draw_Parallepiped(0, 0, -6 * l, 8 * l, 4 * l, 12 * l, 5, 5);
			// Ноги:
			glTranslated(0, 0, -12 * l);
			glPushMatrix();
			{
				glRotated(-(-15 + abs(heroPar)), 1, 0, 0);
				Draw_Parallepiped(-2 * l, -3.5 * l, -4 * l, 4 * l, 4 * l, 8 * l, 5, 5);
				Draw_Parallepiped(-2 * l, 3.5 * l, -4 * l, 4 * l, 4 * l, 8 * l, 5, 5);
			}
			glPopMatrix();
			glPushMatrix();
			{
				glRotated(-15 + abs(heroPar), 1, 0, 0);
				Draw_Parallepiped(2 * l, -3.5 * l, -4 * l, 4 * l, 4 * l, 8 * l, 5, 5);
				Draw_Parallepiped(2 * l, 3.5 * l, -4 * l, 4 * l, 4 * l, 8 * l, 5, 5);
			}
			glPopMatrix();
			/*if (abs(heroPar) == 30)
				heroPar *= -1;
			heroPar++;*/
		}
			break;
		default:
			break;
		}
	}
	glPopMatrix();
}

void Enemy::move() noexcept
{
	if (!isReadyToMove())
		return; // check if idiot

	switch (type)
	{
	case ETYPE::ONE:
	{
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

		if (abs(heroPar) >= 30)
			heroPar *= -1;
		heroPar += 3;
	}
	break;
	case ETYPE::TWO:
	{
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
		{
			std::queue<std::pair<int, MDIR>> empty;
			dirQ.swap(empty);
			nextPos = pos;
		}

		if (abs(heroPar) >= 30)
			heroPar *= -1;
		heroPar += 1;
	}
	break;
	case ETYPE::THREE:
	{
		switch (dirQ.front().second)
		{
		case MDIR::UP:
			pos.ry() += cellSize / 5;
			break;
		case MDIR::RIGHT:
			pos.rx() += cellSize / 5;
			break;
		case MDIR::DOWN:
			pos.ry() -= cellSize / 5;
			break;
		case MDIR::LEFT:
			pos.rx() -= cellSize / 5;
			break;
		}
		++dirQ.front().first;

		if (dirQ.front().first >= 5)
			dirQ.pop();

		if (abs(heroPar) >= 45)
			heroPar *= -1;
		heroPar += 5;
	}
	break;
	case ETYPE::FOUR:
	{
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
		{
			std::queue<std::pair<int, MDIR>> empty;
			dirQ.swap(empty);
			nextPos = pos;
		}

		if (abs(heroPar) == 30)
			heroPar *= -1;
		heroPar++;
	}
	break;
	default:
		break;
	}
}

bool Enemy::addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept
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


	if (lvl[iy][ix])
	{
		dirQ.push({ 0, dir });
		nextPos = QPoint(x, y);
		
		return true;
	}
	return false;
}
