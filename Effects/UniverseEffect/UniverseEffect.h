#ifndef UNIVERSEEFFECT_H_INCLUDED
#define UNIVERSEEFFECT_H_INCLUDED

#include "../Effects.h"
#include <stdlib.h>

class __declspec(dllexport) UniverseEffect : public Effect
{
    public:
    Star * m_stars;
    int numstars;
    int angle;
    int rotation_delay;
    tagColor colorTag;
    int * m_buffer;
    public:
    UniverseEffect(int w, int h) : Effect(w, h), colorTag(0, 64, 96)
    {
        cdeltas[0] = 0;
        cdeltas[1] = -1;
        cdeltas[2] = 1;
        numstars = h;
        m_stars = new Star[numstars];
        m_buffer = new int[w * h];
        memset(m_buffer, 0, (w * h) * sizeof (int));

        srand(GetTickCount());
        for (int i = 0; i < numstars; ++i)
        {
            m_stars[i].x = (float)(rand()%70 - 35);
            m_stars[i].y = (float)(rand()%70 - 35);
            m_stars[i].z = (float)(rand()%100 + 1);
        }
        angle = 0;
        rotation_delay = 10;
    }
    ~UniverseEffect()
    {
        delete [] m_stars;
        delete [] m_buffer;
    }

    void blurFilter(int * data)
    {
        int color = *((int*)&colorTag);
        int * alias = data;
        int len = w * h;

        while (len-- > 0)
        {
            *alias |= color;
            alias++;
        };
        colorTag.runcolor(cdeltas, 32, 128);
    }
    void memadd(int * dst, int * src, int len)
    {
        int * d = dst;
        int * s = src;
        int idx = 0;
        int ts;
        int td;
        while (idx < len)
        {
            td = *d;
            ts = *s;

            *d = (td + ts) >> 2;

            d++;
            s++;
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
                bmp[i + 0] = temp.getInt();
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
                // re-initialize if star is too close
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
        memcpy(data, m_buffer, (w * h) * sizeof(int));
    }
};

#endif // UNIVERSEEFFECT_H_INCLUDED
