#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include "EventListener.h"
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

    protected:
        std::vector<EventListener*> m_listeners;

};
#endif // EVENTS_H_INCLUDED
