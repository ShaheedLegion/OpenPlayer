#ifndef FLAMEEFFECT_H_INCLUDED
#define FLAMEEFFECT_H_INCLUDED

#include "../Effects.h"
#include <stdlib.h>

class __declspec(dllexport) FlameEffect : public Effect
{
    public:
    Star * m_stars;
    Emitter * m_fireworks;
    int numstars;
    int numfireworks;
    int angle;
    int rotation_delay;
    tagColor colorTag;
    int * m_buffer;

    public:
    FlameEffect(int w, int h) : Effect(w, h), colorTag(0, 64, 96)
    {
        cdeltas[0] = 0;
        cdeltas[1] = -1;
        cdeltas[2] = 1;
        numstars = h;
        numfireworks = 40;
        try{
        m_stars = new Star[numstars];
        m_fireworks = new Emitter[numfireworks];
        m_buffer = new int[w * h];
        }
        catch (...){}
        memset(m_buffer, 0, (w * h) * sizeof (int));

        srand(GetTickCount());
        for (int i = 0; i < numstars; ++i)
        {
            m_stars[i].x = (float)(rand()%70 - 35);
            m_stars[i].y = (float)(rand()%70 - 35);
            m_stars[i].z = (float)(rand()%100 + 1);
        }
        for (int i = 0; i < numfireworks; ++i)
        {
            InitFirework(i);
            m_fireworks[i].state = 0;//(int)((float)(rand() * 3.0f));   //this is deterministic - all of them will go into their specific states when required.
            m_fireworks[i].numstars = 80;
            m_fireworks[i].particles = new Star[m_fireworks[i].numstars];
        }
        angle = 0;
        rotation_delay = 10;
    }

    ~FlameEffect()
    {
        delete [] m_stars;
        for (int i = 0; i < numfireworks; ++i)
            delete [] m_fireworks[i].particles;
        delete [] m_buffer;
    }

    void memadd(int * dst, int * src, int len)
    {
        int idx = 0;

        EmptyColor * ds = reinterpret_cast<EmptyColor *>(dst);
        EmptyColor * ss = reinterpret_cast<EmptyColor *>(src);

        while (idx < len)
        {
            ds->r = (ds->r + ss->r) >> 1;
            ds->g = (ds->g + ss->g) >> 1;
            ds->b = (ds->b + ss->b) >> 1;

            ds++;
            ss++;
            idx++;
        }
    }

