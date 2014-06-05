#ifndef MILKYEFFECT_H_INCLUDED
#define MILKYEFFECT_H_INCLUDED
#include "../Effects.h"
#include <stdlib.h>

class __declspec(dllexport) MilkyEffect : public Effect
{
    Star * m_stars;
    int numstars;

    public:
    MilkyEffect(int w, int h) : Effect(w, h)
    {
        numstars = h / 2;
        m_stars = new Star[numstars];

        srand(GetTickCount());
        for (int i = 0; i < numstars; ++i)
        {
            m_stars[i].x = (float)(rand()%70 - 35);
            m_stars[i].y = (float)(rand()%70 - 35);
            m_stars[i].z = (float)(rand()%100 + 1);
        }
    }
  ~MilkyEffect(){ delete [] m_stars; }

  void Process(float * spec, int * data, int color)
  {
    Effect::Process(spec, data, color);
    float specAvg = 1;
    for (int i = 0; i < 20; i+=2)
        specAvg += (spec[i] + spec[i + 1]);

    int a, x, y;
	for (a = 0; a < numstars; ++a)
	{
		m_stars[a].z -= (int)(specAvg);
		if (m_stars[a].z <= 0 || m_stars[a].tx <0 || m_stars[a].tx > w || m_stars[a].ty < 0 || m_stars[a].ty > h)
		{
			// re-initialize if star is too close
			m_stars[a].x = (float)(rand()%70 - 35);
			m_stars[a].y = (float)(rand()%70 - 35);
			m_stars[a].z += 100;
		}

		TransformPoint(m_stars[a], w, h);
		x = m_stars[a].tx;
		y = m_stars[a].ty;

		if (x < 0) x = 0;
		if (x >= w) x = w - 1;
		if (y < 0) y = 0;
		if (y >= h) y = h - 1;
		data[x + (y * w)] = 0xffffffff;
	}
  }
};

#endif // MILKYEFFECT_H_INCLUDED
