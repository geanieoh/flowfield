#ifndef FLOWFIELD_H
#define FLOWFIELD_H

#include <stdio.h>
#include <stdlib.h>

#define CELL_RADIUS 0.5f

typedef enum{
    F_NORTH,
    F_SOUTH,
    F_EAST,
    F_WEST,
    F_NORTHWEST,
    F_NORTHEAST,
    F_SOUTHWEST,
    F_SOUTHEAST,
    F_NONE
} Direction;

typedef float vec2[2];
typedef int ivec2[2];

typedef struct{
    vec2 worldpos;
    int index;
    Direction dir;
    unsigned short bestCost;
    unsigned char cost;
} Cell;

typedef struct{
    Cell* grid;
    ivec2 gridsize;
    int destIndex;
} Flowfield;

void flowfield_init(Flowfield* field, ivec2 gridsize);
void integrationfield_create(Flowfield* field, vec2 target);
void flowfield_create(Flowfield* field);

#endif