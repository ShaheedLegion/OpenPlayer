#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include "EventListener.h"
#include "Commands.h"
#include <vector>

class Events
{
    public:
        Events();
        ~Events();
    public:
        void AddEventListener(EventListener * listener);
        void RemoveEventListener(EventListener * listener);
        void PublishEventData(EventData & data);
        void SetCommandHandler(Commands * c);
    protected:
        std::vector<EventListener*> m_listeners;
        Commands * m_commands;

};
#endif // EVENTS_H_INCLUDED
