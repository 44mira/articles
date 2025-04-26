# In defense of `union` and `goto`

Like mentioned in my previous video, memory safety has been an issue since the dawn of programming, which is the main reason why an entire category of *memory safe* languages exist in the first place.

And now, there\'s currently a big push by the American government, and the FBI in particular, to move away from memory unsafe languages or produce a memory safety roadmap for existing government software that use these aforementioned languages.

Of course, entirely migrating to a different language especially when most of the existing software is written in languages like C/C++, is a monumental task. Which is why many people have taken it into their own hands the responsibility of *extending* C/C++ to be *safer*, or entirely memory safe, instead of rewriting their entire codebase.

One of these attempts in particular caught my attention, *TrapC*.

## Undefined behavior

Before even reading the rest of the language spec you are met with its title: **\"TrapC: Memory Safe C Programming with No UB\"**

Although I agree that undefined behavior is quite annoying, we must not forget that C\'s edge on performance *hinge* on undefined behavior existing! Having to not consider entire branches of logic (i.e. accessing uninitialized arrays, null checking every pointer dereference), lets the compiler perform absolutely heinous optimizations, as long as you don\'t actually invoke the undefined branches.

This is in theme with the rest of the language\'s design principles, that is to sacrifice these safety checks for a staggering increase in its efficiency and the ability to get away with the fastest implementation for target architectures. The compiler offloads this responsibility to the programmer.

And if the use case demanded for C in the first place, specifically for realtime software, you should *NOT* be willing to take that performance hit.

But let\'s give `TrapC` the benefit of the doubt that maybe it is worth to do this change for the sake of memory safety.

## `union` and `goto`

> TrapC removes 2 keywords: 'goto' and 'union', as unsafe and having been widely deprecated from use

Now to say that this change is outrageous is an understatement. It is *harmful* and outright *offensive* to the language itself. People who don\'t know better would actually believe these statements as it comes from the false authority of a language proponent.

Unions have never been deprecated, and `goto` still has its use cases, such as greatly improving readability and reducing code duplication for errorhandling and escaping nested loops.

\"So we tried C, we didn\'t understand why certain features existed or they weren\'t necessary for our use case, so we\'ve decided that everyone else *also* doesn\'t know how to use it and therefore it shouldn\'t exist for all other use cases.\"

In fact, instead of just ragging this completely arrogant change, let\'s go over how you can properly use these language features, specifically union.

## Unions

For those who are unfamiliar, unions are a type construct which holds attributes that share the same memory, the alignment of the union being the largest type in it.

```c
struct IntOrFloatOrBool {
  int n;   // 4 bytes
  float f; // 4 bytes
  bool b;  // 1 byte
};

// [ i i i i ] [ f f f f ] [ b _ _ _ ]
// stored in memory with 12 bytes!

union IntOrFloatOrBool {
  int n;    // 4 bytes
  float f;  // 4 bytes
  bool b;   // 1 byte
};

// [ x x x x ]
// stored in memory with 4 bytes.

```

You can think of alignment as the size a type assumes. This is mostly influenced by the largest type in a struct, as we cannot store types with differing alignments. Which means our smaller types get padded by *garbage* data to fulfill the alignment requirements of a struct. Of course this issue of wasted memory in padding only gets worse with types that have attributes of a bigger size, and when storing them in arrays (which require elements of the same alignment)

```c
struct Foo {
  int bar;      // 4 bytes
  double baz;   // 8 bytes
  bool qux;     // 1 byte
};

/* 
  [ i i i i | _ _ _ _ ] 
  [ d d d d | d d d d ] 
  [ b _ _ _ | _ _ _ _ ]

  stored in 24 bytes! 
  (16 if arranged such that double comes first)
*/

union Foo {
  int bar;      // 4 bytes
  double baz;   // 8 bytes
  bool qux;     // 1 byte
};

/* 
  [ x x x x | x x x x ] 

  stored in 8 bytes!
*/
```

By itself, unions struggle to see any practical application outside of programs that concern themselves with bitwise operations. As it turns out, being able to reinterpret values as other types does offer clever ways of doing bitwise operations.

## Tagged unions

But if we really think about it, using a union\'s field usually entails that we only care about that interpretation for that specific variable. That is, if we define a union type and access an `int` field, we would only ever want to do operations on it that we could do on an `int`.

We can name this behavior as unions having an *active* state. If we access a union as a specific type during an assignment, we would only ever want to access it as that type.

```c
typedef union {
  int rotations;
  float degrees;
} Rotation;

Rotate angle = { .degrees = 0.45f };  // float as the active type
Rotate cycles = { .rotations = 0.4 }; // int

```

You\'ll find that thinking of unions as a *variant* of its active state allows us to use unions as how would probably expect type unions to be used.

```c
/**
 * Animate a rotation on an `object`.
 *
 * If `rotation` is a float, rotate by that angle in degrees.
 * If it is an integer, do that number of full rotations. (Negative for clockwise)
 */
void object_rotate(Object *obj, Rotate rotation);
```

