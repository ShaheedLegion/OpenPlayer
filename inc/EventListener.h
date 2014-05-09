#ifndef EVENTLISTENER_H_INCLUDED
#define EVENTLISTENER_H_INCLUDED

typedef struct tagEventData
{
    int ID; //event ID
    int keycode;    //key code (keyboard key code or mouse button code)
    int x;  //x coordinate - should be in window coordinates
    int y;  //y coordinate - should be in window coordinates

    tagEventData(int id, int kc, int xp, int yp) : ID(id), keycode(kc), x(xp), y(yp)
    {

    }

} EventData;

class EventListener
{
    public:
    virtual void HandleEvent(EventData & data) = 0;
};

#endif // EVENTLISTENER_H_INCLUDED
