#ifndef EFFECTS_H_INCLUDED
#define EFFECTS_H_INCLUDED

typedef unsigned char uchar;

class Effect
{
    public:
    typedef struct tagStar
    {
        float x;
        float y;
        float z;
        int tx;
        int ty;
        float angle;
    } Star;

    int cdeltas[3];
    typedef struct tagEmptycolor
    {
    public:
        uchar u;
        uchar r;
        uchar g;
        uchar b;

    } EmptyColor;
    typedef struct tagColor
    {
        public:
        uchar b;
        uchar g;
        uchar r;
        uchar u;

        tagColor(uchar rd, uchar gr, uchar bl) : b(bl), g(gr), r(rd), u(0)
        {
        }
        tagColor() : b(0), g(0), r(0), u(0)
        {

        }
        uchar add(uchar val)
        {
            uchar ret = val + 1;
            if (ret > 254)
                ret = 255;
            return ret;
        }
        uchar subtract(uchar val)
        {
            uchar ret = val - 1;
            if (ret < 1)
                ret = 0;
            return ret;
        }
        uchar run(uchar val, int * cdeltas, int cidx, int min, int max)
        {
            uchar ret;
            int dt = cdeltas[cidx];

            if (dt == 0)
                return val;

            if (dt > 0)
                ret = add(val);
            else
                ret = subtract(val);

            if (ret == max)
                cdeltas[cidx] = -1;
            else if (ret == min)
                cdeltas[cidx] = 1;
            return ret;
        }
        void runcolor(int * deltas, int min, int max)
        {
            r = run(r, deltas, 0, min, max);
            g = run(g, deltas, 1, min, max);
            b = run(b, deltas, 2, min, max);
        }
        uchar averageChannel(uchar t, uchar b, uchar l, uchar r)
        {
            int re = (t + b + l + r) >> 2;
            return (uchar)re;
        }
        void avg4(tagColor * tp, tagColor * bm, tagColor * lt, tagColor * rt)
        {
            r = averageChannel(tp->r, bm->r, lt->r, rt->r);
            g = averageChannel(tp->g, bm->g, lt->g, rt->g);
            b = averageChannel(tp->b, bm->b, lt->b, rt->b);
        }
        int getInt()
        {
            int intrep = ((int)u << 24 | (int)r << 16 | (int)g << 8 | (int)b);
            return intrep;
        }
        void setInt(int col)
        {
            u = (char)((int)(col & 0xff000000) >> 24);
            r = (char)((int)(col & 0x00ff0000) >> 16);
            g = (char)((int)(col & 0x0000ff00) >> 8);
            b = (char)((int)(col & 0x000000ff));
        }
    } Color;
    int w;
    int h;
    float m_degrad;

    public:
    Effect(int ew, int eh) : w(ew), h(eh)
    {
        m_degrad = (float)(M_PI / 180.0);
        memset(cdeltas, 0, 3 * sizeof(int));
    }
    virtual ~Effect(){}

    public:

    virtual void Process(float * spec, int * data)
    {
    }
};

class MilkyEffect : public Effect
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

  void Process(float * spec, int * data)
  {
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

		// perspective transformation - this is interesting! Note the
		// transformation from 3D coordinates to 2D:
		//
		// x2D = x3D / z3D * (screen width) + (half of screen width)
		// y2D = y3D / z3D * (screen height) + (half of screen height)
		//
		// Pretty neat, hey?

		// Note also that since I'm using the variable screen dimensions,
		// the starfield changes size automatically if you resize the
		// window.
		m_stars[a].tx = (m_stars[a].x / m_stars[a].z) * w + (w >> 1);
		m_stars[a].ty = (m_stars[a].y / m_stars[a].z) * h + (h >> 1);
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

class UniverseEffect : public Effect
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
    /*
    void fireFilter(int * data)
    {
        int * bmp = data;
        int idx;
        int i;
        for (int y = 1; y < h-1; ++y)
        {
            idx = (y * w);
            for (int x = 1; x < w-1; ++x)
            {
                i = idx + x;
                bmp[i] = (bmp[i - 1] + bmp[i + 1] + bmp[i - w] + bmp[i + w]) >> 2;
                //bmp[i + w] = (bmp[i - 1] + bmp[i + 1] + bmp[i - w] + bmp[i]) >> 3;
            }
        }
    }
    */
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
    void Process(float * spec, int * data)
    {
        //blurFilter(data);
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

            m_stars[a].tx = (m_stars[a].x / m_stars[a].z) * w + (w >> 1);
            m_stars[a].ty = (m_stars[a].y / m_stars[a].z) * h + (h >> 1);
            x = m_stars[a].tx;
            y = m_stars[a].ty;

            if (x < 0) x = 0;
            if (x >= w) x = w - 1;
            if (y < 0) y = 0;
            if (y >= h) y = h - 1;
            alias[x + (y * w)] = 0xffffffff;
        }

        //memadd(data, m_buffer, (w * h));
        memcpy(data, m_buffer, (w * h) * sizeof(int));
    }
};

