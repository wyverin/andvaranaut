#include "Gauge.h"

#include "util.h"

Gauge xzgauge()
{
    static Gauge gauge;
    return gauge;
}

Gauge xgnew()
{
    Gauge g = xzgauge();
    // TODO:
    // Maybe have max change with weapon? Stronger weapons have longer windup.
    g.max = 120; /* About two seconds max. */
    g.points = xtoss(Point, g.max);
    return g;
}

void xgfree(const Gauge g)
{
    free(g.points);
}

static Gauge reset(Gauge g)
{
    g.mx = g.my = g.count = 0;
    return g;
}

Gauge xgwind(Gauge g, const Classification c, const Input input)
{
    if(input.l)
    {
        // Fizzle out.
        if(g.count == g.max)
            g = reset(g);
        // Trace.
        g.points[g.count].x = (g.mx += input.dx);
        g.points[g.count].y = (g.my += input.dy);
        g.count++;
    }
    // Done winding.
    else g = reset(g);
    return g;
}
