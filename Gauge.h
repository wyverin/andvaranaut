#pragma once

#include "Point.h"
#include "Input.h"
#include "Weapon.h"
#include "Attack.h"

typedef struct
{
    int count;
    int max;
    Point* points;
    int mx;
    int my;
    float sfactor;
}
Gauge;

Gauge xgnew(const float sfactor);

void xgfree(const Gauge);

Gauge xgwind(Gauge, const Weapon, const Input);

Attack xgpower(const Gauge, const Input);
