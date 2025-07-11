#include "flowfield.h"

typedef unsigned char _bool;
#define _true 1
#define _false 0

typedef struct Node Node;
typedef struct Node{
    Node* lneighbor;
    Node* rneighbor;
    Cell* cell;
} Node;

typedef struct{
    Node* back;
    Node* front;
    int count;
} Queue;

void queue_create(Queue* q);
void enqueue(Queue* q, Cell* cell);
void dequeue(Queue* q);

void index_to_world(Flowfield* field, int x, int y, vec2 dest){
    dest[0] = x*CELL_RADIUS*2 - field->gridsize[0]/2 + CELL_RADIUS; //gridsize/2 to set of grid origin at 0, 0 
    dest[1] = y*CELL_RADIUS*2 - field->gridsize[1]/2 + CELL_RADIUS;
}

int world_to_index(Flowfield* field, vec2 coord){
    float a = (int)coord[0] + CELL_RADIUS; 
    float b = (int)coord[1] + CELL_RADIUS; 
    return ((((b - CELL_RADIUS)*(-1)) + (field->gridsize[1]/2))/(CELL_RADIUS*2))*field->gridsize[0] + ((a - CELL_RADIUS) + (field->gridsize[0]/2)/(CELL_RADIUS*2));
}

void cell_get_neighbors(Flowfield* field, Cell* cell, Cell** neighbors, _bool wIntercardinal){
    if(cell->index - field->gridsize[0] >= 0) neighbors[F_NORTH] = &field->grid[cell->index - field->gridsize[0]]; //north of target cell
    else neighbors[F_NORTH] = NULL;
        
    if(cell->index + field->gridsize[0] < field->gridsize[0]*field->gridsize[1]) neighbors[F_SOUTH] = &field->grid[cell->index + field->gridsize[0]]; //south of target cell
    else neighbors[F_SOUTH] = NULL;

    if(cell->index + 1 < field->gridsize[0]*field->gridsize[1]) neighbors[F_EAST] = &field->grid[cell->index + 1]; //east of target cell
    else neighbors[F_EAST] = NULL;

    if(cell->index - 1 >= 0) neighbors[F_WEST] = &field->grid[cell->index - 1]; //west of target cell
    else neighbors[F_WEST] = NULL;

    if(cell->index%field->gridsize[0] == 0) neighbors[F_WEST] = NULL; //cell is on left edge, no cell to its west
    if((cell->index + 1)%field->gridsize[0] == 0) neighbors[F_EAST] = NULL; // cell is on right edge, no cell to its east

    if(wIntercardinal){
        if(neighbors[F_NORTH]){
            if(neighbors[F_NORTH]->index - 1 >= 0 && neighbors[F_NORTH]->index%field->gridsize[0] != 0) neighbors[F_NORTHWEST] = &field->grid[neighbors[F_NORTH]->index - 1]; //northwest
            else neighbors[F_NORTHWEST] = NULL;

            if(neighbors[F_NORTH]->index + 1 < field->gridsize[0]*field->gridsize[1] && (neighbors[F_NORTH]->index + 1)%field->gridsize[0] != 0) neighbors[F_NORTHEAST] = &field->grid[neighbors[0]->index + 1]; //northeast
            else neighbors[F_NORTHEAST] = NULL;
        }

        if(neighbors[F_SOUTH]){
            if(neighbors[F_SOUTH]->index - 1 >= 0 && neighbors[F_SOUTH]->index%field->gridsize[0] != 0) neighbors[F_SOUTHWEST] = &field->grid[neighbors[F_SOUTH]->index - 1]; //southwest
            else neighbors[F_SOUTHWEST] = NULL;

            if(neighbors[F_SOUTH]->index + 1 < field->gridsize[0]*field->gridsize[1] && (neighbors[F_SOUTH]->index + 1)%field->gridsize[0] != 0) neighbors[F_SOUTHEAST] = &field->grid[neighbors[F_SOUTH]->index + 1]; //southeast
            else neighbors[F_SOUTHEAST] = NULL;
        }
    }
}

void flowfield_init(Flowfield* field, ivec2 gridsize){
    field->gridsize[0] = gridsize[0]/(CELL_RADIUS*2); field->gridsize[1] = gridsize[1]/(CELL_RADIUS*2);
    field->grid = malloc(sizeof(Cell)*field->gridsize[0]*field->gridsize[1]);

    for(int y = 0; y < field->gridsize[1]; y++){
        for(int x = 0; x < field->gridsize[0]; x++){
            index_to_world(field, x, y, field->grid[y*field->gridsize[0] + x].worldpos);
            field->grid[y*field->gridsize[0] + x].index = y*field->gridsize[0] + x;
            field->grid[y*field->gridsize[0] + x].dir = F_NONE;
            field->grid[y*field->gridsize[0] + x].cost = 1;
            field->grid[y*field->gridsize[0] + x].bestCost = 65535;
        }
    }
}

void integrationfield_create(Flowfield* field, vec2 target){
    field->destIndex = world_to_index(field, target);
    field->grid[field->destIndex].bestCost = 0;
    field->grid[field->destIndex].cost = 0;

    Queue* q = malloc(sizeof(Queue));
    queue_create(q);
    enqueue(q, &field->grid[field->destIndex]);

    while(q->count > 0){
        Cell* cur = q->front->cell;

        Cell* neighbors[4];
        cell_get_neighbors(field, cur, neighbors, _false);

        for(int i = 0; i < 4; i++){
            if(neighbors[i] == NULL) continue;
            if(neighbors[i]->cost == 255) continue;

            if(neighbors[i]->cost + cur->bestCost < neighbors[i]->bestCost){
                neighbors[i]->bestCost = neighbors[i]->cost + cur->bestCost;
                enqueue(q, neighbors[i]);
            }
        }

        dequeue(q);
    }

    free(q);
}

void flowfield_create(Flowfield* field){
    for(int i = 0; i < field->gridsize[0]*field->gridsize[1]; i++){
        Cell* neighbors[8];
        cell_get_neighbors(field, &field->grid[i], neighbors, _true);

        int bestCost = field->grid[i].bestCost;
        for(int j = 0; j < 8; j++){
            if(neighbors[j] == NULL) continue;

            if(neighbors[j]->bestCost < bestCost){
                bestCost = neighbors[j]->bestCost;
                field->grid[i].dir = j;
            }
        }
    }
}

void queue_create(Queue* q){
    q->back = NULL;
    q->front = NULL;
    q->count = 0;
}

void enqueue(Queue* q, Cell* cell){
    Node* node = malloc(sizeof(Node));
    node->cell = cell;
    node->lneighbor = NULL;

    if(q->count == 0){
        node->rneighbor = NULL;
        q->front = node;
        q->back = node;
    }
    else{
        q->back->lneighbor = node;
        node->rneighbor = q->back;
        q->back = node;
    }

    q->count++;
}

void dequeue(Queue* q){
    if(q->count == 0) return;

    Node* temp = q->front;
    q->front = q->front->lneighbor;

    if(q->front) q->front->rneighbor = NULL;
    else q->back = NULL;

    free(temp);

    q->count--;
}