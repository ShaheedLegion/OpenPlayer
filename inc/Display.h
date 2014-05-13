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
#include "VolumeButton.h"
#include "CommandIDs.h"
#include "DSP.h"

using namespace openplayer;

class Display : public EventListener, public IDataHandler
{
    protected:
        SDL_Surface * screen;
        SDL_Surface * visualization;
        SDL_Rect dstrect;
        short m_data[32];

        struct tagColor
        {
            public:
            int r;
            int g;
            int b;
            int rd;
            int gd;
            int bd;

            tagColor(int _r, int _g, int _b, int _rd, int _gd, int _bd) : r(_r), g(_g), b(_b), rd(_rd), gd(_gd), bd(_bd)
            {

            }
            void increment()
            {
                r += rd;
                g += gd;
                b += bd;
            }
        };

    public:
        DisplayButton * mediaButtons;
        int num_media_buttons;
        ToggleDisplayButton * stateButtons;
        VolumeButton * volume;
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

            int bw = 32, bh = 32;
            const char * names[] = {"gui/op_previous.bmp","gui/op_play.bmp","gui/op_pause.bmp","gui/op_stop.bmp","gui/op_next.bmp","gui/op_open.bmp"};
            int commands[] = {CMD_PREV, CMD_PLAY, CMD_PAUSE, CMD_STOP, CMD_NEXT, CMD_OPEN};
            for (int i = 0; i < num_media_buttons; i++)
                mediaButtons[i].Initialize(names[i], (i * bw), screen->h - bh, bw, bh, commands[i]);

            volume = new VolumeButton("gui/op_vol_bg.bmp", "gui/op_vol_sl.bmp", screen->w / 2, screen->h - bh, screen->w / 2, bh, CMD_VOL);

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
                        if (command != -1)
                        {
                            //process the logic for only allowing one state at a time.
                            for (int j = 0; j < num_state_buttons; j++)
                                stateButtons[j].ToggleButton(false);
                            stateButtons[i].ToggleButton(true);
                            return command;
                        }
                    }
                    command = volume->HandleMouseDown(data.x, data.y);
                    data.value = volume->GetValue();
                    if (command != -1) return command;
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
                RenderVisualization();
                SDL_BlitSurface(visualization, 0, screen, &dstrect);
                for (int i = 0; i < num_media_buttons; i++)
                    mediaButtons[i].Render(screen);
                for (int i = 0; i < num_state_buttons; i++)
                    stateButtons[i].Render(screen);
                volume->Render(screen);
            }

            SDL_Flip(screen); // finally, update the screen :)
        }
        void VLine(int * pixels, int x0, int y0, int y1, int sw, Uint32 color)
        {
            int idx = 0;
            for (int i = 0; i < (y1 - y0); i++)
            {
                idx = x0 + ((y0 + i) * sw);
                pixels[idx] = color;
            }

        }
        void VGLine(int * pixels, int x0, int y0, int y1, int sw, tagColor & col, SDL_Surface * surf)
        {
            int idx = 0;
            for (int i = 0; i < (y1 - y0); i++)
            {
                col.increment();
                idx = x0 + ((y0 + i) * sw);
                pixels[idx] = SDL_MapRGB(surf->format, col.r, col.g, col.b);
            }

        }
        void HLine(int * pixels, int x0, int x1, int y, int sw, Uint32 color)
        {
            int idx = x0 + (y * sw);
            for (int i = 0; i < (x1 - x0); i++)
            {
                pixels[idx + i] = color;
            }
        }
        void HandleData(short * buff, int len, int chan)
        {
            int indices = (len / 160);
            int idxoffset = 0;
            int internaloffset = 0;
            for (int i = 0; i < indices; i++)
            {
                m_data[internaloffset + 0] = buff[idxoffset + 0];
                m_data[internaloffset + 1] = buff[idxoffset + 40];

                idxoffset += 80;
                internaloffset += 2;
            }
        }
        void RenderVisualization()
        {
            if (visualization)
            {   //possibly clear out viz here
                SDL_FillRect(visualization, 0, SDL_MapRGB(visualization->format, 255, 64, 64));
                SDL_LockSurface(visualization);

                if (visualization->pixels)
                {
                    Uint32 c_gray = SDL_MapRGB(visualization->format, 128, 128, 128);
                    short lval, rval;
                    double ldval, rdval;
                    int offset = visualization->h / 2;

                    int idxoffset = 0;
                    for (int i = 0; i < 16; i++)
                    {
                        lval = m_data[idxoffset];
                        rval = m_data[idxoffset + 1];
                        idxoffset += 2;

                        if (lval)
                        {
                            ldval = (lval < 0? (double)((double)lval / (SHRT_MIN)) : (double)((double)lval / (SHRT_MAX)));
                            rdval = (rval < 0? (double)((double)rval / (SHRT_MIN)) : (double)((double)rval / (SHRT_MAX)));
                            ldval *= 50;
                            rdval *= 50;
                        }

                        int index = i * 24;
                        for (int j = 0; j < 24; j++)
                        {
                            if ((ldval < 0) || (ldval > 50.0))
                                continue;
                            if ((rdval < 0) || (rdval > 50.0))
                                continue;

                            tagColor ggrad(0, 255, 0, 0, -3, 0);
                            VGLine((int*)visualization->pixels, index, offset - floor(ldval), offset, visualization->w, ggrad, visualization);
                            tagColor bgrad(0, 0, 105, 0, 0, 3);
                            VGLine((int*)visualization->pixels, index, offset, offset + floor(rdval), visualization->w, bgrad, visualization);
                            HLine((int*)visualization->pixels, 0, visualization->w, offset, visualization->w, c_gray);
                            index++;
                        }
                    }
                }
                SDL_UnlockSurface(visualization);
            }
        }
};

#endif // DISPLAY_H_INCLUDED
