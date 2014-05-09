#ifndef MULTISTATEBUTTON_H_INCLUDED
#define MULTISTATEBUTTON_H_INCLUDED

#include "ToggleDisplayButton.h"
#include <vector>

using namespace std;
typedef vector<ToggleDisplayButton*>::iterator st_iter;

class MultiStateButton : public DisplayButton
{
    protected:
        int currentState;
        vector<ToggleDisplayButton*> m_states;

    public:
        MultiStateButton() : DisplayButton(), currentState(0)
        {

        }

        void AddState(const char * filename, int x, int y, int w, int h, int c)
        {
            ToggleDisplayButton * button = new ToggleDisplayButton(filename, x, y, w, h, c);
            m_states.push_back(button);
        }

        ~MultiStateButton()
        {
			for (st_iter i = m_states.begin(); i != m_states.end(); i++)
			{//delete the states here.
				delete (*i);
			}
        }

    public:
        int HandleMouseMove(int x, int y)
        {
            return 0;
        }

        int HandleMouseDown(int x, int y)
        {
            for (st_iter i = m_states.begin(); i != m_states.end(); i++)
			{
				if ((*i)->bToggled)   //if the button is already toggled
                {
                    printf("Current Button is toggled un-toggling button.\n");
                    (*i)->HandleMouseDown(x, y);
					return 0;
                }
			}
			currentState++;
			currentState %= m_states.size();
			int idx = 0;
            for (st_iter i = m_states.begin(); i != m_states.end(); i++)
			{
				if (idx == currentState)
					(*i)->HandleMouseDown(x, y);
				idx++;
			}
			return 1;
        }
        int HandleMouseUp(int x, int y)
        {
            return 0;
        }

        void Render(SDL_Surface * screen)
        {
            int idx = 0;
            for (st_iter i = m_states.begin(); i != m_states.end(); i++)
			{
				if (idx == currentState)
				{
					(*i)->Render(screen);
					break;
				}
			}
        }
};

#endif // MULTISTATEBUTTON_H_INCLUDED
