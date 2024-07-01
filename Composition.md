# Functional Patterns: Composition and Implicitness

> This is part 2 of a series of articles entitled *Functional Patterns*.
>
> Make sure to check out the rest of the articles!
> 1. [The Monoid](https://dev.to/if-els/functional-patterns-the-monoid-22ef)

## Partial Application

Often we talk about *currying* in an imperative context, it seems unnecessary as it is extra overhead just to be able to deal with multiple arguments.

After all, why should you write `a => b => a + b` when you can write it with `(a, b) => a + b`?

And the reason is the main power of currying patterns: **Partial Application**.

In a non-curried declaration we can really only have:

```python
# Python

def add(a, b):
    return a + b

print(add(4, 5))    # 9
print(add(4, 3))    # 7
```

But because curried functions return us another function, we can decide to keep it in a *partially-applied state*.

```python
def add(a):
    return lambda b: a + b

print(add(4)(5))    # 9

# or ...

add4 = add(4)       # A function that takes 1 argument and adds 4
                    # It's a `partially-applied` version of the original `add` function.

print(add4(5))      # 9
print(add4(3))      # 7
```

Since arguments are handled in separate functions, we can *pre-apply* some arguments that might fit our use-case without having to rewrite similar logic.

```py
def resize_image(image_type):
    def resize(image, x, y):
        # some extra logic
        return resized_image

    if image_type == "svg":
        # some extra logic 

    return resize

resize_svg  = resize("svg")
resize_png  = resize("png")
resize_jpg  = resize("jpg")
# ...
```

And now we pretty much have a *Builder* pattern minus all of the disgusting OOP.

## Composition and Combinators

In the functional style, complexity comes from simple functions *composed* together.

```python
reverse(map(lambda a: a*a, [4,5,3,2]))

# or ...

reverse([a*a for a in [4,5,3,2]])
```

Here we are squaring all the numbers in an array, and then reversing the resulting array. When we find ourselves saying "*and then*" after describing what a function does, this is already actually *composition*! And in a functional paradigm, that's pretty much where all the logic happens, *in* composition.

Moreover, the type of composition you're probably most accustomed to is actually, what we call a *combinator*.

Combinators (from the field of mathematics called *lambda calculus*, and eventually [*combinatory logic*](combinatorylogic.com) \[which is different from combinatorics!]) are patterns which describe the *way* you are to compose functions together.

In fact, this manner of composition is called the **Bluebird** (or *B-Combinator*). The definition is as follows:

```javascript
// javascript for terse lambdas
B = f => g => a => f(g(a))
```

And it checks out, the `g` function is applied first to a, then `f` is applied to its result!

Let's see an example of the *B-combinator* in use.

```python
B = lambda f: lambda g: lambda a: f(g(a))

# curried map
c_map = lambda f: lambda a: map(f, a)

# our original composition
reverse(map(lambda a: a*a, [4,5,3,2]))

# The same expression written with the Bluebird
# (spaces are added for clarity)
B (reverse) (c_map(lambda: a*a)) ([4, 5, 3, 2])

```

And let's add on another combinator called the *Thrush* combinator, which is pretty useless in imperative programming. All it does is apply a function `f` to a value `a`.

```javascript
T = f => a => f(a)
```

However, this is an important building block in a *pure* and *lazy* functional language such as Haskell, as it allows us to evaluate the function `f` first, before applying it.

And now we should be able to read all common Haskell syntax!

```haskell
-- B-Combinator
(.) :: (b -> c) -> (a -> b) -> a -> c
(.) f g a = f (g a)

-- T-Combinator
($) :: (a -> b) -> a -> b
($) f a = f a
infixr 0 $  -- the left argument is evaluated first

ans :: [Int]
ans = reverse . map (\a -> a*a) $ [4, 5, 3, 2]
```

Okay, well maybe not *all* syntax, but from inference, we can see where the combinators are used:

- The B-Combinator composes `reverse` and a curried `map`
- The T-Combinator applies the composed function to the array `[4, 5, 3, 2]`

## Even more Combinators!

Composition happens so much in functional programming that mathematicians way smarter than us have actually written down and coined recurring patterns as *even more* [combinators](combinatorylogic.com/table.html).

> Combinators are called combinators because most of them are derived from *combining* other combinators.

Here are 3 combinators that are notable in my opinion (the rest is left for curious readers to read up on :>).

1. The *Phi* Combinator

```js
phi = f => g => h => a => f (g(a)) (h(a))
```

Function `f` is called with the arguments `g(a)` and `h(a)`, or the result of calling `g` and `h` on a value `a` separately.

A great example of this pattern would be the `average` function.

```py
phi = lambda f: lambda g: lambda h: lambda a: f (g(a)) (h(a))

a = [1, 2, 3, 4, 5]

div = lambda a: lambda b: a / b     # helper division function

print( sum(a) / len(b) )            # we can see the pattern emerge here
                                    # if we think of division as a function

average = phi (div) (sum) (len) (a)
print(average)
```

```hs
-- haskell
import Control.Applicative

-- liftA2 is Haskell's phi combinator
average = liftA2 div sum length [1 .. 5]
```

2. The *Psi* Combinator

```js
psi = f => g => a => b => f (g(a)) (g(b))
```

Function `f` is called with the arguments `g(a)` and `g(b)`, or the result of calling `g` on value `a` and `b` separately.

There are two good examples that come to mind.

```javascript
// javascript
psi = f => g => a => b => f (g(a)) (g(b))

eq = a => b => a == b   // helper function for checking equality

// A simple way to compare arrays in javascript is to turn *both* of them into strings first.
H
a = [2, 3, 4]
b = [3, 4, 5]

console.log( JSON.stringify(a) == JSON.stringify(b) )

console.log( psi (eq) (JSON.stringify) (a) (b) )

// The distance formula has you square *both* differences first, and then sqrt the sum.
distanceFormula = (x2, x1, y2, y1) => {
    return psi (a => b => Math.sqrt(a + b)) (a => a * a) (x2 - x1) (y2 - y1))
}
```

```hs
-- haskell
import Data.Function

-- `on` is Haskell's infix version of Psi
eqArr = ((==) `on` show) [2,3,4] [3,4,5]

distanceFormula x2 x1 y2 y1 = (sqrt .: ((+) `on` (^2))) (x2 - x1) (y2 - y1)
```

3. The *Starling* (S-Combinator)

```javascript
s = f => g => a => f (a) (g(a))
```

Function `f` is called with the arguments `a` and `g(a)`, the result of calling `g` on value `a`.

For the ones with keen eyes, you can already probably notice that the S-Combinator is actually just a special form of the Phi combinator. And that's because it is. Specifically, it is the Phi combinator with `g` as the `identity` function (or the **I-Combinator**) defined as:

```javascript
i = a => a          // the identity combinator simply returns its argument

s = f => g => a => phi (f) (i) (g) (a)
```

A great example would be checking if a string is a palindrome, wherein we compare a string to its reverse.

```python
# python
s = lambda f: lambda g: lambda a: f (a) (g(a))

eq = lambda a: lambda b: a == b     # curried helper

s (eq) (reverse) ("racecar")
```

```hs
-- haskell
ans :: Bool
ans = ((==) <*> reverse) "racecar"   -- (<*> is Haskell's infix version for the S-combinator)
```

## Implicitness
Using compositions and combinators allow us to write functions with *implicit* arguments. This is called its *tacit* or *point-free* form.

This means we no longer have to specify the arguments of our functions, as they are implicitly declared by the compositions we use, leaving us with a partially applied function.

```python
sqr = lambda a: a*a # helper for squaring
c_map = lambda f: lambda a: map(f, a)

# Explicit form
def _sum_of_squares(arr : list[int]) -> int:
    squares = map(sqr, arr)
    return sum(squares)

# Implicit form
sum_of_squares = b (sum) (c_map(sqr))   # Note that we aren't providing the `arr` argument

sum_of_squares([1,2,3]) # But we can still use it because sum_of_squares is partially applied
```

And this is how functional languages can get away with elegant point-free definitions. In fact, one of my favorite things to do when writing in functional languages is to refactor them into their tacit forms.

```hs
import Control.Applicative
import Data.Function

sumOfSquares :: [Int] -> Int
sumOfSquares = sum . map (^2)

isPalindrome :: String -> Bool
isPalindrome = (==) <*> reverse

isLonger :: String -> String -> Bool
isLonger = (>) `on` length

average :: [Int] -> Float
average = liftA2 (/) (sum) (length)
```

Tacit definitions of course bring extra overhead to your code, but like many things in modern programming, they can serve as abstractions for programmers to be able to write terse and (subjectively) *clean* code by leveraging *math*.

Applying functional patterns in mostly imperative languages don't really end up that nice-looking but I still hope you learned something new from this, and are able to apply the patterns in this article (probably not in production codebases) to your future code!
