> This is part 4 of a series of articles entitled *Functional Patterns*.
>
> Make sure to check out the rest of the articles!
>
> 1. [The Monoid](https://dev.to/if-els/functional-patterns-the-monoid-22ef)
> 2. [Compositions and Implicitness](https://dev.to/if-els/functional-patterns-composition-and-implicitness-4n08)
> 3. [Interfaces and Functors](https://dev.to/if-els/functional-patterns-interfaces-and-functors-359e)
> 4. [Recursion and Reduces](https://dev.to/if-els/functional-patterns-recursions-and-reduces-jhk)

# Functional Patterns: Applicatives and Zips
## Functor Recap

Learning about Functors have taught us the very important function that is `fmap`, or *mapping* a function on a `Functor` interface (most commonly on the *array*).

> Remember, the `Functor` interface differs from the actual term *functor*. Members of the interface `Functor`,
> to put simply, can be mapped over by *endofunctors* (which are a type of a function that maps a category to
> itself).

```python
# python
map( lambda a: a**2, [1, 2, 3] )  # [1, 4, 9]
```

Mapping a function on an array is a pretty common operation, a transformation on your data, if you may. But what if we wanted to apply a *different* function to each element? In terms of using `map` (or `fmap`), we are out of luck, as the functor supported by `map` are applied to all of the elements.

## Zips

Say we have an array of `int`, our points in a game show. We'll get this in array for every round of the game.

```py
# points per round
 [4, 3, 8, 1]
# r1 r2 r3 r4
```

We'd like to get the total of our score. However, we are told that points are worth more in the later rounds! That is, every point is worth the round it was gained in.

```python
[
    4 * 1,
    3 * 2,
    8 * 3,
    1 * 4
]
```

Let's try to get these values programatically. Let's first take an imperative approach, by using the index of our current value + 1 to multiply at our current number. We'll do this in Go, for a clearer demonstration:

```go
rounds := []int{ ... }
result := 0

for (i := 0; i < len(rounds); i++) {
   result += rounds[i] * (i+1)
}

return result
```

then in Python:

```py
rounds = [ ... ]
result = 0

for round, score in enumerate(rounds):
    result += score * (round + 1)

return result
```

We achieve pretty similar code, except for how we're now using a function named `enumerate`. Now what exactly does `enumerate` do?

```py
enumerate(["a", "b", "c", "d"]) # [ (0, "a"), (1, "b"), (2, "c"), (3, "d") ]
```

Ah, the `enumerate` function "maps" some function to our original array, turning it into an array of *pairs* containing the index and the original value. But this can't be any ordinary functor, since there can't be any function that can achieve this changing value without side-effects!

Let's try to work backwards, and separate the pairs into their own arrays.

```python
a = [ 0,   1,   2,   3 ]
b = ["a", "b", "c", "d"]
```

This shouldn't be that to get back to our paired array.

```python
a = [ 0,   1,   2,   3 ]
b = ["a", "b", "c", "d"]
result = []

for i in range(len(b)):
    c.append( (a[i], b[i]) )

return c
```

We can sim"b"plify this further by realizing our `a` array is actually just `range(len(b))`.

```python
a = range(len(b))
b = ["a", "b", "c", "d"]
result = []

for i in a:
    result.append( (i, b[i]) ):

return result

# also can be written as
# [ (i, b[i]) for i in a ]
```

Now we've implemented a function that takes two arrays `a` and `b`, and pairs them up into a resulting array! This is called the `zip` function. And the `enumerate` function is just a specialization of this, a `zip` with the first argument as the `range(len(b))` of the second argument.

> This is a simplified implementation for Python, as `zip` and `enumerate` actually return *generators*, and 
> mostly like don't need to use `range` as a dependency.

```python
b = ["a", "b", "c", "d"]
[*zip(range(len(b))], b) == [*enumerate(b)]  # [ (0, "a"), (1, "b"), (2, "c"), (3, "d") ]
```

Also, we've unlocked a whole new world for our `map` function, since now we can do differing *partial* applications for every element!

```hs
-- haskell
zip [3 .. 5] [9 .. 11] == [ (3, 9), (4, 10), (5, 11) ]
-- moreover, since functions are values...
zip [1 .. 3] [(*4), (+5), negate] == [ (1, (*4)), (2, (+5)), (3, negate)]
```

And since now we're in the array *category* once again, we can simply `map` or `reduce` over these pairs to perform our transformation!

```hs
points = [4, 3, 8, 1]

-- we pattern match on the tuple to add both elements
sum . map (\(a,b) -> a * b) . zip [1..] $ points

-- alternatively we can use the `uncurry` function, which does that
sum . map (uncurry (*)) . zip [1..] $ points

-- uncurry and mapping a zip is such a common pattern that it can be written as `zipWith`
sum . zipWith (*) [1..] $ points

-- point-free definition
totalScore = sum . zipWith (*) [1..]
```

![image](https://github.com/44mira/articles/assets/116419708/17a2744d-0dfb-4b7e-9c14-0708fd178ac5)

## Applicatives

A much more specific interface that is relevant to us is the `Applicative` typeclass.

```hs
class (Functor f) => Applicative f where
    pure :: a -> f a
    (<*>) :: f (a -> b) -> f a -> f b
--  ...
```

What this means is that: 
- in order to be `Applicative`, you first must be an instance of `Functor`.
- You have to define a function `pure`, which takes any type and then wraps it with your type constructor `f` (again, this is synonymous to a struct)
- You have to define a function `<*>`, which applies a wrapped function `f (a -> b)` to your wrapped value `f a` resulting in a wrapped result `f b`.

> The following are additional axioms that need to be fulfilled ontop of being a valid member of the `Functor` typeclass. You don't have to fully understand these right now, but I figured I shouldn't omit them.

```hs
pure id <*> v = v                            -- Identity
pure f <*> pure x = pure (f x)               -- Homomorphism
u <*> pure y = pure ($ y) <*> u              -- Interchange
pure (.) <*> u <*> v <*> w = u <*> (v <*> w) -- Composition
```

- The **identity** axiom states that applying a wrapped `id` (identity function) does nothing, working like the unwrapped id function.
- The **homorphism** axiom states that applying a wrapped function to a wrapped value is equal to applying the function to the value *and then* wrapping it.
- The **interchange** axiom states that applying u to a wrapped value is equal to applying a wrapped *partial application of `$` (thrush)* of the original value to the function.
    - `($ a) f` is the same as `f $ a` or `f a`.
- The **composition** axiom states that applying a wrapped `(.)` (composition function, b-combinator) is equal to composing the applied function to the following applications.

If we look at the signatures of the two main functions here `fmap` and `<*>`:

```hs
fmap  ::   (a -> b) -> f a -> f b
(<*>) :: f (a -> b) -> f a -> f b
```

We see that the only difference between these two is that `Applicative` has your functor wrapped in the same category (or data type) as your inputs!

## The Maybe Monad
A ubiquitous *Monad* (which requires it be an `Applicative`) is the `Maybe` monad. We won't be talking about its `Monad` aspects here but what it does, to put simply, is act as context for functions with optional returns.

```hs
data Maybe m = Just m | Nothing
```

What this says is: `Maybe` is a data type (can be thought of as a struct) that holds type `m` and can either be a `Just m` or `Nothing`. An example of a function using this data type is:

```hs
findIndex :: (a -> Bool) -> [a] -> Maybe Int

findIndex even [1, 3, 4, 5] == Just 2
findIndex even [1, 3, 5] == Nothing
```

What `findIndex` does is find the first index wherein a predicate returns `True`. Now of course, this has a case wherein the predicate never returns `True`, and what would we return in that case? This is where the `Maybe` monad comes in, by wrapping our result in the `Maybe` type, we can return a `Nothing` in this case, and when we do find a `True` case, we just have to wrap our return with `Just`.

> And if we're certain that we will always return a `Just`, we can just unwrap it using the `fromJust` function.

Since `Maybe` is under the `Monad` interface, and therefore an `Applicative` and `Functor`, we can do the things we were able to do on `Applicative` and `Functor` on it.

```hs
-- Being a functor:
fmap (* 2) (Just 4) == Just 8   -- we can map on it because its a functor, removing the need to unwrap
fmap (* 2) Nothing  == Nothing  -- which allows us to safely do operations on it.
```

But what if we needed to perform operations on multiple instances of a `Maybe`? We would have to unwrap them both, and then apply the function, right? However, this poses as risk— as unwrapping a `Nothing` will cause a runtime error, and writing all of that is just, horrible.

```hs
import Data.Maybe

Just (fromJust (Just 4)) + (fromJust (Just 5))  -- Just 9
Just (fromJust Nothing + (fromJust (Just 5))    -- ERROR!
```

`Applicative` allows us to *compose* functions without unwrapping our data types using the `<*>` function! Recall that all it does is apply a wrapped function, so we simply need to get to that point first. This is made more terse by the *infix* version of our lovely `fmap` function, written as `<$>`.

```hs
-- `pure` typed in the context of `Maybe Int` wraps our value in Just.
(pure 5) :: Maybe Int == Just 5

-- Applying a wrapped function
Just negate <*> Just 5 == Just (-5)
Just (+5)   <*> Just 2 == Just 7

-- Applying Nothing on either side resolves to Nothing
Nothing     <*> Just 5 == Nothing
Just (+5)  <*> Nothing == Nothing

fmap (+2) (Just 3) == Just 5
(+2) <$> Just 3    == Just 5        -- infix

(+) <$> Just 5 == Just (+5)         -- we get a wrapped *partially applied* function!

-- Therefore ..

(+) <$> Just 4 <*> Just 5 == Just 9   -- Composition!
(+) <$> Just 4 <*> Nothing == Nothing -- Composition!
```

The `Maybe` is analogous to Rust's `Option` type (although they do not have a way of simply composing these types), in that it can be used to avoid using *sentinel* or `null` values in your code altogether! This can even be further extended to handle errors in your code as a `Nothing` always propagates throughout the composition.

## The ZipList Applicative
From reading the documentation, we actually find out that `[]` is also a data type of the `Monad` interface, again making it an `Applicative` and a `Functor`. We won't be demonstrating it's `Functor` properties as we've already covered that in a previous part of the series.

```hs
-- `pure` in the context of `[Int]` creates a singleton list
(pure 5) :: [Int] == [5]

-- Applying a list of functions to a singleton list returns a list of the applications!
[negate] <*> [5] == [-5]
[(*3), abs, (+2)] <*> [5] == [15, 5, 7]

-- Applying to our empty list always returns an empty list
[] <*> [3, 4] == []
[(*3), abs, (+2)] <*> [] == []

-- Applying a list of functions to a list of elements, performs a cartesian product
[(+2), (*3)] <*> [5, 2, 1] == [7, 15, 4, 6, 3, 3]
```

What's interesting with the list of functions applied to a list of values is that: it performs the function on *all combinations* of the functions and elements. Moreover, they are kept in a *flattened* list. This would be equivalent to:

```python
result = []

for x in values:
    for f in functions:
        result.append(f(x))

# or
[ f(x) for x in values for f in functions ]
```

All within a single line.

We can actually implement the `zip` behavior of lists using an `Applicative` interface!

Let us define a `ZipList` typeclass.

```hs
-- we define our struct that holds an array of `a`
data ZipList a = ZipList { getZipList :: [a] }

-- we define its membership to the Functor typeclass
instance Functor Ziplist where
    -- we say that mapping on a ZipList is just mapping over its embedded array
    fmap f (Ziplist a) = Ziplist (map f a)

instance Applicative ZipList where
    -- we say that applying a list of wrapped functions is just zipping over wrapped values
    ZipList fs <*> ZipList xs = ZipList (zipWith ($) fs xs)
```

However, we lack one definition here: `pure`. I've intentionally skipped this because this is a good demonstration to show where the *axioms* come into play.

Say we define the following:

```hs
pure x = ZipList [x]   -- mirroring the definition for `pure` of [], we put x in a singleton list
```

Recall that according to the **identity** axiom:

```hs
pure id <*> v == v
```

This definition falls apart when we substitute `v` as the infinite list `[1..]`

```hs
pure id <*> v == v

ZipList [id] <*> [1..] == ZipList [1..]
ZipList (zipWith ($) id [1..]) == ZipList [1..] -- zipWith truncates based on the shorter list
Ziplist [1] != ZipList [1..]                    -- false!
```

The problem with `zip` is that it truncates based on the shorter list, meaning for an application to be valid, the list `fs` must be infinite! So the definition of `pure` follows from that.

```hs
instance Applicative ZipList where
    -- we say that applying a list of wrapped functions is just zipping over wrapped values
    ZipList fs <*> ZipList xs = ZipList (zipWith ($) fs xs)

    -- We say that the `pure` of some element x, is the infinite list of itself
    pure x = ZipList (repeat x)
```

And if you think about it— applying an infinite list of `id` to a finite list of `x`, will always return you the original list! Cool!

---

Goes without saying; there are so many more *applications* for the `Applicative` pattern out there, I hope you find them and recognize them!

And that concludes the penultimate part of this series! I hope you're enjoying and are able to keep up, if not, feel free to contact me for clarifications. I hope you learned something new, and see you in the last part `The Monad`!

---

Reference: https://en.wikibooks.org/wiki/Haskell/Applicative_functors
