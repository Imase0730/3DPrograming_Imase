#pragma once

#include "../ImaseLib/TaskManager.h"

class Number : public Imase::Task
{
public:

	// �R���X�g���N�^
	Number();

	// �f�X�g���N�^
	~Number();

	// �X�V
	bool Update(float elapsedTime) override;

	// �`��
	void Render() override;

};
