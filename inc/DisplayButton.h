#ifndef DISPLAYBUTTON_H_INCLUDED
#define DISPLAYBUTTON_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

class DisplayButton
{
    protected:
        int CLIP_MOUSEOVER;
        int CLIP_MOUSEOUT;
        int CLIP_MOUSEDOWN;
        int CLIP_MOUSEUP;
        SDL_Surface * buttonSheet;
        SDL_Rect clips[ 4 ];
        SDL_Rect box;
        SDL_Rect clip;
        int command;

    public:
        DisplayButton()
        {

        }
        DisplayButton(const char * filename, int x, int y, int w, int h, int c )
        {
            Initialize(filename, x, y, w, h, c);
        }

        virtual void Initialize(const char * filename, int x, int y, int w, int h, int c )
        {
            command = c;
            CLIP_MOUSEOVER = 0;
            CLIP_MOUSEOUT = 1;
            CLIP_MOUSEDOWN = 2;
            CLIP_MOUSEUP = 3;
            box.x = x;
            box.y = y;
            box.w = w;
            box.h = h;

            clips[ CLIP_MOUSEOVER ].x = 0;
            clips[ CLIP_MOUSEOVER ].y = 0;
            clips[ CLIP_MOUSEOVER ].w = w;
            clips[ CLIP_MOUSEOVER ].h = h;

            clips[ CLIP_MOUSEOUT ].x = w;
            clips[ CLIP_MOUSEOUT ].y = 0;
            clips[ CLIP_MOUSEOUT ].w = w;
            clips[ CLIP_MOUSEOUT ].h = h;

            clips[ CLIP_MOUSEDOWN ].x = 0;
            clips[ CLIP_MOUSEDOWN ].y = h;
            clips[ CLIP_MOUSEDOWN ].w = w;
            clips[ CLIP_MOUSEDOWN ].h = h;

            clips[ CLIP_MOUSEUP ].x = w;
            clips[ CLIP_MOUSEUP ].y = h;
            clips[ CLIP_MOUSEUP ].w = w;
            clips[ CLIP_MOUSEUP ].h = h;

            //Set the default sprite
            CopyClip(CLIP_MOUSEOUT);

            buttonSheet = SDL_LoadBMP(filename);
            if (!buttonSheet)
                printf("Unable to load button sprite: %s\n", filename);
        }

        ~DisplayButton()
        {
            SDL_FreeSurface( buttonSheet );
        }

    public:
        virtual int HandleMouseMove(int x, int y)
        {
            if ( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {//Set the button sprite
                CopyClip(CLIP_MOUSEOVER);
                return -1;
            }
            CopyClip(CLIP_MOUSEOUT);
            return -1;
        }

        virtual int HandleMouseDown(int x, int y)
        {
            if ( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {//Set the button sprite
                CopyClip(CLIP_MOUSEDOWN);
                return command;
            }

            return -1;
        }

        virtual int HandleMouseUp(int x, int y)
        {
            if ( ( x > this->box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {//Set the button sprite
                CopyClip(CLIP_MOUSEUP);
                return -1;
            }
            return -1;
        }

        virtual void Render(SDL_Surface * screen)
        {
            SDL_Rect * clp = &(clip);
            SDL_Rect offset;

            //Get offsets
            offset.x = box.x;
            offset.y = box.y;

            SDL_BlitSurface( buttonSheet, clp, screen, &offset );
        }

    protected:

        void CopyClip(int offset)
        {
            //printf("CopyClip offset:%d\n", offset);
            if (offset >= 0 && offset < 4)
            {   //not the correct fix - just stop te program from crashing so I can debug it.
                clip.x = clips[ offset ].x;
                clip.y = clips[ offset ].y;
                clip.w = clips[ offset ].w;
                clip.h = clips[ offset ].h;
            }
        }

};

#endif // DISPLAYBUTTON_H_INCLUDED
