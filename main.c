#include "flowfield.h"

#define WORLD_HEIGHT 11
#define WORLD_WIDTH 11

int main(){
    Flowfield field;
    flowfield_init(&field, (ivec2){WORLD_HEIGHT, WORLD_WIDTH});

    vec2 pos = {0.0f, 0.0f};

    integrationfield_create(&field, pos);
    flowfield_create(&field);
    
    printf("target coordinates: %.2f, %.2f\n", pos[0], pos[1]);
    for(int y = 0; y < field.gridsize[1]; y++){
        for(int x = 0; x < field.gridsize[0]; x++){
            switch(field.grid[y*field.gridsize[0] + x].dir){
                case(F_NORTH):
                    printf("  |  ");
                    break;
                case(F_SOUTH):
                    printf("  |  ");
                    break;
                case(F_EAST):
                    printf(" --> ");
                    break;
                case(F_WEST):
                    printf(" <-- ");
                    break;
                case(F_NORTHWEST):
                    printf("  \\  ");
                    break;
                case(F_SOUTHEAST):
                    printf("  \\  ");
                    break;
                case(F_NORTHEAST):
                    printf("  /  ");
                    break;
                case(F_SOUTHWEST):
                    printf("  /  ");
                    break;
                case(F_NONE):
                    printf("  o  ");
                    break;
            }
        }
        printf("\n\n");
    }
    printf("\n");

    return 0;
}