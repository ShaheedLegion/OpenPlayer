#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

/*
This class will handle all commands which are generated by th GUI, so we will end up with a command map which
contains all the possible command identifiers for the gui.
*/
#include <map>
using namespace std;

class CommandHandler
{
    public:
    virtual void HandleExecCommand(int command);
};

class Commands
{
protected:
    map<int/*command*/, CommandHandler*> m_handlers;
public:
    Commands(){}
    ~Commands(){}

    void HandleCommand(int c)
    {
        map<int, CommandHandler*>::iterator i = m_handlers.find(c);
        if (i != m_handlers.end())
            i->second->HandleExecCommand(c);
    }

    void SetCommandHandler(int c, CommandHandler * h)
    {
        m_handlers[c] = h;  //we overwrite the handler for this command - there van be only one.
    }
};

#endif // COMMANDS_H_INCLUDED