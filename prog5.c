#include "Graph.h"
#include "List.h"
#include "PQueue.h"
#include "Dictionary.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Point {
    int x;
    int y;
} Point;

Point *create_point(int x, int y) {
    Point *p = malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    return p;
}

//Hint: The ListPtrs will store ints for bfs and a_star.
//      You need to define the dataCompare, dataPrinter and freeData functions here. 
int intComp(void* obj1, void* obj2){
    int *o1 = obj1;
    int *o2 = obj2;
    if(*o1 == *o2)
        return 0;
    return (*o1-*o2);
}
void printInt(void* data){
    int i = data;
    printf("%d\n", i);
}
int pointCompare(void* obj1, void* obj2){
    Point *p1 = obj1; //curr
    Point *p2 = obj2; //next
    if(p1->y != p2->y){  //if their y values are different then we are moving up or down
        if(p1->y > p2->y){
            return 0; //step up
        }
        else if(p1->y < p2->y){
            return 1; //step down
        }
    }
    else if(p1->x != p2->x){
        if(p1->x < p2->x){
            return 2; //step right
        }
        else if(p1->x > p2->x)
            return 3; //step left
    }
    return -1;
}
int pointComp(Point *p1, Point* p2){
    if(p1->y != p2->y){  //if their y values are different then we are moving up or down
        if(p1->y > p2->y){
            return 0; //step up
        }
        else if(p1->y < p2->y){
            return 1; //step down
        }
    }
    else if(p1->x != p2->x){
        if(p1->x < p2->x){
            return 2; //step right
        }
        else if(p1->x > p2->x)
            return 3; //step left
    }
    return -1;
}
void printPoint(void* data){
    Point* p = data;
    printf("%d %d\n", p->x, p->y);
}
void freePoint(void* data){
    free(data);
}
int comp(void* obj1, void* obj2){
    int o1 = obj1;
    int o2 = obj2;
    if(o1 == o2)
        return 0;
    return (o1-o2);
}
/**
 * @brief Implements the bst algorithm to find the shortest path between 2 nodes in an unweighted graph.
 * 
 * @param g The graph to search through
 * @param vertex_start The index of the vertex in the graph from where to start the search.
 * @param vertex_end The index of the vertex in the graph that needs to be reached.
 * @param vertices_explored The value in this pointer should be set to the number of vertices that were visited by the algorithm.
 * @return ListPtr A list of the nodes that make up the path from the start to the end, including the start and end nodes.
 */
ListPtr bfs(Graph *g, int vertex_start, int vertex_end, long *vertices_explored) {
    //Init variables including Frontier "Queue" and reached list
    int *start = (int*) malloc(sizeof (int));
    int* goal = (int*) malloc(sizeof (int));
    int count = 0;
    *start = vertex_start;
    *goal = vertex_end;
    ListPtr frontier = list_create(intComp, printInt, freePoint);
    ListPtr reached = list_create(intComp, printInt,freePoint);
    int previous[graph_num_vertices(g)];
    //our vertex_start is where we start
    previous[vertex_start] = -1;
    previous[vertex_end] = -1;
    list_append(frontier,start);
    list_append(reached,start);
    //Perform bfs traversal. Make sure to store who the parent of each node in your traversal is.
    while(list_length(frontier) != 0){
        int *curr = list_del_index(frontier,0);
        list_append(reached,curr);
        if(*curr == *goal){
            break;
        }
        ListPtr neighbors = graph_get_neighbors(g,*curr);
        for(int i = 0; i < list_length(neighbors); i++){
            int* next = list_get_index(neighbors, i);
            if(list_find_element(reached,next) == -1){
                list_append(frontier,next);
                list_append(reached,next);
                previous[*next] = *curr;
                count++;
            }
        }
    }
    //list_destroy(frontier,true);
    int currPath;
    currPath = vertex_end;
    ListPtr path = list_create(comp,printInt,freePoint);
    while(currPath != vertex_start){
        list_insert(path, 0, currPath);        //push curr to the front of path
        currPath = previous[currPath];        // how we got to curr
    }
    currPath = vertex_start;
    list_insert(path, 0, currPath);

    //*vertices_explored = list_length(reached);
    *vertices_explored = count;
    return path;
}

