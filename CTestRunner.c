#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdint.h>

/* Include correct data structure header */
#include "Graph.h"

#define FIRST_TEST Empty_length

static uint8_t disable_exit_handler;
jmp_buf test_crash;
int saved_stdout;
int devNull;

/* Define tests */
enum Test_e {
  Graph_size = 0,
  Free_test_1,
  Insert_edge_has_edge_1,
  Insert_edge_has_edge_2,
  Insert_edge_edge_count_1,
  Insert_edge_edge_count_2,
  Has_edge_empty,
  Add_nodes_1,
  Delete_edge_has_edge_1,
  Delete_edge_edge_count_1,
  Graph_neighbors_1,
  
  NUM_TESTS,
};

bool any_char(char ch, char *charArr) {
    while (*charArr != 0) {
        if (ch == *charArr++) {
            return true;
        }
    }
    return false;
}


/* Define test names */
char *testName(int test) {
  if (test == Graph_size) return "Graph_size";

  if (test == Free_test_1) return "Free_test_1";

  if (test == Insert_edge_has_edge_1) return "Insert_edge_has_edge_1";
  if (test == Insert_edge_has_edge_2) return "Insert_edge_has_edge_2";

  if (test == Insert_edge_edge_count_1) return "Insert_edge_edge_count_1";
  if (test == Insert_edge_edge_count_2) return "Insert_edge_edge_count_2";

  if (test == Has_edge_empty) return "Has_edge_empty";
  
  if (test == Add_nodes_1) return "Add_nodes_1";

  if (test == Delete_edge_has_edge_1) return "Delete_edge_has_edge_1";

  if (test == Delete_edge_edge_count_1) return "Delete_edge_edge_count_1";

  if (test == Graph_neighbors_1) return "Graph_neighbors_1";

  return "";
}

char *standardize_output(char *inString) {
  char *outString = calloc(sizeof(char), strlen(inString));
  size_t outOffset = 0;
  char c;
  char prevChar = ' '; //Ignore all leading space
  while ((c = *inString++) != 0) {
    if (any_char(c, " \t\n")) {
      if (prevChar != ' ') {
        outString[outOffset++] = ' ';
        prevChar = c;
      }
    } else {
      outString[outOffset++] = c;
      prevChar = c;
    }
  }
  
  if (outOffset > 0 && prevChar == ' ') {
   
    outString[outOffset - 1] = 0; // Cut off last space
  } else {
    outString[outOffset] = 0; // Add end of string
  }
  return outString;
}


