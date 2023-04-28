#pragma once

#include "../ImaseLib/TaskManager.h"

class Number : public Imase::Task
{
public:

	// コンストラクタ
	Number();

	// デストラクタ
	~Number();

	// 更新
	bool Update(float elapsedTime) override;

	// 描画
	void Render() override;

};
