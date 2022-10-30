#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define size 20

typedef struct vCell{
    int x, y;
} cell;

typedef struct vStack{
    cell point[size * size];
    int top;
} stack;

typedef struct mMaze{
    char **mass;
    int count_str, count_col, vis;
} vMaze;

void   zero_stack (stack*);
void   push       (stack*, cell);
cell   pop        (stack*);

vMaze  generate_maze   (int, int);
char*  check_neighbors (vMaze*, cell);
char** create_mass     (int, int);
cell   next_point      (cell, char*);
void   out_maze        (vMaze*);
void   destroy_wall    (vMaze*, cell, cell);
void   out_picture     (vMaze*);

int main() {
    vMaze maze;
    maze = generate_maze(size, size);
    out_maze(&maze);
    out_picture(&maze);
    return 0;
}

void out_picture(vMaze *maze){
    FILE* pic;
    pic = fopen("pic.pgm", "wb");
    fprintf(pic, "P2\n");
    fprintf(pic, "%d %d\n", maze->count_col * 2 + 1, maze->count_str * 2 + 1);
    fprintf(pic, "255\n");
    int count = 0, j, temp;
    for (int i = 0; i < maze->count_str * 2 + 1; i++) {
        for (j = 0; j < maze->count_col * 2 + 1; j++)
            fprintf(pic, "%d ", maze->mass[i][j] ? 255 : 0);
        fprintf(pic, "\n");
    }
    fclose(pic);
}

vMaze generate_maze(int size_str, int size_col){
    vMaze maze;
    stack st;
    zero_stack(&st);
    srand(time(0));

    maze.mass = create_mass(size_str, size_col);
    maze.count_col = size_col;
    maze.count_str = size_str;
    maze.vis = 1;

    cell nPoint, pPoint;

    pPoint.x = 1;
    pPoint.y = 1;
    maze.mass[pPoint.x][pPoint.y] = 2;

    char *neig;
    while(maze.vis < maze.count_col*maze.count_str){
        neig = check_neighbors(&maze, pPoint);
        if(neig[0] > 0){
            push(&st, pPoint);
            nPoint = next_point(pPoint, neig);

            if(maze.mass[nPoint.y][nPoint.x] != 2){
                maze.mass[nPoint.y][nPoint.x] = 2;
                maze.vis++;
            }

            destroy_wall(&maze, pPoint, nPoint);
            pPoint = nPoint;

        } else {
            nPoint = pop(&st);
            pPoint = nPoint;
        }
        free(neig);
    }

    return maze;
}

void destroy_wall(vMaze *maze, cell point_one, cell point_two){
    if(point_one.x != point_two.x) maze->mass[point_one.y][(point_one.x+point_two.x)/2] = 2;
    else if(point_one.y != point_two.y) maze->mass[(point_one.y+point_two.y)/2][point_one.x] = 2;
}

cell next_point(cell point, char *neig){
    cell nPoint;
    char pos = rand() % neig[0];
    int j = 1;

    for(int i = 0; i < pos + 1; j++)
        if(neig[j])
            i++;

    j--;
    switch(j){
        case 1:
            nPoint.x = point.x;
            nPoint.y = point.y-2;
            break;

        case 2:
            nPoint.x = point.x+2;
            nPoint.y = point.y;
            break;

        case 3:
            nPoint.x = point.x;
            nPoint.y = point.y+2;
            break;

        case 4:
            nPoint.x = point.x-2;
            nPoint.y = point.y;
            break;
    }

    return nPoint;
}

char* check_neighbors(vMaze *maze, cell point){
    char top, down, right, left, i=0;
    top = down = right = left = 0;

    if(point.x > 0 && point.y > 0 && point.x < maze->count_col * 2 + 1 && point.y < maze->count_str * 2 + 1){
        if(point.y - 2 > 0                       && maze->mass[point.y - 2][point.x] == 1) {top = 1;   i++;}
        if(maze->count_col * 2 + 1 > point.x + 2 && maze->mass[point.y][point.x + 2] == 1) {right = 1; i++;}
        if(maze->count_str * 2 + 1 > point.y + 2 && maze->mass[point.y + 2][point.x] == 1) {down = 1;  i++;}
        if(point.x - 2 > 0                       && maze->mass[point.y][point.x - 2] == 1) {left = 1;  i++;}
    } else i--;

    char * mass = (char*)calloc(5, sizeof(char));
    mass[0] = i; mass[1] = top; mass[2] = right; mass[3] = down; mass[4] = left;

    return mass;
}

void out_maze(vMaze *maze){
    char c;
    int i, j;
    for(i = 0; i<(maze->count_str) * 2 + 1; i++, printf("\n"))
        for(j=0; j<(maze->count_col) * 2 + 1; j++)
            printf("%c ", c=maze->mass[i][j] > 0 ? ' ' : '*');
}

char** create_mass(int size_i, int size_j){
    int i, j;
    char **mass = (char**)malloc((size_i * 2 + 1) * sizeof(char*));
    for(i = 0; i < size_i * 2 + 1; i++)
        *(mass+i) = (char*)calloc((size_j * 2 + 1), sizeof(char));

    for(i = 0; i < size_i * 2 + 1; i++)
        for(j = 0; j < size_j * 2 + 1; j++)
            if(i&1 && j&1)
                mass[i][j] = 1;

    return mass;
}

void zero_stack(stack *st){
    st->top = -1;
}

void push(stack *st, cell pointt){
    if(st->top > size*size) {printf("Error"); return;}
    st->top++;
    st->point[st->top].x = pointt.x;
    st->point[st->top].y = pointt.y;
}

cell pop(stack *st){
    cell point, point_error;
    point_error.x = point_error.y = -1;
    if(st->top == -1 ) return point_error;
    point.x = st->point[st->top].x;
    point.y = st->point[st->top].y;
    st->point[st->top].x = -1;
    st->point[st->top].y = -1;
    st->top--;
    return point;
}