    void fireFilter(int * data)
    {
        int * bmp = data;
        int idx;
        int i;
        tagColor tp;
        tagColor lt;
        tagColor bm;
        tagColor rt;
        tagColor temp(0, 0, 0);
        int ccol;

        for (int y = 1; y < h-1; ++y)
        {
            idx = (y * w);
            for (int x = 1; x < w-1; ++x)
            {
                i = idx + x;
                tp.setInt(bmp[i + w]);
                bm.setInt(bmp[i - w]);
                lt.setInt(bmp[i - 1]);
                rt.setInt(bmp[i + 1]);

                temp.avg4(&tp, &bm, &lt, &rt);
                ccol = temp.getInt();
                bmp[i + 0] = ccol;
                bmp[i + w] = ccol;
            }
        }
    }
    void Process(float * spec, int * data, int color)
    {
        Effect::Process(spec, data, color);
        memadd(m_buffer, data, (w * h));
        fireFilter(m_buffer);

        float specAvg = 1;
        for (int i = 0; i < 20; i+=2)
            specAvg += (spec[i] + spec[i + 1]);

        int a, x, y;
        float dRot = 1;
        if (specAvg < 2.5f)
            dRot = -1;

        float rads = dRot * m_degrad;
        float s = sin(rads);
        float c = cos(rads);
        float nx;
        float ny;

        int * alias = m_buffer;

        for (a = 0; a < numstars; ++a)
        {
            m_stars[a].z -= (int)(specAvg * 2.0f);
            if (m_stars[a].z <= 0 || m_stars[a].tx <0 || m_stars[a].tx > w || m_stars[a].ty < 0 || m_stars[a].ty > h)
            {
                m_stars[a].x = (float)(rand()%70 - 35);
                m_stars[a].y = (float)(rand()%70 - 35);
                m_stars[a].z += 100;
            }

            nx = m_stars[a].x * c - m_stars[a].y * s;
            ny = m_stars[a].x * s + m_stars[a].y * c;
            m_stars[a].x = nx;
            m_stars[a].y = ny;

            TransformPoint(m_stars[a], w, h);
            x = m_stars[a].tx;
            y = m_stars[a].ty;

            if (x < 0) x = 0;
            if (x >= w) x = w - 1;
            if (y < 0) y = 0;
            if (y >= h) y = h - 1;
            alias[x + (y * w)] = 0xffffffff;
        }

        for (int i = 0; i < numfireworks; ++i)
        {
            switch (m_fireworks[i].state)
            {
                case 0: //moving ... travelling to piont where it explodes
                {
                    m_fireworks[i].z--;

                    if (m_fireworks[i].z < m_fireworks[i].life)
                    {
                        m_fireworks[i].state = 1;   //set it to exploding state...
                        for (int j = 0; j < m_fireworks[i].numstars; ++j)
                        {
                            m_fireworks[i].particles[j].x = m_fireworks[i].x;
                            m_fireworks[i].particles[j].y = m_fireworks[i].y;
                            m_fireworks[i].particles[i].dx = ((rand() / (float)RAND_MAX) * 2.0f) - 1.0f;
                            m_fireworks[i].particles[i].dy = ((rand() / (float)RAND_MAX) * 2.0f) - 1.0f;
                            m_fireworks[i].particles[j].z = 99.0f;//m_fireworks[i].z;
                        }
                        continue;
                    }

                    TransformPoint(m_fireworks[i], w, h);

                    x = m_fireworks[i].tx;
                    y = m_fireworks[i].ty;

                    if (x < 0) x = 0;
                    if (x >= w) x = w - 1;
                    if (y < 0) y = 0;
                    if (y >= h) y = h - 1;
                    alias[x + (y * w)] = 0xffffffff;
                }break;
                case 1: //exploding state ... now draw the stars of the "firework"
                {
                    int numalivestars = m_fireworks[i].numstars;
                    for (int j = 0; j < m_fireworks[i].numstars; ++j)
                    {
                        m_fireworks[i].particles[j].x += m_fireworks[i].particles[j].dx;
                        m_fireworks[i].particles[j].y += m_fireworks[i].particles[j].dy;
                        m_fireworks[i].particles[j].z--;
                        if (m_fireworks[i].particles[j].z < 0)
                        {
                            numalivestars--;
                            continue;
                        }

                        TransformPoint(m_fireworks[i].particles[j], w, h);

                        x = m_fireworks[i].particles[j].tx;
                        y = m_fireworks[i].particles[j].ty;

                        if (x < 0) x = 0;
                        if (x >= w) x = w - 1;
                        if (y < 0) y = 0;
                        if (y >= h) y = h - 1;
                        alias[x + (y * w)] = 0xff64ffbb;
                    }
                    if (numalivestars == 0)
                    {
                        m_fireworks[i].state = 2;   //set it to waiting state...
                        m_fireworks[i].life = -(float)(rand()%50);
                    }
                }break;
                case 2:
                {
                    m_fireworks[i].life++;
                    if (m_fireworks[i].life > 0)
                    {
                        m_fireworks[i].state = 0;   //set it to travelling state...
                        InitFirework(0);
                    }
                }break;
                default:
                break;  //should never get here ...
            }
        }
        memcpy(data, m_buffer, (w * h) * sizeof(int));
    }

    void InitFirework(int i)
    {
        m_fireworks[i].x = (float)(rand()%70 - 35);
        m_fireworks[i].y = (float)(rand()%70 - 35);
        m_fireworks[i].z = 99.0f;//(float)(rand()%100 + 1);
        m_fireworks[i].dx = (float)(rand() % 2 - 1);
        m_fireworks[i].dy = (float)((rand() % 2 - 1) + 0.1f);
        m_fireworks[i].life = (float)(rand()%60);//(int)((float)(rand() * 50 + 5));
    }
};

#endif // FLAMEEFFECT_H_INCLUDED
