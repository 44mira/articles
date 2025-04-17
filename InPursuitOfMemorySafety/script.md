---
title: "in pursuit of **memory safety**"
sub_title: lifetimes, RAII, and arenas
author: "44mira"
# theme:
#   override:
#     code:
#       theme_name: gruvbox-dark
---

Memory Safety
==

<!-- font_size: 2 -->
```c +line_numbers {all|5}
#include <ctype.h>
#include <stdlib.h>

char* uppercase(const char* str, size_t buffSize) {
  char* result = (char*) malloc (buffSize);

  for (int i = 0; str[i] != '\0'; i++) {
    result[i] = toupper(str[i]);
  }

  return result;
}
```

<!-- end_slide -->

<!-- jump_to_middle -->
The relationship between /allocation/ and /de-allocation/
==

<!-- pause -->
<!-- font_size: 2 -->

<!-- alignment: center -->
> Trying to maintain this mindset is incredibly tideous and error-prone!

<!-- end_slide -->

The relationship between /allocation/ and /de-allocation/
==

<!-- font_size: 2 -->
<!-- new_lines: 5 -->
<!-- incremental_lists: true -->
- Too *granular*
  - Only **really** works for smaller scopes
- Too *simplified*
- Should be abstracted out
  - Achieve memory safety <span style="color:green">*by design*</span>.

<!-- end_slide -->

<!-- jump_to_middle -->
<!-- font_size: 2 -->
It has never been about allocation and de-allocation *specifically*.

<!-- pause -->
but <span style="color:green">*lifetimes.*</span>

<!-- end_slide -->

Lifetimes
==

<!-- font_size: 2 -->

<!-- new_lines: 4 -->
You'll find it becomes significantly easier to grasp the concept of
memory safety by thinking of <span style="color:red">*when*</span> should your references be valid, and 
<span style="color:green">*grouping together*</span> references that share the same lifetimes, instead of
*trying to think of them* <span style="color:red">*individually*</span>.


<!-- end_slide -->

Lifetimes
==

<!-- font_size: 2 -->
<!-- new_line -->
```c +line_numbers
char* uppercase(const char* str, size_t buffSize) {
  char* result = (char*) malloc (buffSize);

  int i;
  for (i = 0; str[i] != '\0'; i++) {
    result[i] = toupper(str[i]);
  }
  result[i] = '\0';

  return result;
}
```

<!-- end_slide -->

Lifetimes
==

```file +exec +line_numbers
path: test.c
language: c
```

<!-- end_slide -->

<!-- jump_to_middle -->
*Group together* references that share the same lifetimes.
==

<!-- end_slide -->

Structs
==

- A very common example of this would be when it comes to allocating resources for attributes of a struct.

<!-- pause -->

<!-- font_size: 1 -->
<!-- column_layout: [3,2] -->
<!-- column: 0 -->
```bash +exec_replace
bat -p --color=always -n --theme=TwoDark -S -r 8:27 needarena.c -H 16:18
```
<!-- column: 1 -->
```bash +exec_replace
bat -p --color=always -n --theme=TwoDark -r 29: needarena.c -H 35:37
```

<!-- reset_layout -->

<!-- end_slide -->

<!-- jump_to_middle -->
RAII (Resource acquisition is initialization)
==

<!-- end_slide -->
RAII (Resource acquisition is initialization)
==

<!-- column_layout: [1,1] -->
<!-- column: 0 -->
```file
path: raii.cpp
language: cpp
```
<!-- column: 1 -->
```file
path: raii/src/main.rs
language: rust
```

<!-- reset_layout -->

<!-- end_slide -->
RAII (Resource acquisition is initialization)
==

<!-- new_lines: 3 -->
```file
path:  raii2.cpp
language: cpp
```

<!-- end_slide -->

<!-- font_size: 2 -->

<!-- jump_to_middle -->
<span style="color: green">*Object*</span> lifetimes are simpler to manage than
<span style="color: red">*individual*</span> lifetimes.

<!-- end_slide -->

Ownership and Borrowing
==

<!-- new_lines: 3 -->

<!-- font_size: 2 -->
Rust's memory safety can actually be attributed to it enforcing essentially a *stricter* version of *RAII*
with the following rules:

<!-- incremental_lists: true -->
1. Each value has an <span style="color: green">*owner*</span>.
2. There can only be <span style="color: green">*one*</span> owner at a time.
3. When the owner goes <span style="color: red">*out of scope*</span>, the value is <span style="color:red">*dropped*</span>.

These are enforced by their *compile-time borrow checker*.

- C makes it easy to shoot yourself in the foot.
- C++ makes it harder, but when you do it blows your whole leg off.
- Rust straight up takes your entire arm off.

