#include "Entity.h"

class Enemy :
	public Entity
{
public:
	typedef enum class ENEMY_TYPE
	{
		ONE = 1,
		TWO,
		THREE,
		FOUR
	} ETYPE;

	Enemy(QPoint pos, int cellSize, ETYPE type = ETYPE::ONE) noexcept;

	ETYPE getType() const noexcept;

	bool addMove(const std::vector<std::vector<bool>>& lvl, std::pair<int, int> pPos) noexcept;

	virtual void draw() const noexcept override final;

	virtual void move() noexcept override final;

	virtual bool addMove(MDIR dir, const std::vector<std::vector<bool>>& lvl) noexcept override final;
private:
	ETYPE type;
};
