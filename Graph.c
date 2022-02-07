#include <stdlib.h>
#include <stdio.h>
#include "List.h"
#include "Graph.h"

typedef struct Graph{
    int num_vertices;
    ListPtr *adjacency_list; // Is an array of ListPtrs for faster access
} Graph;

// Hints: You will need to implement a ListPtr DataCompare, DataPrinter and 
//        freeData for the lists used in the adjacency list. These will be ints.
//        You can copy this from the ones in prog5.c, but they should have different name to avoid a namesapce collision.
int dataCompare(void* obj1, void* obj2){
    int* v1 = obj1;
    int* v2 = obj2;
    if(*v1 == *v2)
        return 0;
    else
        return 1;
}
void dataPrinter(void* data){
    int *dataInt = data;
    printf("%d ", *dataInt);
}
void freeData(void* data){
    free(data);
}
//TODO: Implement all functions from the header file.
Graph *graph_create(int numVertices){
    Graph* G = (Graph*) malloc(sizeof (Graph));
    G->num_vertices = numVertices;
    G->adjacency_list = (ListPtr *)calloc(sizeof(ListPtr), numVertices);
    for(int i = 0; i < numVertices; i++){
        G->adjacency_list[i] = list_create(dataCompare, dataPrinter, freeData);
    }
    return G;
}
//loops through each adjacency list and list_destroy() them
void graph_destroy(Graph *g){
     for(int i = 0; i < g->num_vertices; i++){
         list_destroy(g->adjacency_list[i], true);
     }
     free(g);
}
//helper function to check if two vertices are in bounds
bool inBounds(Graph *g,int v1, int v2){
 if(v1 < g->num_vertices && v2 < g->num_vertices && v1 >= 0 && v2 >= 0)
    return true;
 else
     return false;
}
//after checking if v1 and v2 are in bounds and do not already have an edge this function will create
// two int*'s and append each vertices to eachothers lists
bool graph_add_edge(Graph *g, int v1, int v2){
    if(g != NULL && inBounds(g,v1,v2) && !graph_has_edge(g,v1,v2)){
        int * node1 = malloc(sizeof (int));
        int * node2 = malloc(sizeof (int));
        *node1 = v1;
        *node2 = v2;
        list_append(g->adjacency_list[v1], node2);
        list_append(g->adjacency_list[v2], node1);
        return true;
    }
    return false;
}
//reallocates memory and fills in listptrs to the new piece of memory
bool graph_add_nodes(Graph *g, int numVertices){
    ListPtr *tmp = realloc((ListPtr *)g->adjacency_list, (g->num_vertices + numVertices) * sizeof(ListPtr));
    if(tmp != NULL) {
        g->adjacency_list = tmp;
        for(int i = g->num_vertices; i < (g->num_vertices+numVertices); i++){
            g->adjacency_list[i] = list_create(dataCompare, dataPrinter, freeData);
        }
        g->num_vertices += numVertices;
        return true;
    }
    return false;
}
//Checks if the edge exists and deletes the edge from both v1 and v2
bool graph_remove_edge(Graph *g, int v1, int v2){
    if(graph_has_edge(g,v1,v2)) {
        list_del_index(g->adjacency_list[v2],list_find_element(g->adjacency_list[v2],&v1));
        list_del_index(g->adjacency_list[v1],list_find_element(g->adjacency_list[v1],&v2));
        return true;
    }
    return false;
}
//checks whether the two points are in bounds then checks the adjacency list at the first vertex if it contains v2
bool graph_has_edge(Graph *g, int v1, int v2){
    if(g != NULL && inBounds(g,v1,v2)) {
        if (list_find_element(g->adjacency_list[v1], &v2) != -1) {
            return true;
        }
    }
    return false;
}
//Given a graph and a certain vertex, returns the listptr at that vertex which contains all its neighbors
ListPtr graph_get_neighbors(Graph *g, int vertex){
    if(g->adjacency_list[vertex] != NULL && vertex >= 0 && vertex < g->num_vertices){
        return (ListPtr)g->adjacency_list[vertex];
    }
    return NULL;
}
//returns num_vertices
int graph_num_vertices(Graph *g){
    if(g != NULL)
        return g->num_vertices;
    return -1;
}
//adds up the lists in graph which contain the number of edges and returns that number /2 since there is
//duplicate edge insertion
int graph_num_edges(Graph *g){
    if(g != NULL){
        int edges = 0;
        for(int i = 0; i < graph_num_vertices(g); i++){
            edges += list_length(g->adjacency_list[i]);
        }
        return edges/2;
    }
    return -1;
}
