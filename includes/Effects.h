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
        float dx;
        float dy;
        float angle;
    } Star;
    typedef struct tagEmitter
    {
        public:
        float x;
        float y;
        float z;
        float dx;
        float dy;
        int tx;
        int ty;
        float life;
        Star * particles;
        int numstars;
        int state;

/*
        tagEmitter(float xp, float yp, float zp, float dxp, float dyp, int stars, int lf) : x(xp), y(yp), z(zp), dx(dxp), dy(dyp)
        {
            state = 0;
            life = lf;
            numstars = stars;
            particles = new Star[numstars];
        }
        ~tagEmitter()
        {
            delete [] particles;
        }
*/
    }Emitter;
    int cdeltas[3];
    typedef struct tagEmptyColor
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
        void set(uchar _r, uchar _g, uchar _b)
        {
            r = _r;
            g = _g;
            b = _b;
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

    virtual void Process(float * spec, int * data, int color)
    {
        int y, y1 = 0;
        for (int x = 0; x < w; ++x)
        {
            y = sqrt(spec[x]) * 3 * h-4; // scale it (sqrt to make low values more visible)

            if (y > h) y = h - 1; // cap it
            if (x && (y1 = (y + y1) / 2)) // interpolate from previous to make the display smoother
            {
                if (y1 > h)
                    y1 = h - 1;   //cap it again.
                while (--y1>=0) data[y1 * w + x] = color << 16;
            }
            y1 = y;
            if (y > h)
                y = h - 1;
            while (--y>=0) data[y * w + x] = color + y; // draw level
        }
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
    void Process(float * spec, int * data, int color)
    {
        Effect::Process(spec, data, color);
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
    void Process(float * spec, int * data, int color)
    {
        Effect::Process(spec, data, color);
        int len = w * h;
        //memadd(m_bitmap, data, len);
        m_animation += 0.01f;
        //calculate the shift values out of the animation value
        int shiftX = (int)(w * 1.0 * m_animation);
        int shiftY = (int)(w * 0.25 * m_animation);

        int idx = 0;
        int col;

        unsigned int lkx;
        unsigned int lky;
        for(int x = 0; x < w; x++)
        for(int y = 0; y < h; y++)
        {
            idx = x + (y * w);
            //get the texel from the texture by using the tables, shifted with the animation values
            lkx = (unsigned int)(distanceTable[idx] + shiftX) % w;
            lky = (unsigned int)(angleTable[idx] + shiftY) % h;
            col = data[lkx + (lky * w)];
            m_bitmap[idx] = col;
        }
        memblur(m_bitmap, len);
        memcpy(data, m_bitmap, len * sizeof(int));
    }
};

class FractalEffect: public Effect
{
    float zoom;
    float czoom;
    float moveX;
    float moveY; //you can change these to zoom and change position
    int maxIterations; //after how much iterations the function should stop
    float frameTime;
    float frameRunner;
    float runVal;

    public:
    FractalEffect(int w, int h) : Effect(w, h), zoom(1.0f), moveX(0.0f), moveY(0.0f), maxIterations(64)
    {
        frameTime = 0.0f;
        czoom = zoom;
        frameRunner = 0.011f;
        runVal = 0.0001f;
    }
    ~FractalEffect()
    {
    }
    int HSVtoRGB(float hue, float s, float v)
    {
        Color col;
        int i;
        float f, p, q, t;
        if ( s == 0 )
        {
            // achromatic (grey)
            uchar vt = (uchar)v;
            col.set(vt, vt, vt);
            return col.getInt();
        }
        hue /= 60;			// sector 0 to 5
        i = floor( hue );
        f = hue - i;			// factorial part of h
        p = v * ( 1 - s );
        q = v * ( 1 - s * f );
        t = v * ( 1 - s * ( 1 - f ) );

        switch( i )
        {
            case 0:
                col.set((uchar)v, (uchar)t, (uchar)p);
                break;
            case 1:
                col.set((uchar)q, (uchar)v, (uchar)p);
                break;
            case 2:
                col.set((uchar)p, (uchar)v, (uchar)t);
                break;
            case 3:
                col.set((uchar)p, (uchar)q, (uchar)v);
                break;
            case 4:
                col.set((uchar)t, (uchar)p, (uchar)v);
                break;
            default:		// case 5:
                col.set((uchar)v, (uchar)p, (uchar)q);
                break;
        }
        return col.getInt();
    }
    void Process(float * spec, int * data, int color)
    {
        //Effect::Process(spec, data, color);

        //each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
        float cRe, cIm;                   //real and imaginary part of the constant c, determines shape of the Julia Set
        float newRe, newIm, oldRe, oldIm;   //real and imaginary parts of new and old

        //pick some values for the constant c, this determines the shape of the Julia Set
        cRe = -0.7;
        cIm = 0.27015;
        frameTime += 0.01f;

        if (frameTime > 2.0f)
            frameTime = 0.0f;
        if (czoom < zoom)
            czoom += (cIm / 2.0f);
        else if (czoom > zoom)
            czoom -= (cIm / 2.0f);

        //draw the fractal
        for(int x = 0; x < w; x++)
        for(int y = 0; y < h; y++)
        {
            //calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
            newRe = 1.5 * (x - w / 2) / (0.5 * czoom * w) + moveX;
            newIm = (y - h / 2) / (0.5 * czoom * h) + moveY;
            //i will represent the number of iterations
            int i;
            //start the iteration process
            for (i = 0; i < maxIterations; i++)
            {
                //remember value of previous iteration
                oldRe = newRe;
                oldIm = newIm;
                //the actual iteration, the real and imaginary part are calculated
                newRe = oldRe * oldRe - oldIm * oldIm + cRe;
                newIm = 2 * oldRe * oldIm + cIm;
                //if the point is outside the circle with radius 2: stop
                if((newRe * newRe + newIm * newIm) > 4) break;
            }

            //draw the pixel
            data[x + (y * w)] = HSVtoRGB((float)(i % 256), 255.0f, 255.0f * (i < maxIterations));
        }

        float specAvg = 1;
        for (int i = 0; i < 20; i+=2)
            specAvg += (spec[i] + spec[i + 1]);
        float dRot = 1.5;
        if (specAvg < 2.5f)
            dRot = 1;

        //moveX = (int)specAvg;
        if (zoom > 90.0f)
            zoom = 1.0f;

        zoom *= pow(dRot, frameTime / 4);
        moveX = (int)(czoom / 20.0f);
        //moveY = (int)(czoom /2);
        //cIm += 0.0002 * frameTime / czoom;
        //cRe += 0.0002 * frameTime / czoom;

        frameRunner += runVal;
        if (frameRunner > 0.1f)
            runVal *= -1.0f;
        if (frameRunner < 0.1f)
            runVal *= -1.0f;

        Effect::Process(spec, data, color >> 8);
    }
};

class FlameEffect : public Effect
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
        m_stars = new Star[numstars];
        m_fireworks = new Emitter[numfireworks];
        m_buffer = new int[w * h];
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
        //int * d = dst;
        //int * s = src;
        int idx = 0;

        //int ts;
        //int td;

        EmptyColor * ds = reinterpret_cast<EmptyColor *>(dst);
        EmptyColor * ss = reinterpret_cast<EmptyColor *>(src);
        //int r, g, b;
        while (idx < len)
        {
            //td = (int)*(reinterpret_cast<int *>(dd);
            //ts = *s;

            ds->r = (ds->r + ss->r) >> 1;
            ds->g = (ds->g + ss->g) >> 1;
            ds->b = (ds->b + ss->b) >> 1;
            //g = *d = (td + ts) >> 2;

            //d++;
            //s++;
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
        //int x, y;

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
                    m_fireworks[i].tx = (m_fireworks[i].x / m_fireworks[i].z) * w + (w >> 1);
                    m_fireworks[i].ty = (m_fireworks[i].y / m_fireworks[i].z) * h + (h >> 1);

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

                        m_fireworks[i].particles[j].tx = (m_fireworks[i].particles[j].x / m_fireworks[i].particles[j].z) * w + (w >> 1);
                        m_fireworks[i].particles[j].ty = (m_fireworks[i].particles[j].y / m_fireworks[i].particles[j].z) * h + (h >> 1);

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
        fireFilter(m_buffer);
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
#endif // EFFECTS_H_INCLUDED
