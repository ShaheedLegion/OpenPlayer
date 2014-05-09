#ifndef TOGGLEDISPLAYBUTTON_H_INCLUDED
#define TOGGLEDISPLAYBUTTON_H_INCLUDED

#include "DisplayButton.h"

class ToggleDisplayButton : public DisplayButton
{
    public:
        bool bToggled;

    public:
        ToggleDisplayButton() : DisplayButton(), bToggled(false)
        {

        }

        void Initialize(const char * filename, int x, int y, int w, int h, int c)
        {
            DisplayButton::Initialize(filename, x, y, w, h, c);
            clips[ CLIP_MOUSEOVER ].x = 0;
            clips[ CLIP_MOUSEOVER ].y = 0;
            clips[ CLIP_MOUSEOVER ].w = w;
            clips[ CLIP_MOUSEOVER ].h = h;

            clips[ CLIP_MOUSEOUT ].x = 0;
            clips[ CLIP_MOUSEOUT ].y = h;
            clips[ CLIP_MOUSEOUT ].w = w;
            clips[ CLIP_MOUSEOUT ].h = h;

            //Set the default sprite
            CopyClip(CLIP_MOUSEOVER);
        }
        ToggleDisplayButton(const char * filename, int x, int y, int w, int h, int c) : DisplayButton(filename, x, y, w, h, c), bToggled(false)
        {
            clips[ CLIP_MOUSEOVER ].x = 0;
            clips[ CLIP_MOUSEOVER ].y = 0;
            clips[ CLIP_MOUSEOVER ].w = w;
            clips[ CLIP_MOUSEOVER ].h = h;

            clips[ CLIP_MOUSEOUT ].x = 0;
            clips[ CLIP_MOUSEOUT ].y = h;
            clips[ CLIP_MOUSEOUT ].w = w;
            clips[ CLIP_MOUSEOUT ].h = h;

            //Set the default sprite
            CopyClip(CLIP_MOUSEOVER);
        }

        ~ToggleDisplayButton()
        {
        }

    public:
        int HandleMouseMove(int x, int y)
        {
            return -1;
        }

        int HandleMouseDown(int x, int y)
        {
            if ( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {//Set the button sprite
                bToggled = !bToggled;
                CopyClip(bToggled ? CLIP_MOUSEOUT : CLIP_MOUSEOVER);
                return (bToggled ? command : -1);
            }

            return -1;
        }
        int HandleMouseUp(int x, int y)
        {
            return -1;
        }

};
#endif // TOGGLEDISPLAYBUTTON_H_INCLUDED
