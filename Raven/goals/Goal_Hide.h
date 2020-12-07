#pragma once

#pragma warning (disable:4786)

#include "Raven_Goal_Types.h"
#include "../Common/Goals/Goal_Composite.h"

class Raven_Bot;

class Goal_Hide : public Goal_Composite<Raven_Bot>
{
private:

public:
	Goal_Hide(Raven_Bot* pOwner) :Goal_Composite<Raven_Bot>(pOwner, goal_hide) {}

	void Activate();

	int Process();

	void Terminate() { m_iStatus = completed; }

	Vector2D FindAPointToHide(Raven_Bot*pTarget);

	bool HandleMessage(const Telegram& msg);
};

