#ifndef SOUNDHANDLER_H_INCLUDED
#define SOUNDHANDLER_H_INCLUDED

#include "CommandHandler.h"
#include "CommandIDs.h"
#include "Commands.h"
#include "DSP.h"

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <string>

#include <Windows.h>
#include <Commdlg.h>
#include <iostream>

using namespace openplayer;
using namespace std;
class SoundHandler;

namespace sounds
{
    SoundHandler * g_shandler = 0;
}


class SoundHandler : public CommandHandler
{
protected:
    Mix_Music * m_file;
    string m_filename;
    DSP * m_dsp;
public:
    SoundHandler(Commands * c) : m_file(0), m_dsp(0)
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
        c->SetCommandHandler(CMD_VOL, this);

        Mix_OpenAudio(22050,AUDIO_S16SYS,2,640);
        sounds::g_shandler = this;

        m_dsp = new DSP();
        if (!Mix_RegisterEffect(MIX_CHANNEL_POST, DSP::ProcessAudio, NULL, NULL))
            printf("Mix_RegisterEffect: %s\n", Mix_GetError());
        //Mix_Volume(1,MIX_MAX_VOLUME/2);   //set music volume
    }
    ~SoundHandler()
    {
        CompleteMusic();
        Mix_CloseAudio();
        delete m_dsp;
    }

public:
    static void CompleteMusic()
    {
        if (sounds::g_shandler)
        {
            Mix_HaltMusic();
            Mix_FreeMusic(sounds::g_shandler->m_file);
            sounds::g_shandler->m_file = 0;
            printf("Completed song: %s\n", sounds::g_shandler->m_filename.c_str());
        }
    }
    void HandleExecCommand(int command, int value)
    {
        printf("Handling command code[%d] value[%d]\n", command, value);
        switch (command)
        {
            case CMD_PLAY:
                PlayFile();
            break;
            case CMD_PAUSE:
                Mix_PauseMusic();
            break;
            case CMD_STOP:
                CompleteMusic();
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
            case CMD_VOL:
                {
                    int volume = (int)((double)((double)value / 100.0) * MIX_MAX_VOLUME);
                    Mix_Volume(-1, volume);
                    Mix_VolumeMusic(volume);
                    printf("Setting volume: %d\n", volume);
                }
            default:
            break;
        }
    }

    void SetHandler(IDataHandler * h)
    {
        if (m_dsp)
            m_dsp->handler = h;
    }
protected:
    void OpenFile()
    {
        OPENFILENAME ofn;       // common dialog box structure
        char szFile[260];       // buffer for file name

        ZeroMemory(&ofn, sizeof(ofn));        // Initialize OPENFILENAME
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
        // use the contents of szFile to initialize itself.
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "MP3\0*.mp3\0Ogg\0*.ogg\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Display the Open dialog box.
        if (GetOpenFileName(&ofn)==TRUE)
        {
            m_filename.assign(ofn.lpstrFile);
            cout << "Got file name: " << m_filename << endl;
        }

    }
    void PlayFile()
    {
        if (m_file) //if the file is still playing, then stop it and free the resource.
            CompleteMusic();
        if (m_filename.size())
        {
            cout << "Trying to play file: " << m_filename << endl;
            m_file = Mix_LoadMUS(m_filename.c_str());
            Mix_PlayMusic(m_file,0);
            Mix_HookMusicFinished(SoundHandler::CompleteMusic);
        }
    }
};

#endif // SOUNDHANDLER_H_INCLUDED
