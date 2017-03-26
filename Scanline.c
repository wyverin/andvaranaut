#include "Scanline.h"

#include "util.h"

void wrend(const Scanline scanline, const Wall wall, const Hit hit)
{
    const SDL_Surface* const surface = scanline.gpu.surfaces.surface[hit.tile];
    const int col = surface->w * hit.offset;
    const uint32_t* const pixels = surface->pixels;
    for(int x = wall.clamped.bot; x < wall.clamped.top; x++)
    {
        const int row = surface->h * (x - wall.bot) / (wall.top - wall.bot);
        scanline.display.pixels[x + scanline.y * scanline.display.width] = pixels[row + col * surface->w];
    }
}

void frend(
    const Scanline scanline,
    const Wall wall,
    Point wheres[],
    const Traceline traceline,
    char** const floring,
    const float party[])
    {
        for(int x = 0; x < wall.clamped.bot; x++)
        {
            const Point where = wheres[scanline.res - 1 - x] = lerp(traceline.trace, party[x] / traceline.corrected.x);
            const SDL_Surface* const surface = scanline.gpu.surfaces.surface[tile(where, floring)];
            const int col = surface->w * dec(where.x);
            const int row = surface->h * dec(where.y);
            const uint32_t* const pixels = surface->pixels;
            scanline.display.pixels[x + scanline.y * scanline.display.width] = pixels[row + col * surface->w];
        }
    }

void crend(
    const Scanline scanline,
    const Wall wall,
    const Point wheres[],
    char** const ceiling)
    {
        for(int x = wall.clamped.top; x < scanline.res; x++)
        {
            const Point where = wheres[x];
            if(tile(where, ceiling))
            {
                const SDL_Surface* const surface = scanline.gpu.surfaces.surface[tile(where, ceiling)];
                const int col = surface->w * dec(where.x);
                const int row = surface->h * dec(where.y);
                const uint32_t* const pixels = surface->pixels;
                scanline.display.pixels[x + scanline.y * scanline.display.width] = pixels[row + col * surface->w];
            }
        }
    }

void brend(const Scanline scanline)
{
    for(int x = 0; x < scanline.res; x++)
        scanline.display.pixels[x + scanline.y * scanline.display.width] = 0x0;
}

void srend(const Scanline scanline)
{
    for(int x = scanline.res / 2; x < scanline.res; x++)
        scanline.display.pixels[x + scanline.y * scanline.display.width] = 0x0;
}
