#ifndef COMMANDHANDLER_H_INCLUDED
#define COMMANDHANDLER_H_INCLUDED

class CommandHandler
{
    public:
    virtual void HandleExecCommand(int command) = 0;
};

#endif // COMMANDHANDLER_H_INCLUDED
