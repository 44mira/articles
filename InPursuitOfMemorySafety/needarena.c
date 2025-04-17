#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_NEIGHBOR_COUNT 10

struct Node;
struct Node {
  char* name;
  struct Node* neighbors;
  size_t neighbor_count;
} Node;

struct Node* newNode(const char* _name) {
  struct Node* node = (struct Node*) malloc (sizeof (*node));
  char* name = (char*) malloc (MAX_NAME_LENGTH);
  struct Node* neighbors = (struct Node*) malloc ((sizeof *neighbors) * MAX_NEIGHBOR_COUNT);

  strncpy(name, _name, MAX_NAME_LENGTH);
  
  node->name = name;
  node->neighbor_count = 0;
  node->neighbors = neighbors;

  return node;
}

int main(void) {
  const char name[] = "Node 1";
  struct Node* node = newNode(name);

  printf("%s\n", node->name);

  free(node->name);
  free(node->neighbors);
  free(node);

  return 0;
}
