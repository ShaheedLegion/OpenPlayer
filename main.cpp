#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "inc/Events.h"
#include "inc/Display.h"
#include "inc/Commands.h"

namespace openplayer
{
    Commands * g_commands;
    Events * g_events;
    Display * g_display;

    void CreateSubsystems()
    {
        //Declare all classes etc. here.
        g_commands = new Commands();
        g_events = new Events();
        g_display = new Display();

        g_events->SetCommandHandler(g_commands);
        g_events->AddEventListener(g_display);
    }

    void DestroySubsystems()
    {
        delete g_events;
        delete g_display;
        delete g_commands;
    }
}

int main ( int argc, char** argv )
{
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )    // initialize SDL video
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    atexit(SDL_Quit);    // make sure SDL cleans up before exit
    SDL_WM_SetCaption("OpenPlayer", "OpenPlayer");
    openplayer::CreateSubsystems();
    SDL_Surface * screen = SDL_SetVideoMode(384, 480, 32, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_NOFRAME);
    if ( !screen )
    {
        printf("Unable to set 384x480 video: %s\n", SDL_GetError());
        return 1;
    }

    openplayer::g_display->Initialize(screen);

    bool done = false;
    while (!done)    // program main loop
    {// message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {// check for messages
            switch (event.type)
            {// exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

            case SDL_KEYDOWN:// check for keypresses
                {// exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    else
                    {
                        EventData data(SDL_KEYDOWN, event.key.keysym.sym, 0, 0);
                        openplayer::g_events->PublishEventData(data);
                    }

                    break;
                }
            case SDL_MOUSEMOTION:
                {
                    EventData data(SDL_MOUSEMOTION, event.button.button, event.motion.x, event.motion.y);
                    openplayer::g_events->PublishEventData(data);
                    break;
                }
            case SDL_MOUSEBUTTONDOWN :
                {
                    //printf("Mouse down x:%d y:%d\n", event.motion.x, event.motion.y);
                    EventData data(SDL_MOUSEBUTTONDOWN, event.button.button, event.button.x, event.button.y);
                    openplayer::g_events->PublishEventData(data);
                    break;
                }
                case SDL_MOUSEBUTTONUP :
                {
                    //printf("Mouse up x:%d y:%d\n", event.motion.x, event.motion.y);
                    EventData data(SDL_MOUSEBUTTONUP, event.button.button, event.button.x, event.button.y);
                    openplayer::g_events->PublishEventData(data);
                    break;
                }
            } // end switch
        } // end of message processing
       openplayer::g_display->Render();
    } // end main loop

    openplayer::DestroySubsystems();
    printf("Exited cleanly\n");    // all is well ;)

    return 0;
}
