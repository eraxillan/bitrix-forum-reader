#include "task.h"


BfrTask::BfrTask() : m_action(Action::Invalid), m_url(), m_pageNo(INVALID_PAGENO)
{
}

BfrTask::BfrTask(BfrTask::Action action, ForumThreadUrlData url) : m_action(action), m_url(url), m_pageNo(INVALID_PAGENO)
{
}

BfrTask::BfrTask(BfrTask::Action action, ForumThreadUrlData url, int pageNo) : m_action(action), m_url(url), m_pageNo(pageNo)
{
}

bool BfrTask::isValid() const
{
    // TODO: implement and use
    return (m_action != Action::Invalid);
}
