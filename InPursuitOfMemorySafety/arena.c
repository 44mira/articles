#ifndef ARENA_ALLOCATOR
#define ARENA_ALLOCATOR
#include <stdlib.h>

struct Arena {
  void* stack;
  size_t idx;
  size_t size;
};

struct Arena* arena_create(size_t size) {
  struct Arena* arena = (struct Arena*) malloc(sizeof (*arena));
  if (arena == NULL) return NULL;

  void* stack = malloc(size);
  if (stack == NULL) return NULL;

  arena->stack = stack;
  arena->idx = 0;
  arena->size = size;

  return arena;
}

void arena_destroy(struct Arena* arena) {
  free(arena->stack);
  free(arena);
}

void* arena_alloc(struct Arena* arena, size_t size) {
  if (size + arena->idx > arena->size) return NULL;

  arena->idx += size;
  return (arena->stack + arena->idx - size);
}

#endif

#include <stdio.h>

char* uppercase(struct Arena* arena, const char* str, size_t size);

int main(void) {
  struct Arena *arena = arena_create(4096); // start of lifetime

  // all allocations made with the arena will be valid within these bounds
  
  // we can use it as a malloc substitute
  char* word = (char*) arena_alloc(arena, 16); 
  word = "Hello world!\n";
  printf("%s", word);

  // you can even pass the arena into functions
  char* uppercased_string = uppercase(arena, "hello world", 16);

  arena_destroy(arena); // end of lifetime
                        
  // all allocations made with the arena will be invalid here.

  return 0;
}
