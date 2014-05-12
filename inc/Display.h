#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include <limits.h>

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

        //debugging
        //short l_lowest, r_lowest;
        //short l_highest, r_highest;
    public:
        Display() : screen(0), visualization(0), num_media_buttons(6), num_state_buttons(3)
        {
            mediaButtons = new DisplayButton[num_media_buttons];
            stateButtons = new ToggleDisplayButton[num_state_buttons];

            //l_lowest = 0;
            //r_lowest = 0;
            //l_highest = 0;
            //r_highest = 0;
        }

        ~Display()
        {
            // free loaded bitmap
            SDL_FreeSurface(visualization);
        }
    public:
        void Initialize(SDL_Surface* scr)
        {
            screen = scr;
            // load an image
            visualization = SDL_CreateRGBSurface(SDL_SWSURFACE, 100, 100, 32, 0, 0, 0, 0);//SDL_LoadBMP("gui/cb.bmp");
            if (!visualization)
            {
                printf("Unable to create visualization surface: %s\n", SDL_GetError());
                return;
            }

            // centre the bitmap on screen
            dstrect.x = (screen->w - visualization->w) / 2;
            dstrect.y = 0;//(screen->h - bmp->h) / 2;

            int bw = 64, bh = 64;
            const char * names[] = {"gui/op_previous.bmp","gui/op_play.bmp","gui/op_pause.bmp","gui/op_stop.bmp","gui/op_next.bmp","gui/op_open.bmp"};
            int commands[] = {CMD_PREV, CMD_PLAY, CMD_PAUSE, CMD_STOP, CMD_NEXT, CMD_OPEN};
            for (int i = 0; i < num_media_buttons; i++)
                mediaButtons[i].Initialize(names[i], (i * bw), screen->h - bh, bw, bh, commands[i]);

            const char * states[] = {"gui/op_rep_off.bmp", "gui/op_rep_one.bmp", "gui/op_rep_all.bmp"};
            int statecommands[] = {CMD_REP0, CMD_REP1, CMD_REPA};
            for (int i = 0; i < num_state_buttons; i++)
                //repeatButton.AddState(states[i], 0, screen->h - (bh * 2), bw, bh);
                stateButtons[i].Initialize(states[i], (i * bw), screen->h - (bh * 2), bw, bh, statecommands[i]);
        }

        int HandleEvent(EventData & data)
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
                //repeatButton.Render(screen);
            }

            SDL_Flip(screen); // finally, update the screen :)
        }
        void HandleData(short * buff, int len, int chan)
        {
            //short l_lowest = 0, r_lowest = 0;
            //short l_highest = 0, r_highest = 0;
/*
            short l;
            short r;
            for (int i = 0; i < len; i+=2)
            {
                l = buff[i];
                r = buff[i + 1];

                if (l < l_lowest) l_lowest = l;
                if (r < r_lowest) r_lowest = r;

                if (r > r_highest) r_highest = r;
                if (l > l_highest) l_highest = l;
            }
*/
            //now copy the data to the screen using some algorithm...

            //printf("HandleData called channels[%d] lowest l[%d] r[%d] highest l[%d] r[%d]\n", chan, l_lowest, r_lowest, l_highest, r_highest);

            if (visualization)
            {   //possibly clear out viz here
                SDL_FillRect(visualization, 0, SDL_MapRGB(visualization->format, 0, 0, 0));
                //perform some crazy math on viz...
                SDL_LockSurface(visualization);
                //we will assume that len is the same as viz width, but do some math here just in case.
                int indices = len;//(len < visualization->w ? len : visualization->w);
                int * pixels = (int*)visualization->pixels;

                if (pixels)
                {
                    Uint32 c_white = SDL_MapRGB(visualization->format, 255,255,255);
                    //int combinedvalue;
                    short lval;
                    double ldval;
                    //short rval;
                    int offset = visualization->h / 2;
                    int l_idx;
                    int r_idx;
                    for (int i = 0; i < indices; i+=2)
                    {
                        lval = buff[i];//(32000 / buff[i]) * 50;
                        //rval = buff[i + 1];//(32000 / buff[i + 1]) * 50;

                        if (lval)
                        {
                            if (lval < 0) ldval = (double)((double)lval / (-32768));
                            if (lval > 0) ldval = (double)((double)lval / (32768));

                            //printf("Clamped lval[%f]\n", ldval);

                            ldval *= 50;

                            if (ldval > 50.0)
                                printf("Exceeded! Transfored lval [%f]\n", ldval);
                        }
                        //lval = (32000 / (lval + 255)) * 50;
                        //rval = (32000 / (rval + 255)) * 50;
                        //printf("Initial data values l[%d] r[%d]\n", lval, rval);
                        //if (lval > 50)lval = 50;
                        //if (rval > 50)rval = 50;
                        //if (lval < 0)lval = 0;
                        //if (rval < 0)rval = 0;
                        //combinedvalue = (int)((int)lval << 16 | rval);

                        l_idx = i + (offset - (int)(ldval)) * visualization->w;
                        r_idx = i + (offset + (int)(ldval)) * visualization->w;
                        //printf("Transformed data values l[%d] r[%d]\n", lval, rval);
                        //printf("Setting indices i[%d] l[%d] r[%d]\n", i, l_idx, r_idx);
                        pixels[l_idx] = c_white;
                        pixels[r_idx] = c_white;
                        //Draw_VLine(visualization, i, offset - lval, offset + rval, c_white);
                    }
                }
                SDL_UnlockSurface(visualization);
            }

        }
};

#endif // DISPLAY_H_INCLUDED
