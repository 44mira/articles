> This is part 4 of a series of articles entitled *Functional Patterns*.
>
> Make sure to check out the rest of the articles!
>
> 1. [The Monoid](https://dev.to/if-els/functional-patterns-the-monoid-22ef)
> 2. [Compositions and Implicitness](https://dev.to/if-els/functional-patterns-composition-and-implicitness-4n08)
> 3. [Interfaces and Functors](https://dev.to/if-els/functional-patterns-interfaces-and-functors-359e)

# Functional Patterns: Recursion and Reduces

## State

A big part of what defines functional programming— is the *immutability* of **state**. Because via this, we can immediately guarantee *idempotence*, a strict mapping of our input to output, just like in Mathematics.

*Idempotence* or *referential transparency* is just a fancy way of saying: If you give me `x`, I will always give you `y`. Moreover, referential transparency specifically, says that you can replace all *occurrences* of your function with the actual function body and your code should be functionally the same.

```py
# python
def double(n):
    return n * 2

double(n)

# should be replaceable by

n * 2
```

At first glance you might probably go: "Well, duh, a function call is pretty much substituting code."

But this is sometimes not true, specifically when your function has **side-effects**.

A side-effect is state *mutated* (or changed) outside of the scope of the function.

Example:

```python
counter = 0

def count():
    global counter
    counter += 1
    return counter

count() # 1
count() # 2
count() # 3
```

Even though we're calling `count` with the same argument (nothing!), we end up with different output! This is because this function has a *side-effect* of updating the state of `counter`, which is outside of its scope. Therefore, this function is not *purely* functional.

This *immutability* of state allows us to think in a simpler, and more *pure* way of avoiding side-effects altogether. And in this design do we find that, already, a lot of bugs start becoming impossible by *design* and that is truely something to give merit to functional programming for.

However, this does not come without its consequence (as it is a constraint, after all). But like any other constraint in functional programming, there is an elegant workaround to it.

In this article we'll talk about *iteration*, in the pure functional sense, and realizing how overrated `for`-loops are.

## Recursion

A problem with the aforementioned constraint of immutability, is that some code structures inherently *rely* on state, such as the `for`-loop.

```c
for (int i = 0; i < 10; i++) {
    // ...
}
```

Here's a ubiquitous `for`-loop construct. We declare some state `i`, our counter, and then we increment it on every iteration until we fail our condition. Looks good, works pretty well. But the problem lies in how we're relying on mutable state to ensure idempotent behavior.

```c
for (int i = 0; i < 10; i++) {
    i--;
}
```

When we modify the logic inside of the `for`-loop (not modifying the `for`-loop itself), we can introduce bugs such as this one that creates an infinite loop. All because our iteration relies on mutable state.

You might be thinking: "Okay, so for-loops are evil, how are we going to do iterations now?"

Okay no more assumptions on what you're thinking, you're likely more clever than that (or you've read the section header). Here's the answer: Every `for`-loop can be written **recursively**.

```c
void loop(const int i) {
    if (i >= 10) return;

    loop(i+1)
}
```

Oh yeah, computer science! Though significantly more code in an imperative sense, note that it's no longer possible to create the same bug we had in the `for`-loop without modifying the recursive construct itself, and in that case, skill issue.

Here's that expression in both Haskell and Elixir, which are both functional languages:

```hs
loop n
  | n >= 10 = undefined
  | otherwise = loop n+1
```

```elixir
def loop(n) when n >= 10, do: nil
def loop(n), do: loop n+1
```

So much cleaner (and the only way to do standard iteration in these languages).

Do note that although these do the same amount of iteration, they don't actually equal each other exactly. And that's because most `for`-loops rely on mutable state to be useful. The reason you're probably going to be looping 10 times is imperative, or introduces some side-effect (like printing to standard output). And to make up for that, functional recursions will always have you returning something, the reason you did recursion in the first place, no side-effects.

## The Call Stack

If you have ever used recursion before, you will probably be aware of its biggest drawback when compared to iterative constructs, and that is— the **call stack**.

Every time you enter a function, your program keeps track of it by putting it on the call stack as a *stack frame*. And when it's done, it simply has to pop that off, and now you're back to the function that called it, and again and again, until you get back to your `main` function.

This poses as a constraint as maintaining the call stack takes significantly more memory than just keeping track of a counter for iteration. In fact, this leads us to a very common error, the *stack overflow*.

A stack overflow occurs, when you are in a nested call *deeper* than what your call stack can handle, and this occurs *WAY* earlier than an *integer* overflow (when the computer can no longer represent your big number, so it loops back around to the smallest number), which is the hypothetical bound to our `for`-loop.

This is problematic, because what if we, for some reason, *did* need to iterate that many times?

```hs
-- haskell
factorial 1 = 1                         -- we set the base case via pattern matching
factorial n = n * factorial (n - 1)
```

This factorial function would be bounded by the call stack! Not good. This form is what we call **head recursion**. This is the most common form of recursion, and you'd be forgiven to think that it's the *only* form of recursion.

## The Tail Call Optimization
```hs
factorial a 1 = a
factorial a n = factorial (a * n) (n - 1)
```

This is the same factorial function, but in **tail recursive** form. Spot the differences. For clarity, here's an imperative example as well:

```python
# head recursive
def factorial(n):
    if n == 1: return 1

    return n * factorial(n-1)

# tail recursive
def factorial_t(a = 1, n):
    if n == 1: return a

    return factorial_t(a * n, n - 1)
```

The difference in these implementations is that— in tail recursion, we don't *need* the previous stack frame. We don't need anything from the previous call. We can store all the information we need in our `a` parameter, which stands for the *accumulator* (because this is where you accumulate your computation, instead of relying on the call stack).

[ ILLUSTRATION 1 ]

In fact, we can just *re-use* our stack frame for our new call. This is called **tail-call optimization**, a compiler trick done by functional languages (and some imperative languages such as Rust, Lua, and Javascript) wherein it sees that there is no more computation needed after the recursive call, so it reuses the same stack frame.

[ ILLUSTRATION 2 ]

To better understand what it means to no longer have any other computation past the recursive call, think about how when you get to the lowest depth of head recursion, you now have to go back down the call stack, multiplying the accumulated `n`s while you were nesting calls. But in tail-recursion, you can get your return value (your accumulator) when you get to your final recursive call, no need for traversing the call stack twice.

The most common example of tail-recursion, and is actually just the generalization of it, is the `reduce`/`fold` function.

## Function Origami

If you are from a `Javascript` background, you've most likely encountered the `reduce` function. In fact, you might be familiar with this idiom for summing numbers:

```js
sum = arr => arr.reduce((a,b) => a + b, 0)
```

`reduce` is actually (roughly) equivalent to this *tail recursive* function:

```py
# python (because i don't want to deal with array prototype)

def reduce(arr, fn, acc):
    if len(arr) == 0: return        # base case, no more array

    acc = fn(acc, arr[0])           # apply fn to the head of the array and the accumulator

    return reduce(arr[1:], fn, acc) # recur with the rest of the array and the new accumulator
```

As we can see, `reduce` (also known as `fold`) is just a generalization over a tail recursive function! And you'd be surprised at how many things you can express as a `reduce`.

> `reduce`/`fold` gets its name from the fact that it `reduces`/`folds` dimensions of an array. Reducing a 2D array yields you a 1D array, and reducing a 1D array yields you an *atom* (single value). Neat!

Haskell provides us with two standard `fold` functions, `foldr` (reduces from the right) and `foldl` (reduces from the left). 

Here are some examples:

```hs
sum = foldr (+) 0                   -- partial application
product = foldr (*) 1               -- it doesn't matter whether we use foldr or foldl
any = foldr (||) False              -- because these operations are monoids over their inputs
all = foldr (&&) True               -- (which guarantees associativity)

factorial n = foldr1 (*) [1..n]     -- foldr1 takes the first element as the initial value
```

```hs
-- Folding from the left and prepending the result to an accumulator returns you the reversed array
reverse = foldl (\acc x -> x : acc) []

{-
    ARRAY   |  ACCUMULATOR
    [1 2 3]    [ ]
    [2 3]      [1]
    [3]        [2 1]
    [ ]        [3 2 1]
-}
```

```hs
max = foldr1 compare
    where compare a b           -- a helper function is defined to handle the condition
             | a > b = a
             | otherwise = b

{-
    ARRAY       |  ACCUMULATOR
    [3 4 2 5]       3
    [4 2 5]         3
    [2 5]           4
    [5]             4
    [ ]             5
-}
```

And here is the imperative pattern that `reduce` generalizes:

```go
// go

// the input array is of type U because your accumulator doesn't have to be
// the same type as your elements
func reduce[T any, U any](arr []U, fn func(U, T) T, initial T) T {
    result := initial

    for i := 0; i < len(arr); i++ {     // standard for loop syntax used for clarity
        result = fn(arr[i], result)
    }

    return result
}
```

Best part is, most modern languages that support higher-order functions (Python, Rust, Kotlin, ...) come with a built-in `reduce`/`fold` function, so you don't have to implement your own, just have to read documentation :>

> Except Go, Go likes to do its own thing.

---

And that should be it for this part, I figured a break was needed right after the **Functors** article, so here's one that's a bit more application-oriented. As always, I hope you enjoyed the article, and learned something new!
