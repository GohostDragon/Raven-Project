#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   PathManager.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:   a template class to manage a number of graph searches, and to 
//          distribute the calculation of each search over several update-steps
//-----------------------------------------------------------------------------
#include <list>
#include <cassert>



template <class path_planner>
class PathManager
{
private:

    //a container of all the active search requests
    std::list<path_planner*>  m_SearchRequests; // 길찾기 결과를 담기 위한 컨테이너

    //새로 추가
    std::list<path_planner*>  Center_SearchRequests;

    //this is the total number of search cycles allocated to the manager. 
    //Each update-step these are divided equally amongst all registered path
    //requests
    unsigned int              m_iNumSearchCyclesPerUpdate;

public:

    PathManager(unsigned int NumCyclesPerUpdate) :m_iNumSearchCyclesPerUpdate(NumCyclesPerUpdate) {}

    //every time this is called the total amount of search cycles available will
    //be shared out equally between all the active path requests. If a search
    //completes successfully or fails the method will notify the relevant bot
    void UpdateSearches();

    //a path planner should call this method to register a search with the 
    //manager. (The method checks to ensure the path planner is only registered
    //once)
    void Register(path_planner* pPathPlanner);

    void UnRegister(path_planner* pPathPlanner);

    //returns the amount of path requests currently active.
    int  GetNumActiveSearches()const { return m_SearchRequests.size(); }
};

///////////////////////////////////////////////////////////////////////////////
//------------------------- UpdateSearches ------------------------------------
//
//  This method iterates through all the active path planning requests 
//  updating their searches until the user specified total number of search
//  cycles has been satisfied.
//
//  If a path is found or the search is unsuccessful the relevant agent is
//  notified accordingly by Telegram
//-----------------------------------------------------------------------------

// 이 방법은 사용자가 지정한 총 검색 주기 수가 충족될 때까지 검색을 업데이트하는 모든 활성 경로 계획 요청을 반복한다.
// 경로가 발견되거나 검색에 실패한 경우, 관련 에이전트는 텔레그램에 의해 그에 따라 통지된다

template <class path_planner>
inline void PathManager<path_planner>::UpdateSearches()
{
    int NumCyclesRemaining = m_iNumSearchCyclesPerUpdate;//남은 사이클 수 = 사용자 지정 사이클수 

    //iterate through the search requests until either all requests have been
    //fulfilled or there are no search cycles remaining for this update-step.
    std::list<path_planner*>::iterator curPath = m_SearchRequests.begin();// 길찾기 저장 컨테이너의 반복자 생성
    while (NumCyclesRemaining-- && !m_SearchRequests.empty()) // 사이클수가 0이고 , 길찾기 저장 컨테이너가 비어있지 않을때까지
    {
        //사이클돌린 결과를 result에 저장한다.
        int result = (*curPath)->CycleOnce();

        // 검색이 종료된 경우 목록에서 제거
        if ((result == target_found) || (result == target_not_found))
        {
            //remove this path from the path list
            //경로에서 목록 제거 
            curPath = m_SearchRequests.erase(curPath);
        }
        else if (result == target_founding_now)
        {
            curPath = m_SearchRequests.erase(curPath);
        }
        //move on to the next
        else
        {
            ++curPath; // 길찾기 저장 ++한다.
        }

        //the iterator may now be pointing to the end of the list. If this is so,
        // it must be reset to the beginning.
        //이제 반복자가 목록의 끝을 가리킬 수 있다. 만약 그렇다면, 처음부터 다시 시작해야 한다
        if (curPath == m_SearchRequests.end())
        {
            curPath = m_SearchRequests.begin();
        }

    }//end while
}

//--------------------------- Register ----------------------------------------
//
//  this is called to register a search with the manager.
//-----------------------------------------------------------------------------
template <class path_planner>
inline void PathManager<path_planner>::Register(path_planner* pPathPlanner)
{
    //make sure the bot does not already have a current search in the queue
    if (std::find(m_SearchRequests.begin(),
        m_SearchRequests.end(),
        pPathPlanner) == m_SearchRequests.end())
    {
        //add to the list
        m_SearchRequests.push_back(pPathPlanner);
    }
}

//----------------------------- UnRegister ------------------------------------
//-----------------------------------------------------------------------------
template <class path_planner>
inline void PathManager<path_planner>::UnRegister(path_planner* pPathPlanner)
{
    m_SearchRequests.remove(pPathPlanner);

}





#endif