#include "Projection.h"

#include "Clamped.h"

#include <math.h>

Projection xzproj()
{
    static Projection proj;
    return proj;
}

Projection xproject(const int yres, const int xres, const float focal, const float yaw, const Point corrected, const float height)
{
    // The corrected x distance must be clamped to a value small enough otherwise size will
    // exceed the limitations of single precision floating point. The clamp value is arbitrary.
    const float min = 1e-5;
    const float normal = corrected.x < min ? min : corrected.x;
    Projection p;
    p.size = 0.5f * focal * xres / normal;
    p.mid = yaw * yres / 2.0f;
    p.bot = p.mid + (0.0f - height) * p.size;
    p.top = p.mid + (1.0f - height) * p.size;
    p.clamped = xclamp(yres, p.bot, p.top);
    p.height = height;
    p.yres = yres;
    p.sheer.a = 0.0f;
    p.sheer.b = 0.0f;
    return p;
}

Projection xsheer(Projection p, const Sheer s)
{
    p.bot += p.size * s.a - (s.b >= s.a  ? 1.0f : 0.0f); // Pretty ridiculous imo, but there is no other way
    p.top += p.size * s.b + (s.b >= s.a  ? 0.0f : 2.0f); // to fix the missing pixels between stacks.
    p.clamped = xclamp(p.yres, p.bot, p.top);
    p.sheer.a = fabsf(s.a);
    p.sheer.b = fabsf(s.b);
    return p;
}

float xccast(const Projection p, const int x)
{
    return (1.0f - p.height + p.sheer.b) * p.size / (x + 1 - p.mid);
}

float xfcast(const Projection p, const int x)
{
    return (0.0f - p.height - p.sheer.a) * p.size / (x + 0 - p.mid);
}