<!-- end_slide -->

<!-- font_size: 2 -->
<!-- new_lines: 7 -->
> But objects aren't a C feature!
<!-- pause -->
<!-- new_lines: 2 -->
  *RAII* isn't the <span style="color:red">*entire*</span> answer, but it does show us the *pattern* we can
take advantage of to get memory safety, <span style="color: green">*by design*</span>.

<!-- end_slide -->

A Review
==

<!-- incremental_lists: true -->

<!-- font_size: 2 -->
<!-- new_lines: 2 -->
- `malloc()` and `free()` are <span style="color:green">*generalized*</span> interfaces over memory.
  - <span style="color:green">*Portable*</span>, and applicable to most usecases.
  - Struggles to stay reasonable in scenarios that require <span style="color:red">*specialization*</span>.
<!-- new_line -->
- It's fair to say that *RAII* is a <span style="color:green">*specialization*</span> of this interface.
  - We *limit* the interface in how it deals with the symmetry of lifetimes.
    - That is, by <span style="color:red">*only*</span> allowing the references to be valid <span style="color:red">*while the object is valid*</span>.

<!-- end_slide -->

<!-- font_size: 2 -->
<!-- new_lines: 4 -->
Objects aren't what makes *RAII* work! 

<!-- pause -->
It's the idea of <span style="color:green">*setting regions*</span> in 
your code in which groups of references can be expected to stay in, their
<span style="color:green">*shared lifetime*</span>.

<!-- pause -->
They should be <span style="color:green">*valid*</span> within these bounds, 
and <span style="color:red">*invalid*</span> outside.

<!-- pause -->
These *regions* have manifested in *RAII* as the lifetime of an object, dictating
the part of code in which its allocated resources are also valid.

<!-- end_slide -->

<!-- font_size: 2 -->
<!-- jump_to_middle -->
And now, we find ourselves at the *precipice* of human ingenuity.

<!-- end_slide -->

<!-- font_size: 5  -->
<!-- new_lines: 3 -->

Arena Allocators
==

<!-- font_size: 2 -->
<!-- alignment: center -->
or simply, <span style="color:green">*Arenas*</span>.

<!-- end_slide -->

<!-- font_size: 2 -->

<!-- incremental_lists: true -->

<!-- new_lines: 2 -->
## So here's the plan:

- We prepare a big buffer using `malloc()`, and our future allocations go in this pre-allocated buffer. This is going to be the <span style="color:green">*start*</span> of our valid region for this lifetime.

- Whenever we need to allocate resources, instead of using `malloc()`, we use a function we define to <span style="color:green">*push*</span> our required memory onto our buffer.

- When we deem that the lifetime should be over, we simply `free()` the big buffer itself. This is the <span style="color:green">*end*</span> of our valid region.

Not only does this approach emancipate us from the forced *symmetry* of `malloc()` and `free()`,
it is also much more efficient CPU-wise to <span style="color:green">*free memory in bulk*</span>.

<!-- end_slide -->

Arena Allocators
==

```c +line_numbers {all|3-7|9-21|10-11|13-14|16-20}
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
```

<!-- end_slide -->

Arena Allocators
==

<!-- column_layout: [1,5,1] -->
<!-- column: 1 -->

```c +line_numbers {all|2|0}
void* arena_alloc(struct Arena* arena, size_t size) {
  if (size + arena->idx > arena->size) return NULL;

  arena->idx += size;
  return (arena->stack + arena->idx - size);
}
```

```c +line_numbers {0|all|4|8-11|13-14|16|all}
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
```

<!-- end_slide -->

Arena Allocators
==

<!-- font_size: 2 -->

Nothing is stopping you from <span style="color:green">*further specializing*</span>
this arena for your usecase!

# Examples include:

<!-- incremental_lists: true -->
- Allowing for popping topmost allocation
- Allowing for popping <span style="color:green">*anywhere*</span> in the allocated memory
  - You will have to implement your own *defragmentation* algorithm
  - Most of the time that is overkill and linear allocation is good enough
- You can declare a `static` arena
  - An arena with the same lifetime as your entire program
- You can declare *scratch* arenas
  - Function-scoped arenas that are used for temporary allocations
- So much more! 
  - (I think)

<!-- end_slide -->

First-class Arenas
==

```file +line_numbers {all|10-11|13|5-7,16|all}
path: test.zig
language: zig
```

```bash +exec
zig run ./test.zig
```

<!-- end_slide -->

<!-- font_size: 2 -->
<!-- new_lines: 8 -->
```c
  free(presentation);
```

<!-- end_slide -->

<!-- jump_to_middle -->
thanks for watching!
==
