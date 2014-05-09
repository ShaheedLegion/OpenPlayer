#ifndef SOUNDHANDLER_H_INCLUDED
#define SOUNDHANDLER_H_INCLUDED

#include "CommandHandler.h"
#include "CommandIDs.h"
#include "Commands.h"

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

using namespace openplayer;

class SoundHandler : public CommandHandler
{
protected:
    Mix_Music * m_file;
public:
    SoundHandler(Commands * c)
    {
        c->SetCommandHandler(CMD_PLAY, this);
        c->SetCommandHandler(CMD_PAUSE, this);
        c->SetCommandHandler(CMD_STOP, this);
        c->SetCommandHandler(CMD_PREV, this);
        c->SetCommandHandler(CMD_NEXT, this);
        c->SetCommandHandler(CMD_OPEN, this);
        c->SetCommandHandler(CMD_REP0, this);
        c->SetCommandHandler(CMD_REP1, this);
        c->SetCommandHandler(CMD_REPA, this);
        c->SetCommandHandler(CMD_SHUFF0, this);
        c->SetCommandHandler(CMD_SHUFFA, this);

        Mix_OpenAudio(22050,AUDIO_S16SYS,2,640);
        m_file = Mix_LoadMUS("test.mp3");
        Mix_PlayMusic(m_file,0);
    }
    ~SoundHandler(){}

public:
    void HandleExecCommand(int command)
    {
        switch (command)
        {
            case CMD_PLAY:
                PlayFile();
            break;
            case CMD_PAUSE:
            break;
            case CMD_STOP:
            break;
            case CMD_PREV:
            break;
            case CMD_NEXT:
            break;
            case CMD_OPEN:
                OpenFile();
            break;
            case CMD_REP0:
            break;
            case CMD_REP1:
            break;
            case CMD_REPA:
            break;
            case CMD_SHUFF0:
            break;
            case CMD_SHUFFA:
            break;
            default:
            break;
        }
    }

protected:
    void OpenFile()
    {
    }
    void PlayFile()
    {
    }
};

#endif // SOUNDHANDLER_H_INCLUDED