class StarGateEffect : public Effect
{
    int * distanceTable;
    int * angleTable;
    int * m_bitmap;
    float m_animation;
    public:
    StarGateEffect(int w, int h) : Effect(w, h), m_animation(0.0f)
    {
        int len = w * h;
        m_bitmap = new int[len];
        memset(m_bitmap, 0, len * sizeof(int));

        distanceTable = new int[len];
        angleTable = new int[len];

        //precompute lookup tables.
        //generate non-linear transformation table
        int angle, distance;
        float ratio = 32.0;
        for(int x = 0; x < w; x++)
        for(int y = 0; y < h; y++)
        {
            distance = (int)(ratio * h / sqrt((x - w / 2.0) * (x - w / 2.0) + (y - h / 2.0) * (y - h / 2.0))) % h;
            angle = (unsigned int)(0.5 * w * atan2(y - h / 2.0, x - w / 2.0) / 3.1416);
            distanceTable[x + (y * w)] = distance;
            angleTable[x + (y * w)] = angle;
        }
    }
    ~StarGateEffect()
    {
        delete [] m_bitmap;
        delete [] distanceTable;
        delete [] angleTable;
    }
    void memblur(int * dst, int len)
    {
        EmptyColor * tp;
        EmptyColor * lt;
        EmptyColor * bm;
        EmptyColor * rt;

        Color col;

        int ypos, idx;
        for (int y = 1; y < h-1; ++y)
        {
            ypos = (y * w);
            for (int x = 1; x < w-1; ++x)
            {
                idx = ypos + x;

                lt = reinterpret_cast<EmptyColor *>(&dst[idx - 1]);
                rt = reinterpret_cast<EmptyColor *>(&dst[idx + 1]);
                tp = reinterpret_cast<EmptyColor *>(&dst[idx - w]);
                bm = reinterpret_cast<EmptyColor *>(&dst[idx + w]);

                col.r = col.averageChannel(tp->r, bm->r, lt->r, rt->r);
                col.g = col.averageChannel(tp->g, bm->g, lt->g, rt->g);
                col.b = col.averageChannel(tp->b, bm->b, lt->b, rt->b);

                dst[idx - w] = col.getInt();
            }
        }
    }
    void Process(float * spec, int * data)
    {
        int len = w * h;
        //memadd(m_bitmap, data, len);
        m_animation += 0.01f;
        //calculate the shift values out of the animation value
        int shiftX = (int)(w * 1.0 * m_animation);
        int shiftY = (int)(w * 0.25 * m_animation);

        int idx = 0;
        int color;

        unsigned int lkx;
        unsigned int lky;
        for(int x = 0; x < w; x++)
        for(int y = 0; y < h; y++)
        {
            idx = x + (y * w);
            //get the texel from the texture by using the tables, shifted with the animation values
            lkx = (unsigned int)(distanceTable[idx] + shiftX) % w;
            lky = (unsigned int)(angleTable[idx] + shiftY) % h;
            color = data[lkx + (lky * w)];
            m_bitmap[idx] = color;
        }
        memblur(m_bitmap, len);
        memcpy(data, m_bitmap, len * sizeof(int));
    }
};
#endif // EFFECTS_H_INCLUDED
