#pragma once

#include "Entity.h"

class Enemy :
	public Entity
{
public:
	typedef enum class ENEMY_TYPE
	{
		ONE = 1,	// рсони еакюм, ундхр пюмднлмн (гнлах)
		TWO,		// ме бхдхр ярем дюсм еаюмши (щмдеплем)
		THREE,		// нвемэ ашярпюъ ясйю (о╗я)
		FOUR		// слмши век цнпфсяэ хл (йпхоеп)
	} ETYPE;

	Enemy(QPoint pos, int cellSize, ETYPE type = ETYPE::ONE) noexcept;
	~Enemy();

	ETYPE getType() const noexcept;

	bool addMove(const std::vector<std::vector<bool>>& lvl, std::pair<int, int> pPos) noexcept;

	virtual void draw(std::pair<double, double> t0, double l) const noexcept override final;

	virtual void move() noexcept override final;

	virtual bool addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept override final;
private:
	ETYPE type;
};