uint8_t runTest(int test) {
//   printf("case %s\n", testName(test));
  // Declare necessary data structure
  // ListPtr A = newList(&cmpData, &printData, &free); // *pA
  
  int out_pipe[2]; // Set pipes to intercept stdout
  switch(test) {
    case Graph_size: { // Declare tests
      Graph *g = graph_create(3);
      if (graph_num_vertices(g) != 3) return 1;
      if (graph_num_edges(g) != 0) return 2;
      return 0;
    }
    case Free_test_1: { // Declare tests
      //Ensure works on empty dictionary
      Graph *g = graph_create(3);
      graph_destroy(g);
      return 0;
    }
    case Insert_edge_has_edge_1: { // Declare tests
      
      Graph *g = graph_create(15);

      if (!graph_add_edge(g, 0, 1)) return 1;

      if (!graph_has_edge(g, 0, 1) || !graph_has_edge(g, 1, 0)) return 2;
      return 0;

    }
    case Insert_edge_edge_count_1: { // Declare tests
      
      Graph *g = graph_create(15);

      if (!graph_add_edge(g, 0, 1)) return 1;

      if (graph_num_edges(g) != 1) return 2;
      return 0;
    }
    case Insert_edge_has_edge_2: { // Declare tests
      
      Graph *g = graph_create(15);

      if (!graph_add_edge(g, 1, 2)) return 1;
      if (!graph_add_edge(g, 1, 3)) return 2;
      if (!graph_add_edge(g, 1, 4)) return 3;
      if (!graph_has_edge(g, 1, 2) ||
          !graph_has_edge(g, 1, 3) ||
          graph_has_edge(g, 2, 3) ||
          !graph_has_edge(g, 2, 1) ||
          !graph_has_edge(g, 3, 1) ||
          !graph_has_edge(g, 4, 1) ||
          !graph_has_edge(g, 1, 4) ||
          graph_has_edge(g, 3, 2)) return 4;
      return 0;
    }
    case Insert_edge_edge_count_2: { // Declare tests
      
      Graph *g = graph_create(15);

      if (!graph_add_edge(g, 1, 2)) return 1;
      if (!graph_add_edge(g, 1, 3)) return 2;
      if (!graph_add_edge(g, 1, 4)) return 3;

      if (graph_num_edges(g) != 3) return 4;
      return 0;
    }

    case Has_edge_empty: { // Declare tests
      Graph *g = graph_create(15);

      if (graph_has_edge(g, 0, 2)) return 1;
      if (graph_has_edge(g, 1, 2)) return 2;
      if (graph_has_edge(g, 1, 1)) return 3;
      if (graph_has_edge(g, 1, 14)) return 4;
      if (graph_has_edge(g, -3, 15)) return 5;

      return 0;
    }
    case Add_nodes_1: { // Declare tests
      Graph *g = graph_create(10);

      if (graph_num_vertices(g) != 10) return 1;

      graph_add_nodes(g, 3);

      if (graph_num_vertices(g) != 13) return 2;
      return 0;
    }
    
    case Delete_edge_has_edge_1: { // Declare tests
      Graph *g = graph_create(10);
      graph_add_edge(g, 0, 5);
      graph_add_edge(g, 9, 5);
      graph_add_edge(g, 1, 2);
      graph_add_edge(g, 1, 3);
      graph_add_edge(g, 2, 3);
      graph_add_edge(g, 4, 8);
      if (!graph_remove_edge(g, 2, 1)) return 1;
      if (graph_has_edge(g, 1, 2)) return 2;
      return 0;
    }
    case Delete_edge_edge_count_1: { // Declare tests
      Graph *g = graph_create(10);
      graph_add_edge(g, 0, 5);
      graph_add_edge(g, 9, 5);
      graph_add_edge(g, 1, 2);
      graph_add_edge(g, 1, 3);
      graph_add_edge(g, 2, 3);
      graph_add_edge(g, 4, 8);
      if (!graph_remove_edge(g, 2, 1)) return 1;
      if (graph_num_edges(g) != 5) return 2;
      return 0;
    }
    
    case Graph_neighbors_1: { // Declare tests
      Graph *g = graph_create(6);
      intptr_t edges[] = {2, 3, 4, 5};
      graph_add_edge(g, 1, 2);
      graph_add_edge(g, 1, 3);
      graph_add_edge(g, 1, 4);
      graph_add_edge(g, 1, 5);
      ListPtr neighborsList;
      if (graph_get_neighbors(g, -1) != NULL) return 1;
      if (graph_get_neighbors(g, 6) != NULL) return 2;
      if ((neighborsList = graph_get_neighbors(g, 1)) == NULL || list_length(neighborsList) != 4) return 3;
      for (int i = 0; i < 4; i++) {
        if (list_find_element(neighborsList, &edges[i]) < 0) return 4;
      }

      if ((neighborsList = graph_get_neighbors(g, 5)) == NULL || list_length(neighborsList) != 1) return 5;
      if ((*(int*)list_get_index(neighborsList, 0)) != 1) return 6;
      return 0;
    }

  }
  return 255;
}

void exit_attempt_handler(void) { // only I decide when you are done
  if (disable_exit_handler) return; // allow this to be disabled
  exit(30);
}


int main (int argc, char **argv) {

  if (argc > 2 || argc  <= 1) {
    printf("Usage: %s [<index_of_test>|-tests]\n", argv[0]);
    exit(51);
  }
  if (strcmp(argv[1], "-tests") == 0) {
    for (int i = 0; i < NUM_TESTS; i++) {
      printf("%d %s\n", i, testName(i));
    }
  } else {
    int test_index = atoi(argv[1]);
    if (test_index < 0 || test_index >= NUM_TESTS) {
      printf("Test index is out of bounds");
      exit(50);
    }

    saved_stdout = dup(1);
    devNull = open("/dev/null", O_WRONLY);
    atexit(exit_attempt_handler);
    
    disable_exit_handler = 0;
    int test_output = runTest(test_index);
    disable_exit_handler = 1;
    exit(test_output);
  }
  
  
}
