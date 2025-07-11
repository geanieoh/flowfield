# Flowfield
program creates a flowfield from a given point

## how
- creates a flat array of cells representing a grid
- from the target, works its way outward keeping track of each cells distance to the target
- each cell stores the direction of neighboring cells that have a closer distance to the target

## why
- efficient pathfinding for large number of entities