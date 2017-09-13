#include "Sdl.h"

#include "Frame.h"
#include "Scanline.h"
#include "util.h"

static void churn(const Sdl sdl)
{
    SDL_RenderCopyEx(sdl.renderer, sdl.texture, NULL, NULL, -90.0, NULL, (SDL_RendererFlip) 0);
}

static void present(const Sdl sdl)
{
    SDL_RenderPresent(sdl.renderer);
}

static SDL_Rect clip(const SDL_Rect frame, const Point where, const int res, Point* const correcteds)
{
    SDL_Rect seen = frame;
    // Clips sprite from the left
    for(; seen.w > 0; seen.w--, seen.x++)
    {
        const int x = seen.x;
        if(x < 0 || x >= res)
            continue;
        if(where.x < correcteds[x].x)
            break;
    }
    // Clips sprite from the right
    for(; seen.w > 0; seen.w--)
    {
        const int x = seen.x + seen.w;
        if(x < 0 || x >= res)
            continue;
        if(where.x < correcteds[x].x)
        {
            seen.w = seen.w + 1;
            break;
        }
    }
    return seen;
}

static void paste(const Sdl sdl, const Sprites sprites, Point* const correcteds, const Hero hero, const int ticks)
{
    // Go through all the sprites
    for(int which = 0; which < sprites.count; which++)
    {
        const Sprite sprite = sprites.sprite[which];
        // Moves onto the next sprite if this sprite is behind the player
        if(sprite.where.x < 0)
            continue;
        // Calculates sprite size - the sprite must be an even
        // integer else the sprite will jitter with movement
        const int size = balance(focal(hero.fov) * sdl.res / sprite.where.x);
        // Calculates sprite location on screen
        const int corner = (sdl.res - size) / 2;
        const int slider = (sdl.res / 2) * ratio(hero.fov) * slp(sprite.where);
        const SDL_Rect target = { corner + slider, corner, size, size };
        // Moves onto the next sprite if this sprite is off screen
        if(target.x + target.w < 0 || target.x >= sdl.res)
            continue;
        // Selects sprite
        const int selected = sprite.ascii - ' ';
        SDL_Surface* const surface = sdl.surfaces.surface[selected];
        SDL_Texture* const texture = sdl.textures.texture[selected];
        const int w = surface->w / FRAMES;
        const int h = surface->h / STATES;
        const SDL_Rect image = { w * (ticks % FRAMES), h * sprite.state, w, h };
        // Gcc likes to mess with _seen_
        const volatile SDL_Rect seen = clip(target,
            sprite.where, sdl.res, correcteds);
        // Moves onto the next sprite if this sprite totally behind a wall
        if(seen.w <= 0)
            continue;
        // Applies lighting to the sprite
        const int modding = illuminate(hero.torch, sprite.where.x);
        SDL_SetTextureColorMod(texture, modding, modding, modding);
        // Applies transperancy to the sprite
        if(sprite.transparent)
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
        // Buffers the sprite
        SDL_RenderSetClipRect(sdl.renderer, (SDL_Rect*) &seen);
        SDL_RenderCopy(sdl.renderer, texture, &image, &target);
        SDL_RenderSetClipRect(sdl.renderer, NULL);
        // Removes transperancy from the sprite
        if(sprite.transparent)
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }
}

Sdl setup(const int res, const int fps)
{
    const uint32_t format = SDL_PIXELFORMAT_ARGB8888;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    Sdl sdl;
    zero(sdl);
    sdl.window = SDL_CreateWindow("water", 0, 0, res, res, SDL_WINDOW_SHOWN);
    if(!sdl.window)
        bomb("error: could not open window\n");
    sdl.renderer = SDL_CreateRenderer(
        sdl.window, -1, SDL_RENDERER_ACCELERATED);
    sdl.texture = SDL_CreateTexture(sdl.renderer,
        format, SDL_TEXTUREACCESS_STREAMING, res, res);
    sdl.surfaces = pull(format);
    sdl.textures = cache(sdl.surfaces, sdl.renderer);
    sdl.res = res;
    sdl.fps = fps;
    return sdl;
}

void release(const Sdl sdl)
{
    clean(sdl.surfaces);
    purge(sdl.textures);
    SDL_DestroyTexture(sdl.texture);
    SDL_Quit();
    SDL_DestroyWindow(sdl.window);
    SDL_DestroyRenderer(sdl.renderer);
}

void render(const Sdl sdl, const Hero hero, const Sprites sprites, const Map map, const int ticks)
{
    // Sprite location relative to player
    const Sprites relatives = arrange(sprites, hero);
    // Preallocations for render computations
    Point* wheres = toss(Point, sdl.res);
    int* moddings = toss(int, sdl.res);
    Point* const correcteds = toss(Point, sdl.res);
    // Raycaster: buffers with lighting walls, ceilings, floors, and sprites
    const Line camera = rotate(hero.fov, hero.theta);
    const Display display = lock(sdl);
    for(int y = 0; y < sdl.res; y++)
    {
        const Point column = lerp(camera, y / (float) sdl.res);
        const Ray ray = march(hero, map.walling, column, sdl.res);
        const Scanline scanline = { sdl, display, y };
        wrend(scanline, ray);
        frend(scanline, ray, map, wheres, hero.fov);
        crend(scanline, ray, map, wheres);
        light(scanline, ray, hero.torch, wheres, moddings);
        correcteds[y] = ray.traceline.corrected;
    }
    unlock(sdl);
    churn(sdl);
    paste(sdl, relatives, correcteds, hero, ticks);
    present(sdl);
    free(correcteds);
    free(wheres);
    free(moddings);
    kill(relatives);
}