/**
 * @brief Implements the A* search algorithm to find the shortest path between 2 nodes in an unweighted graph.
 * 
 * @param g The graph to search through
 * @param nodeArr Array of positions of the graph vertices on a 2d plane. Note this is a pointer to a contiguous array, where nodeArr[i] should correspond to node i in the graph. 
 * @param vertex_start The index of the vertex in the graph from where to start the search.
 * @param vertex_end The index of the vertex in the graph that needs to be reached.
 * @param vertices_explored The value in this pointer should be set to the number of vertices that were visited by the algorithm.
 * @return ListPtr A list of the nodes that make up the path from the start to the end, including the start and end nodes.
 */
ListPtr a_star(Graph *g, Point *nodeArr, int vertex_start, int vertex_end, long *vertices_explored) {
    //Perform a star traversal. Make sure to store who the parent of each node in your traversal is, and the path cost up to that node.
    return NULL;
}


int main(int argc, char* argv[]) {
    int SIZE = 100;
    // argv[1] will tell you if to run bfs or a_star
    int algorithm = (strcmp(argv[1], "bfs") == 0) ? 0 : 1;
    // stdin will contain your input
    char buffer[SIZE];
    fgets(buffer,SIZE, stdin); //"======= Nodes ======="
    //Parse node locations + count number of nodes
    ListPtr nodes = list_create(pointCompare, printPoint, freePoint);
    char* stop = "======= Edges =======\n";
    while(strcmp(fgets(buffer,SIZE, stdin), stop) != 0){
        int x = atoi(strtok(buffer," "));
        int y = atoi(strtok(NULL,"\n"));
        Point* p = create_point(x,y);
        list_append(nodes, p);
    }
    // Create graph
    Graph* g = graph_create(list_length(nodes));
    //Parse and add graph edges
    stop = "======= Find paths =======\n";
    while(strcmp(fgets(buffer,SIZE, stdin), stop) != 0){
        int v1 = atoi(strtok(buffer," "));
        int v2 = atoi(strtok(NULL,"\n"));
        graph_add_edge(g,v1,v2);
    }
    //Parse start and end points and run through correct search algorithm
    int S; int E;
    long* vertExp = (long*)malloc(sizeof (long));
    ListPtr SEList = list_create(pointCompare, printPoint, freePoint);
    while(fgets(buffer,SIZE, stdin) != NULL){
        if(sscanf(buffer, "%d %d",&S,&E) != 2)
            break;
        Point* p = create_point(S,E);
        list_append(SEList, p);
    }
    if(algorithm == 0){
        for(int i = 0; i < list_length(SEList); i ++){
            int left = 0; int right = 0; int up = 0; int down = 0;
            Point* SE = (Point*)list_get_index(SEList, i);
            ListPtr bfsOut = bfs(g, SE->x, SE->y, vertExp);
            for(int i = 0; i < list_length(bfsOut); i++){
                if(i < list_length(bfsOut)-1) {
                    Point *curr = list_get_index(nodes, (int) list_get_index(bfsOut, i));
                    Point *next = list_get_index(nodes, (int) list_get_index(bfsOut, (i + 1)));
                    if(curr->y > next->y){//moving up
                        up++;
                    }
                    else if(curr->y < next->y){//moving down
                        down ++;
                    }
                    if(curr->x > next->x){//moving left
                        left ++;
                    }
                    else if(curr->x < next->x){//moving right
                        right ++;
                    }
                }

            }
            printf("%d %d %d %d %d %d\n",*vertExp, list_length(bfsOut)-1, left,right,up,down);
        }
    }
    //free Data structures
    list_destroy(nodes,true);
    list_destroy(SEList,true);
    graph_destroy(g);
    return 0;
}
