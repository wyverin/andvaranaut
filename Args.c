#include "Args.h"

#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

static void print(const Args args)
{
    xlicense();
    printf(
    /* 0 */ "-x: X-Resolution             : %d\n"
    /* 1 */ "-y: Y-Resolution             : %d\n"
    /* 2 */ "-f: Focal Length             : %f\n"
    /* 3 */ "-v: VSync                    : %s\n"
    /* 4 */ "-p: FPS                      : %d\n"
    /* 5 */ "-m: Mouse Sensitivity        : %f\n"
    /* 6 */ "-g: Gauge Sensitivity Factor : %f\n"
    /* 7 */ "-t: CPU Renderer Thread(s)   : %d\n",
    /* 0 */ args.xres,
    /* 1 */ args.yres,
    /* 2 */ args.focal,
    /* 3 */ args.vsync ? "t" : "f",
    /* 4 */ args.fps,
    /* 5 */ args.msen,
    /* 6 */ args.gfac,
    /* 7 */ args.threads);
}

static void check(const Args args)
{
    if(args.threads <= 0)
        xbomb("error: not a valid CPU thread count (-t)\n");
    if(args.xres % args.threads)
        xbomb("error: x-resolution (-x) not divisible by CPU thread count (-t).\n");
}

// Parses command line arguments.
static Args parsed(Args args, const int argc, const char* argv[])
{
    for(int i = 1; i < argc; i += 2)
    {
        if(argv[i][0] == '-')
        {
            if(argv[i][1] == 'h') print(args), xbomb("");
            // Out of bounds check.
            if(i + 1 == argc)
                break;
            // Parse argument list.
            const char* const next = argv[i + 1];
            const char option = argv[i][1];
            switch(option)
            {
                /* X-Resolution. */
                case 'x':
                    args.xres =
                        strtod(next, NULL);
                    break;
                /* Y-Resolution. */
                case 'y':
                    args.yres =
                        strtod(next, NULL);
                    break;
                /* Focal Length. */
                case 'f':
                    args.focal =
                        strtof(next, NULL); /* Notice the float. */
                    break;
                /* VSync. */
                case 'v':
                    args.vsync =
                        // True and false handling via strings.
                        equals(next, "true")  ? 1 : equals(next, "t") ? 1 :
                        equals(next, "false") ? 0 : equals(next, "f") ? 0 :
                        // True and false handling via integers.
                        strtod(next, NULL) != 0;
                    break;
                /* Frames per Second. */
                case 'p':
                    args.fps =
                        strtod(next, NULL);
                    break;
                /* Mouse Sensitivity. */
                case 'm':
                    args.msen =
                        strtof(next, NULL);
                    break;
                /* Gauge Sensitivity Factor. */
                case 'g':
                    args.gfac =
                        strtof(next, NULL);
                    break;
                /* CPU Threads for the Renderer. */
                case 't':
                    args.threads =
                        strtod(next, NULL);
                    break;
                /* Option not recognized. */
                default:
                    xbomb("error: option -%c not recognized\n", option);
                    break;
            }
        }
    }
    print(args);
    check(args);
    return args;
}

static Args defaults()
{
    Args args;
    xzero(args);
    args.xres = 800;
    args.yres = 450;
    args.focal = 1.0; /* 90 Degrees. */
    args.vsync = true;
    args.fps = 60;
    args.msen = 0.01;
    args.gfac = 3.00;
    args.threads = SDL_GetCPUCount();
    return args;
}

Args xparse(const int argc, const char* argv[])
{
    return parsed(defaults(), argc, argv);
}
