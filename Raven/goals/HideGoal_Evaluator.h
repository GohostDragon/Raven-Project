#pragma once
#pragma warning(disable:4786)

#include "../Raven/goals/Goal_Evaluator.h"
#include "../Raven/Raven_Bot.h"

class HideGoal_Evaluator : public Goal_Evaluator
{

public:
	HideGoal_Evaluator(double bias) : Goal_Evaluator(bias) {}

	double CalculateDesirability(Raven_Bot* pBot);

	void SetGoal(Raven_Bot* pEnt);

	void  RenderInfo(Vector2D Position, Raven_Bot* pBot) {}
};
