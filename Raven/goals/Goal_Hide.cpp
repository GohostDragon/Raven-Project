#include "Goal_Hide.h"
#include "../Raven/goals/Goal_MoveToPosition.h"
#include "../Raven/navigation/Raven_PathPlanner.h"
#include "../Raven/goals/Goal_Wander.h"
#include "../Raven/goals/Goal_FollowPath.h"
#include "../Raven/Raven_bot.h"
#include "../Raven/Raven_Game.h"
#include "../Raven/Raven_Map.h"
#include"../Common/Graph/SparseGraph.h"
#include <vector>
#include "../Common/Debug/DebugConsole.h"
#include "../Common/Messaging/Telegram.h"
#include "../Raven/Raven_Messages.h"

//------------------------------ Activate -------------------------------------
//-----------------------------------------------------------------------------
void Goal_Hide::Activate() {
	m_iStatus = active;

	RemoveAllSubgoals();

	if (!m_pOwner->GetTargetSys()->isTargetPresent() || !m_pOwner->GetTargetSys()->isTargetShootable()) {
		m_iStatus = completed;
		return;
	}
	if (m_pOwner->GetWorld()->
		isLOSOkay(m_pOwner->Pos(), m_pOwner->GetTargetBot()->Pos())) {
		Vector2D hidePoint = FindAPointToHide(m_pOwner->GetTargetBot());

		m_pOwner->GetPathPlanner()->RequestPathToPosition(hidePoint);

		AddSubgoal(new Goal_Wander(m_pOwner));

		//AddSubgoal(new Goal_MoveToPosition(m_pOwner, hidePoint));
	}
	
	
}

//------------------------------ Process -------------------------------------
//-----------------------------------------------------------------------------
int Goal_Hide::Process() {
	ActivateIfInactive();
	if (m_pOwner->GetTargetSys()->isTargetPresent()) {
		if (!m_pOwner->GetWorld()->isSecondVisibleToFirst(m_pOwner, m_pOwner->GetTargetBot()))
			Terminate();
		else m_iStatus = ProcessSubgoals();
	}
	else Terminate();

	return m_iStatus;
}

bool Goal_Hide::HandleMessage(const Telegram& msg) {
	bool bHandled = ForwardMessageToFrontMostSubgoal(msg);

	if (bHandled == false) {
		switch (msg.Msg) {
		case Msg_PathReady:
			RemoveAllSubgoals();
			AddSubgoal(new Goal_FollowPath(m_pOwner, m_pOwner->GetPathPlanner()->GetPath()));
			return true;
		case Msg_NoPathAvailable:
			m_iStatus = failed;
			return true;
		default: return false;
		}
	}
	return true;
}

Vector2D Goal_Hide::FindAPointToHide(Raven_Bot* pTarget) {
	Vector2D OwnerPos = m_pOwner->Pos();
	Vector2D TrgPos = pTarget->Pos();
	std::vector<Vector2D> hidablePoints;

	Vector2D diff = TrgPos - OwnerPos;
	Raven_Map::NavGraph WorldGraph = m_pOwner->GetWorld()->GetMap()->GetNavGraph();
	for (int i = 0; i < WorldGraph.NumActiveNodes(); ++i) {
		if (!(m_pOwner->GetWorld()->isLOSOkay(m_pOwner->Pos(),
			WorldGraph.GetNode(i).Pos()))) {
			hidablePoints.push_back(WorldGraph.GetNode(i).Pos());
		}
	}
	Vector2D nearestPoint{ 1000,1000 };
	for (auto& p : hidablePoints) {
		if (nearestPoint.Distance(OwnerPos) > p.Distance(OwnerPos))
			nearestPoint = p;
	}

	if (nearestPoint == Vector2D(1000, 1000))
		return Vector2D(-1, -1);

	return nearestPoint;
}