#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

/*
This class will handle all commands which are generated by th GUI, so we will end up with a command map which
contains all the possible command identifiers for the gui.
*/
class Commands
{
public:
    Commands(){}
    ~Commands(){}

    void HandleCommand(int c){}
};

#endif // COMMANDS_H_INCLUDED
