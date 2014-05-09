#include "inc/Events.h"

using namespace std;
typedef vector<EventListener*>::iterator ev_iter;

Events::Events() : m_commands(0)
{
}
Events::~Events()
{
}
void Events::AddEventListener(EventListener * listener)
{
    m_listeners.push_back(listener);
}
void Events::RemoveEventListener(EventListener * listener)
{
	for (ev_iter i = m_listeners.begin(); i != m_listeners.end(); i++)
	{
		if (*i == listener)
		{	//remove item from vector, but do not erase the memory which the item points to.
			i = m_listeners.erase(i);
			break;
		}
	}
}
void Events::SetCommandHandler(Commands * c)
{
    m_commands = c;
}
void Events::PublishEventData(EventData & data)
{
	for (ev_iter i = m_listeners.begin(); i != m_listeners.end(); i++)
    {
		int command = (*i)->HandleEvent(data);
		if (m_commands)
            m_commands->HandleCommand(command);
	}
}
