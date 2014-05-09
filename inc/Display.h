#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "EventListener.h"
#include "DisplayButton.h"
#include "ToggleDisplayButton.h"

class Display : public EventListener
{
    protected:
        SDL_Surface * screen;
        SDL_Surface * bmp;
        SDL_Rect dstrect;
    public:
        DisplayButton * mediaButtons;
        int num_media_buttons;
        ToggleDisplayButton * stateButtons;
        //MultiStateButton repeatButton;
        int num_state_buttons;
    public:
        Display() : screen(0), bmp(0), num_media_buttons(6), num_state_buttons(3)
        {
            mediaButtons = new DisplayButton[num_media_buttons];
            stateButtons = new ToggleDisplayButton[num_state_buttons];
        }

        ~Display()
        {
            // free loaded bitmap
            SDL_FreeSurface(bmp);
        }
    public:
        void Initialize(SDL_Surface* scr)
        {
            screen = scr;
            // load an image
            bmp = SDL_LoadBMP("gui/cb.bmp");
            if (!bmp)
            {
                printf("Unable to load bitmap: %s\n", SDL_GetError());
                return;
            }

            // centre the bitmap on screen
            dstrect.x = (screen->w - bmp->w) / 2;
            dstrect.y = 0;//(screen->h - bmp->h) / 2;

            int bw = 64, bh = 64;
            const char * names[] = {"gui/op_previous.bmp","gui/op_play.bmp","gui/op_pause.bmp","gui/op_stop.bmp","gui/op_next.bmp","gui/op_open.bmp"};
            for (int i = 0; i < num_media_buttons; i++)
                mediaButtons[i].Initialize(names[i], (i * bw), screen->h - bh, bw, bh);

            const char * states[] = {"gui/op_rep_off.bmp", "gui/op_rep_one.bmp", "gui/op_rep_all.bmp"};
            for (int i = 0; i < num_state_buttons; i++)
                //repeatButton.AddState(states[i], 0, screen->h - (bh * 2), bw, bh);
                stateButtons[i].Initialize(states[i], (i * bw), screen->h - (bh * 2), bw, bh);
        }

        void HandleEvent(EventData & data)
        {
            //printf("Handling display event id[%d] code[%d] x[%d] y[%d]\n", data->ID, data->keycode, data->x, data->y);
            if (data.ID == SDL_MOUSEMOTION)
            {
                for (int i = 0; i < num_media_buttons; i++)
                    mediaButtons[i].HandleMouseMove(data.x, data.y);
                //for (int i = 0; i < num_state_buttons; i++)
                //    stateButtons[i]->HandleMouseMove(data->x, data->y);
            }
            if (data.ID == SDL_MOUSEBUTTONDOWN)
            {
                if (data.keycode == SDL_BUTTON_LEFT)
                {
                    for (int i = 0; i < num_media_buttons; i++)
                        mediaButtons[i].HandleMouseDown(data.x, data.y);
                    for (int i = 0; i < num_state_buttons; i++)
                        stateButtons[i].HandleMouseDown(data.x, data.y);
                    //repeatButton.HandleMouseDown(data.x, data.y);
                }
            }
            if (data.ID == SDL_MOUSEBUTTONUP)
            {
                if (data.keycode == SDL_BUTTON_LEFT)
                {
                    for (int i = 0; i < num_media_buttons; i++)
                        mediaButtons[i].HandleMouseUp(data.x, data.y);
                    //for (int i = 0; i < num_state_buttons; i++)
                    //    stateButtons[i]->HandleMouseUp(data->x, data->y);
                }
            }
        }

        void Render()
        {// clear screen
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

            if (bmp)
            {// draw bitmap ... etc
                SDL_BlitSurface(bmp, 0, screen, &dstrect);
                for (int i = 0; i < num_media_buttons; i++)
                    mediaButtons[i].Render(screen);
                for (int i = 0; i < num_state_buttons; i++)
                    stateButtons[i].Render(screen);
                //repeatButton.Render(screen);
            }

            SDL_Flip(screen); // finally, update the screen :)
        }
};

#endif // DISPLAY_H_INCLUDED
