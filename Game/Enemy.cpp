#include "Enemy.h"

Enemy::Enemy(QPoint pos, int cellSize, ETYPE type) noexcept
	: Entity(pos, cellSize), type(type)
{
}

Enemy::ETYPE Enemy::getType() const noexcept
{
	return type;
}

bool Enemy::addMove(const std::vector<std::vector<bool>>& lvl, std::pair<int, int> pPos) noexcept
{
	switch (type)
	{
	case ETYPE::ONE:
	{
		return addMove(static_cast<MDIR>(rand() % 4), lvl);
	}
	break;
	
	case ETYPE::TWO:
	{
		if (isReadyToMove())
			return false;

		if (pPos == getIndexPos())
			return false;

		std::vector<std::vector<bool>> reversedLvl = lvl;
		for (auto&& l : reversedLvl)
			std::reverse(l.begin(), l.end());
		std::pair<int, int> revPpos = { pPos.second, pPos.first };
		std::pair<int, int> revMpos = { getIndexPos().second, getIndexPos().first };
		//std::reverse(reversedLvl.begin(), reversedLvl.end());

		// lvl = reversedLvl
		std::vector<std::vector<int>> map;
		map.resize(reversedLvl.size());
		for (auto&& line : map)
			line.resize(reversedLvl[0].size());
		for (size_t i = 0; i < map.size(); ++i)
			for (size_t j = 0; j < map[i].size(); ++j)
				map[i][j] = -1;

		for (size_t i = 0; i < map[0].size(); ++i)
			map[0][i] = map[map.size() - 1][i] = 9999;

		for (size_t i = 0; i < map.size(); ++i)
			map[i][0] = map[i][map[0].size() - 1] = 9999;

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
							goto done2;
					}
				}
			}
			oldWave = wave;
		}
	done2:
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

		//
		int test = dirQ.size();

		return retValue;
	}
	break;
	case ETYPE::THREE:
	{
		if (isReadyToMove())
			return false;

		if (pPos == getIndexPos())
			return false;
		
		std::vector<std::vector<bool>> reversedLvl = lvl;
		for (auto&& l : reversedLvl)
			std::reverse(l.begin(), l.end());
		std::pair<int, int> revPpos = { pPos.second, pPos.first };
		std::pair<int, int> revMpos = { getIndexPos().second, getIndexPos().first };
		//std::reverse(reversedLvl.begin(), reversedLvl.end());

		// lvl = reversedLvl
		std::vector<std::vector<int>> map;
		map.resize(reversedLvl.size());
		for (auto&& line : map)
			line.resize(reversedLvl[0].size());
		for (size_t i = 0; i < map.size(); ++i)
			for (size_t j = 0; j < map[i].size(); ++j)
				map[i][j] = reversedLvl[i][j] ? -1 : 9999; // 0 : -1 

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
							goto done3;
					}
				}
			}
			oldWave = wave;
		}
	done3:
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

		//
		int test = dirQ.size();

		return retValue;
	}
	break;
	default:
		break;
	}
}

void Enemy::draw() const noexcept
{
	// test
	switch (type)
	{
	case ETYPE::ONE:
		glColor3f(0.66, 0.66, 0.66);
		glBegin(GL_QUADS);
		{
			glVertex2f(pos.x() - cellSize / 2, pos.y());
			glVertex2f(pos.x(), pos.y() + cellSize / 2);
			glVertex2f(pos.x() + cellSize / 2, pos.y());
			glVertex2f(pos.x(), pos.y() - cellSize / 2);
		}
		glEnd();
		break;

	case ETYPE::TWO:
		glColor3f(0.45, 0.60, 0.90);
		glBegin(GL_QUADS);
		{
			glVertex2f(pos.x() - cellSize / 2, pos.y());
			glVertex2f(pos.x(), pos.y() + cellSize / 4);
			glVertex2f(pos.x() + cellSize / 2, pos.y());
			glVertex2f(pos.x(), pos.y() - cellSize / 4);
		}
		glEnd();
		break;

	case ETYPE::THREE:
		glColor3f(0.1, 0.9, 0.45);
		glBegin(GL_QUADS);
		{
			glVertex2f(pos.x() - cellSize / 4, pos.y());
			glVertex2f(pos.x(), pos.y() + cellSize / 4);
			glVertex2f(pos.x() + cellSize / 4, pos.y());
			glVertex2f(pos.x(), pos.y() - cellSize / 4);
		}
		glEnd();
		break;
	default:
		break;
	}
	
}

void Enemy::move() noexcept
{
	if (!isReadyToMove())
		return; // check if idiot

	switch (type)
	{
	case ETYPE::ONE:
	case ETYPE::TWO:
	case ETYPE::THREE:
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
	}
	break;
	/*case ETYPE::TWO:
	{

	}
	break;*/
	/*case ETYPE::THREE:
	{

	}
	break;*/
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
