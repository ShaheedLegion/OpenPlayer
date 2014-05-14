#ifndef VOLUMEBUTTON_H_INCLUDED
#define VOLUMEBUTTON_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL/SDL.h>
#endif

class VolumeButton
{
    protected:
        SDL_Surface * buttonBackground;
        SDL_Surface * buttonSlider;
        SDL_Rect box;
        SDL_Rect slider;
        int command;

    public:
        VolumeButton()
        {

        }
        VolumeButton(const char * buttonbg, const char * buttonsl, int x, int y, int w, int h, int c )
        {
            Initialize(buttonbg, buttonsl, x, y, w, h, c);
        }

        virtual void Initialize(const char * button, const char * slidern, int x, int y, int w, int h, int c )
        {
            command = c;
            box.x = x;
            box.y = y;
            box.w = w;
            box.h = h;

            buttonBackground = SDL_LoadBMP(button);
            if (!buttonBackground)
                printf("Unable to load button sprite: %s\n", button);
            buttonSlider = SDL_LoadBMP(slidern);
            if (!buttonSlider)
                printf("Unable to load button slider sprite: %s\n", slidern);

            slider.x = (box.x + box.w) - buttonSlider->w;
            slider.y = (box.y + (box.h / 2)) - (buttonSlider->h / 2);
        }

        ~VolumeButton()
        {
            SDL_FreeSurface( buttonBackground );
            SDL_FreeSurface( buttonSlider );
        }

    public:
        virtual int HandleMouseMove(int x, int y)
        {   //do not implement drag mechanics for now.
            return -1;
        }

        virtual int HandleMouseDown(int x, int y)
        {
            if ( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {//Set the button sprite
                SetSliderPosition(x);
                return command;
            }

            return -1;
        }

        virtual int HandleMouseUp(int x, int y)
        {
            return -1;
        }

        virtual void Render(SDL_Surface * screen)
        {
            SDL_Rect offset;

            //Get offsets
            offset.x = box.x;
            offset.y = box.y;

            SDL_BlitSurface( buttonBackground, 0, screen, &offset );

            offset.x = slider.x;
            offset.y = slider.y;

            SDL_BlitSurface( buttonSlider, 0, screen, &offset );
        }

        virtual int GetValue()
        {
            double vv = (double)(slider.x - box.x);
            double vw = vv / (double)box.w;
            double vx = vw * 100.0;
            int vol = (int)vx;
            //printf("Command[%d] Volume Value vv[%f] vw[%f] vx[%f] vol[%d]\n", command, vv, vw, vx, vol);
            return vol;
        }

    protected:

        void SetSliderPosition(int offsetx)
        {
            slider.x = offsetx - (buttonSlider->w / 2);

            if (slider.x < box.x)
                slider.x = box.x;
            if (slider.x > box.x + box.w)
                slider.x = (box.x + box.w) - slider.w;
        }

};

#endif // VOLUMEBUTTON_H_INCLUDED
