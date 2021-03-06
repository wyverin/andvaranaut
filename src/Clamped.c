#include "Clamped.h"

#include "util.h"

Clamped xzclamp()
{
    static Clamped clamped;
    return clamped;
}

Clamped xclamp(const int yres, const float bot, const float top)
{
    const Clamped clamp = {
        (int) bot < 0 ? 0 : xcl(bot), (int) top > yres ? yres : xfl(top)
    };
    return clamp;
}