Now the issue becomes: how do we know what an instance of a union\'s active type is? And the answer is consistent with the mindset that you should have if you\'ve decided to use the language- you implement it yourself!

But if you've tinkered around with C before, you should notice that this is a very familiar pattern.

```c
/**
 * Return the sum of n-length list.
 */
int sum(int nums[], unsigned int n);
```

- `nums` *loses* the size information when passed as an argument, which means that we have to pass in the size as a separate argument.
- The workaround to this is *coupling* the size information with the array, using a `struct`.

```c
typedef struct {
  int* items;
  unsigned int size;
} IntArray;

/**
 * Return the sum of a list.
 */
int sum(IntArray nums);
```

We can couple the union type in a struct with an attribute that tells us what the union\'s active type is. Moreover, since we only really need some representation to check what a union\'s active type is, we can use an enumerated type (`enum`) for this, increasing its readability and makes the compiler check whether switch cases on that type are exhaustive when you turn on warnings.

So we observe that the main type constructs of the language interact harmoniously to create a feature than you probably might be taking for granted in higher level languages, **type polymorphism**.
- Or the ability to do separate code branches depending on the type of a variable, without having to resort to hacks such as `void` pointers.

```c
enum RotateType {
  FULL,
  ANGLE,
};

union _Rotate {
  int rotations;
  float angle;
};

typedef struct {
  enum RotateType type;
  union _Rotate unwrap;
} Rotate;
```

In fact, this type of structure has a name, a **tagged union**. The tag here is our extra attribute on our wrapper that determines the union\'s active type.

Tagged unions can be seen in various modern languages, most prominently in Rust (where they are referred to as Enums), Zig, and languages that heavily emphasize pattern matching.

Applications of tagged unions include but are not limited to:

- the `Result<T,E>` type, which in of itself enables the concept of passing \"errors as values,\" which is behavior that is way easier to deal with than throwing exceptions from anywhere
- the `Option<T>` type, which allows encapsulation of null values.
  - Interestingly enough, we don\'t actually need a union for this type, but the pattern of having a tag dictate the active type makes it still fall into this category.
- Lexical tokens `Token`, for writing leaner parsers that allow for compiletime type checks on your Token variants.
- and other use cases that require *alternation* over types. Think of tagged unions as being able to \"boolean or\" together types, while structs let you \"boolean and\" them.

## More on C's tagged unions

But in C, using the type we've declared is *not* ergonomic at all.

```c
// A Rotate ANGLE variant with value 90.0 degrees
Rotate right_angle = { .type = ANGLE, .unwrap = (union _Rotate) { .angle = 90.0f }};
// A Rotate FULL variant with value 2 rotations
Rotate doubleturn = { .type = FULL, .unwrap = (union _Rotate) { .rotations = 2 }};
```

The answer? **Function macros**, or as I like to call them, wizardry.

> Macros should never be your first solution when it comes to problems you encounter in your code. You should only ever consider it when you need actual *code* to be written for convenience sake.

One way we could tackle this ergonomic issue is by storing the variant type *out-of-band*.
- Instead of declaring it as a value, we *encode* that idea in the macro's name itself.

```c
#define Rotate_FULL(_degrees) \
  ((Rotate) { .type = FULL, .unwrap = (union _Rotate) { .rotations = (_degrees) }})

#define Rotate_ANGLE(_angle) ((Rotate) { \
  .type = ANGLE,                         \
  .unwrap = (union _Rotate) {            \
    .angle = (_angle)                    \
  }                                      \
})
```

So now all we have to do to create instances of these unions are:

<!-- pause -->
```c
Rotate right_angle = Rotate_ANGLE(90.0f);
Rotate doubleturn = Rotate_FULL(2);
```

Or a much more explicit alternative, is to make your macro take in the variant and an anonymous union that corresponds to the type specified by the variant.

```c
#define Rotate(_variant, _union) \
  ((Rotate) { .type = _variant, .unwrap = (union _Rotate) _union })

Rotate halfturn = Rotate(ANGLE, { .rotations = 180.0f });
```

This version is more applicable if your tagged union has more variants than macros you are willing to write, at the cost of being more verbose in creating instances. *(Do note that there is no compatibility checking done with the value and the variant!)*

Here's how you would go about accessing those fields.

```c
union _Rotate unwrapped = doubleturn.unwrap;
switch (doubleturn.type) {
  case FULL:
    printf("Rotate 360deg %d times.", unwrapped.rotations);
    break;
  case ANGLE:
    printf("Rotate %.2fdeg.", unwrapped.unwrap.angle);
}

Rotate rotations[] = {
  Rotate_FULL(4),
  Rotate_ANGLE(45.0f),
  Rotate_FULL(0)
};
```

And for a much more interesting example, macros also let you (sort of) work with generics, by utilizing its preprocessed text concatenation using double hashtags `##`.

