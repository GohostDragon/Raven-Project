#include "Raven_TargetingSystem.h"
#include "Raven_Bot.h"
#include "Raven_SensoryMemory.h"



//-------------------------------- ctor ---------------------------------------
//-----------------------------------------------------------------------------
Raven_TargetingSystem::Raven_TargetingSystem(Raven_Bot* owner):m_pOwner(owner),
                                                               m_pCurrentTarget(0)
{}



//----------------------------- Update ----------------------------------------

//-----------------------------------------------------------------------------
void Raven_TargetingSystem::Update()
{
  double ClosestDistSoFar = MaxDouble;
  m_pCurrentTarget       = 0;

  int minHelath = 100;
  //grab a list of all the opponents the owner can sense
  std::list<Raven_Bot*> SensedBots;
  SensedBots = m_pOwner->GetSensoryMem()->GetListOfRecentlySensedOpponents();
  
  std::list<Raven_Bot*>::const_iterator curBot = SensedBots.begin();
  for (curBot; curBot != SensedBots.end(); ++curBot)
  {
    //make sure the bot is alive and that it is not the owner
    if ((*curBot)->isAlive() && (*curBot != m_pOwner) )
    {
        //내가(봇이) 타격한 봇들 중에 체력이 가장 적은 봇을 타격한다.
        int health = m_pOwner->GetSensoryMem()->GetTargetBotHealth(*curBot);
        if (health < minHelath)
        {
            minHelath = health;
            m_pCurrentTarget = *curBot;
        }
    }
  }

  //그런 봇이 없다면 거리가 가까운 순으로 타격해라
  if (m_pCurrentTarget == 0)
  {
      curBot = SensedBots.begin();
      for (curBot; curBot != SensedBots.end(); ++curBot)
      {
          //make sure the bot is alive and that it is not the owner
          if ((*curBot)->isAlive() && (*curBot != m_pOwner))
          {
              double dist = Vec2DDistanceSq((*curBot)->Pos(), m_pOwner->Pos());

              if (dist < ClosestDistSoFar)
              {
                ClosestDistSoFar = dist;
                m_pCurrentTarget = *curBot;
              }
          }
      }
  }
}




bool Raven_TargetingSystem::isTargetWithinFOV()const
{
  return m_pOwner->GetSensoryMem()->isOpponentWithinFOV(m_pCurrentTarget);
}

bool Raven_TargetingSystem::isTargetShootable()const
{
  return m_pOwner->GetSensoryMem()->isOpponentShootable(m_pCurrentTarget);
}

Vector2D Raven_TargetingSystem::GetLastRecordedPosition()const
{
  return m_pOwner->GetSensoryMem()->GetLastRecordedPositionOfOpponent(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenVisible()const
{
  return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenVisible(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenOutOfView()const
{
  return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenOutOfView(m_pCurrentTarget);
}
