#include "HideGoal_Evaluator.h"
#include "Goal_Think.h"
#include "Raven_Feature.h"
#include "../Raven/Raven_Game.h"
#include "../Raven/Raven_Map.h"
#include"../Common/Graph/SparseGraph.h"
#include "../Common/Debug/DebugConsole.h"
#include "../Common/misc/Stream_Utility_Functions.h"


//------------------- CalculateDesirability ---------------------------------
//-----------------------------------------------------------------------------
double HideGoal_Evaluator::CalculateDesirability(Raven_Bot* pBot) {

	double Desirability = 0.0;
	const double Tweaker = 1.0;

	double Health;
	double Distance;
	bool Visible;
	Vector2D Position;
	Vector2D TargetPosition;

	Position = pBot->Pos();
	Health = Raven_Feature::Health(pBot);

	if (pBot->GetTargetSys()->isTargetPresent()) {
		TargetPosition = pBot->GetTargetBot()->Pos();

		Visible = pBot->GetWorld()->isLOSOkay(Position, TargetPosition);

		Distance = Position.Distance(TargetPosition);

		Desirability = Tweaker * (1-Health) * Visible;

		return Desirability;
	}
	return 0.0;
}

//------------------------------ SetGoal --------------------------------------
void HideGoal_Evaluator::SetGoal(Raven_Bot* pBot) {
	
	pBot->GetBrain()->AddGoal_Hide();
}