Here's an implementation of the generic `Result<T,E>` type as found in languages like Rust.

```c
enum ResultVariant { RESULT_OK, RESULT_ERR };

// the Error type to be used for all Results.
struct Error {
  unsigned int code;
  const char* message;
};

// a macro for creating a Result implementation for some arbitrary type T =====
#define deriveResult(T)         \
  union _Result_##T {           \
    struct Error err;           \
    int ok;                     \
  };                            \
  struct Result_##T {           \
    enum ResultVariant type;    \
    union _Result_##T unwrap;   \
  };

// shorthands for result instances, takes in the type of the Ok variant.  =====
#define Result_Err(ok_type, _code, _message) \
  ((struct Result_##ok_type) {               \
    .type = RESULT_ERR,                      \
    .unwrap = (union _Result_##ok_type) {    \
      .err = {                               \
        .code = (_code),                     \
        .message = (_message),               \
      }                                      \
    }                                        \
  })

#define Result_Ok(ok_type, _value)           \
  ((struct Result_##ok_type) {               \
    .type = RESULT_OK,                       \
    .unwrap = (union _Result_##ok_type) {    \
      .ok = (_value)                         \
    }                                        \
  })
// ============================================================================

// create Result_int
deriveResult(int);

// errors on negative params
struct Result_int area_rectangle(int l, int w) {
  if (l < 0 || w < 0) {
    return Result_Err(int, 1, "invalid dimensions");
  }

  return Result_Ok(int, l * w);
}

int main(void) {
  int l, w;
  char buffer[BUFSIZ];

  printf("input length and width:\n");
  fgets(buffer, BUFSIZ, stdin);
  sscanf(buffer, "%d %d", &l, &w);

  struct Result_int area = area_rectangle(l,w);

  // match on the Result variant
  union _Result_int unwrapped = area.unwrap;
  switch (area.type) {
    case RESULT_ERR:
      printf("ERROR: %s (Code %u)\n", unwrapped.err.message, unwrapped.err.code);
      return area.unwrap.err.code;
    case RESULT_OK:
      printf("The area is: %d\n", unwrapped.ok);
  }

  return 0;
}
```

## `goto`

Though I won\'t be covering goto as in depth in this video (as they already do pretty much what you expect them to do, and in a manner that you also expect to do it with), know that I do share this same sentiment with it.

Edsger Dijkstra, a person who you should probably already know 1-2 years in any computer science related degree, has an article talking about when goto statement is considered harmful, entitled \"Goto statement is considered harmful.\" And the main point he was getting at in said paper is the importance of being able to reason about your program as it grows dynamically in complexity, by knowing the sequence of instructions to get back to some point of a process after being stopped by some arbitrary action?
- Or to put simply, how do you get back to a point in your program by tracing the code sequentially?

Now I believe that his article, along with this idea of `goto` being harmful has been terribly lifted out of context and echoed throughout the pedagogy of programming. That is \"do not use `goto`, ever\".

We should, as critical thinkers, be able to discern when ideas and principles are true, especially after factoring in context. `goto`, and any language feature for that matter, when thought of in a vacuum, should neither be good or bad. `goto` becomes bad when it obscures our ability to reason about the dynamic nature of our program, and good when it improves it.

In fact, it is so useful for handling exit cases of functions that with one quick lookup, you can see that `goto` is found all over the Linux kernel. Point is, if `goto` doesn\'t hinder your ability to trace your program, even if there\'s a more idiomatic way of achieving your desired behavior, it is not a bad use of it.

```c
static int __init button_init(void)
{
        int error;

        if (request_irq(BUTTON_IRQ, button_interrupt, 0, "button", NULL)) {
                printk(KERN_ERR "button.c: Can't allocate irq %d\n", button_irq);
                return -EBUSY;
        }

        button_dev = input_allocate_device();
        if (!button_dev) {
                printk(KERN_ERR "button.c: Not enough memory\n");
                error = -ENOMEM;
                goto err_free_irq;
        }

        button_dev->evbit[0] = BIT_MASK(EV_KEY);
        button_dev->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0);

        error = input_register_device(button_dev);
        if (error) {
                printk(KERN_ERR "button.c: Failed to register device\n");
                goto err_free_dev;
        }

        return 0;

err_free_dev:
        input_free_device(button_dev);
err_free_irq:
        free_irq(BUTTON_IRQ, button_interrupt);
        return error;
}
```

## Conclusion

If C were to have any warts as a language, I do not believe it is with `union` or `goto`. I do not think it's perfect, especially when it comes to a lack of namespaces, the lack of a dedicated build system, and a substandard standard library (i.e. `gets`), but none of these flaws stop it from being an incredibly versatile language and one that makes you feel like a *real* programmer after writing a functioning program in it.

In fact, `union` and `goto` being as open-ended as they are provide the biggest opportunity for clever programmers to create ingenious solutions to their problems, and if you've programmed even the littlest bit, you'd know that's the most *fun* part of all of it.

Enjoy programming.
