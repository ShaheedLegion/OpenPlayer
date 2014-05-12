#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include <limits.h>
#include <math.h>

#include "EventListener.h"
#include "DisplayButton.h"
#include "ToggleDisplayButton.h"
#include "CommandIDs.h"
#include "DSP.h"

using namespace openplayer;

class Display : public EventListener, public IDataHandler
{
    protected:
        SDL_Surface * screen;
        SDL_Surface * visualization;
        SDL_Rect dstrect;

    public:
        DisplayButton * mediaButtons;
        int num_media_buttons;
        ToggleDisplayButton * stateButtons;
        //MultiStateButton repeatButton;
        int num_state_buttons;

    public:
        Display() : screen(0), visualization(0), num_media_buttons(6), num_state_buttons(3)
        {
            mediaButtons = new DisplayButton[num_media_buttons];
            stateButtons = new ToggleDisplayButton[num_state_buttons];
        }

        ~Display()
        {// free loaded bitmap
            SDL_FreeSurface(visualization);
        }
    public:
        void Initialize(SDL_Surface* scr)
        {
            screen = scr;
            visualization = SDL_CreateRGBSurface(SDL_SWSURFACE, 384, 200, 32, 0, 0, 0, 0);
            if (!visualization)
            {
                printf("Unable to create visualization surface: %s\n", SDL_GetError());
                return;
            }

            dstrect.x = 0;
            dstrect.y = 0;

            int bw = 64, bh = 64;
            const char * names[] = {"gui/op_previous.bmp","gui/op_play.bmp","gui/op_pause.bmp","gui/op_stop.bmp","gui/op_next.bmp","gui/op_open.bmp"};
            int commands[] = {CMD_PREV, CMD_PLAY, CMD_PAUSE, CMD_STOP, CMD_NEXT, CMD_OPEN};
            for (int i = 0; i < num_media_buttons; i++)
                mediaButtons[i].Initialize(names[i], (i * bw), screen->h - bh, bw, bh, commands[i]);

            const char * states[] = {"gui/op_rep_off.bmp", "gui/op_rep_one.bmp", "gui/op_rep_all.bmp"};
            int statecommands[] = {CMD_REP0, CMD_REP1, CMD_REPA};
            for (int i = 0; i < num_state_buttons; i++)
                stateButtons[i].Initialize(states[i], (i * bw), screen->h - (bh * 2), bw, bh, statecommands[i]);
        }

        int HandleEvent(EventData & data)
        {
            if (data.ID == SDL_MOUSEMOTION)
            {
                for (int i = 0; i < num_media_buttons; i++)
                    mediaButtons[i].HandleMouseMove(data.x, data.y);
            }
            if (data.ID == SDL_MOUSEBUTTONDOWN)
            {
                if (data.keycode == SDL_BUTTON_LEFT)
                {
                    int command = -1;
                    for (int i = 0; i < num_media_buttons; i++)
                    {
                        command = mediaButtons[i].HandleMouseDown(data.x, data.y);
                       if (command != -1) return command;
                    }
                    for (int i = 0; i < num_state_buttons; i++)
                    {
                        command = stateButtons[i].HandleMouseDown(data.x, data.y);
                        if (command != -1) return command;
                    }
                }
            }
            if (data.ID == SDL_MOUSEBUTTONUP)
            {
                if (data.keycode == SDL_BUTTON_LEFT)
                {
                    for (int i = 0; i < num_media_buttons; i++)
                        mediaButtons[i].HandleMouseUp(data.x, data.y);
                }
            }
            return -1;
        }

        void Render()
        {// clear screen
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

            if (visualization)
            {// draw bitmap ... etc
                SDL_BlitSurface(visualization, 0, screen, &dstrect);
                for (int i = 0; i < num_media_buttons; i++)
                    mediaButtons[i].Render(screen);
                for (int i = 0; i < num_state_buttons; i++)
                    stateButtons[i].Render(screen);
            }

            SDL_Flip(screen); // finally, update the screen :)
        }
        void HandleData(short * buff, int len, int chan)
        {
            //printf("HandleData - stream channel[%d] length [%d]\n", chan, len);
            //printf("HandleData called channels[%d] lowest l[%d] r[%d] highest l[%d] r[%d]\n", chan, l_lowest, r_lowest, l_highest, r_highest);

            if (visualization)
            {   //possibly clear out viz here
                SDL_FillRect(visualization, 0, SDL_MapRGB(visualization->format, 255, 64, 64));
                //perform some crazy math on viz...
                SDL_LockSurface(visualization);
                int indices = (len / 160);
                int * pixels = (int*)visualization->pixels;

                if (pixels)
                {
                    Uint32 c_white = SDL_MapRGB(visualization->format, 255, 255, 255);
                    short lval, rval;
                    double ldval, rdval;
                    int offset = visualization->h / 2;
                    int l_idx;
                    int r_idx;

                    int idxoffset = 0;
                    for (int i = 0; i < indices; i++)
                    {
                        lval = buff[idxoffset];
                        rval = buff[idxoffset + 1];

                        idxoffset += 80;

                        if (lval)
                        {
                            if (lval < 0) ldval = (double)((double)lval / (SHRT_MIN));
                            if (lval > 0) ldval = (double)((double)lval / (SHRT_MAX));
                            if (rval < 0) rdval = (double)((double)rval / (SHRT_MIN));
                            if (rval > 0) rdval = (double)((double)rval / (SHRT_MAX));

                            //printf("Clamped lval[%f]\n", ldval);

                            ldval *= 50;
                            rdval *= 50;

                            if (ldval > 50.0 || rdval > 50.0 || ldval < -50.0 || rdval < -50.0)
                                printf("Exceeded! Transformed lval [%f] rval[%f]\n", ldval, rdval);
                        }

                        int index = i * 24;
                        for (int j = 0; j < 24; j++)
                        {
                            l_idx = index + ((offset - floor(ldval)) * visualization->w);
                            r_idx = index + ((offset + floor(rdval)) * visualization->w);
                            if (l_idx > (visualization->w * visualization->h) || l_idx < 0)
                            {
                                printf("left index exceeded bounds! idx[%d] bounds[%d]\n", l_idx, (visualization->w * visualization->h));
                                continue;
                            }
                            if (r_idx > (visualization->w * visualization->h) || r_idx < 0)
                            {
                                printf("right index exceeded bounds! idx[%d] bounds[%d]\n", r_idx, (visualization->w * visualization->h));
                                continue;
                            }
                            //printf("Setting indices i[%d] l[%d] r[%d]\n", i, l_idx, r_idx);
                            pixels[l_idx] = c_white;
                            pixels[r_idx] = c_white;
                            index++;
                        }
                    }
                }
                SDL_UnlockSurface(visualization);
            }
        }
};

#endif // DISPLAY_H_INCLUDED
