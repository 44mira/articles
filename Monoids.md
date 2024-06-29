# Functional Patterns: The Monoid

> Trigger Warning: this article contains Haskell codeblocks!

## Introduction

As a programmer, I've always found myself obssessing over patterns I
could find in the code I write. From simple ones such as the Gaussian
sum and early returns, to ones that hold a bit more complexity such as the
Strategy pattern.

I find satisfaction in finding elegant ways to express
recurring needs and results, which has ultimately led me to my exploration
of the **Functional Programming** paradigm. And so, early in 2023, in my
freshmen of college, I decided to undergo the massive undertaking that is,
*learning Haskell*.

I'm not going to bore you on the details that is learning a language which is
meant to unapologetically academic as I believe I am still not that good
at it yet, but I have picked up the patterns that I had been searching for in
the first place, which is a win in my book.

I plan to have this as the first article of more elegant patterns I've found
from my months of studying functional programming.

## Types and Categories

> I'll try to save you from the incredibly *white-paper* definitions (I'm
> looking at you, category theory), so some of the definitions you encounter in
> these articles may be oversimplifications and I encourage the reader to
> research more on it for a deeper (and more correct) understanding.

A recurring theme amongst functional languages (or ones that implement a lot of
of rules present in *pure* functional languages such as Rust)— is the presence
of **type safety** and how, most of the time, the functions themselves are
correct (also known as provable), as long it type checks.

Not only does this make several bugs impossible *by design*, we can find that
from this emerges entirely new patterns that we can take advantage of (at the
cost of a little bit more overhead, of course).

Let's take a look at a function signature for the `abs` (a common name for the
absolute value function) function, from Haskell:

```hs
-- function bodies won't be defined unless they are relevant
abs :: Int -> Int
abs n = undefined
```

and its equivalent signature in a language like Go:

```go
func abs (n int) int {
    // function bodies won't be defined unless they are relevant
    panic()
}
```

We can see that Haskell's `->` gives us a pretty good idea of what a function
does.

> It takes an `Int` and *returns* another `Int`.

Moreover, we see that this function only takes *one* argument, and therefore can be
referred to as a *unary* function.

Let's take a look at an example for a *binary* function.

```hs
add :: Int -> Int -> Int
add a b = a + b
```

And what should be its equivalent Go function:

```go
func add(a, b int) int {
    return a + b
}
```

That's odd, we can see that Haskell's signature requires a bit more of thinking
to understand, and you'd be forgiven for thinking this signature meant:

> A function that takes an `Int` and *returns* an `Int` that *returns* an `Int`.

But this actually has something to do with how it deals with multi-variable
functions under the hood. This is due to an inherent constraint in *pure*
functional languages, that is:

> A function always take *one* argument and *returns* one result.

And as you can tell, it does its job as a constraint really well because— well,
it is very constraining. However, this can be worked around using this pattern
called **currying**.

This is the actual equivalent of the Haskell code in Go code:

```go
func add(a int) func(int) int {
    return func (b int) int {
        return a + b
    }
}
```

Or a terser equivalent in Javascript:

```js
a => b => a + b
```

Aha! There are two functions, one for each argument! And because the second
function is declared inside the first one, it can access the `a`. This is
called a *closure*.

So what's really happening in the Haskell signature is:

```hs
add :: Int -> (Int -> Int)
```

Our `add` function takes an `Int`, then returns **another** function that takes
an `Int` and returns an `Int`! Currying!

Lastly, to demonstrate type-correctness here's another example:

```hs

-- takes:
-- * some unary function (a -> b)
-- * list of a
--
-- returns:
-- * a list of b
map :: (a -> b) -> [a] -> [b]

sqr :: Int -> Int

sum :: [Int] -> Int

sumOfSquares :: [Int] -> Int
sumOfSquares = sum . map sqr

```

We can prove the type of `sumOfSquares` by following the types of the composed
functions in its definition.

- `sqr` takes an `Int` returns an `Int`
    - At this point our signature is: `Int -> Int`
- `map` takes a function from some type `a` (in this case `Int`) and turns it
  into some type `b` (in this case, still `Int`), and also takes a list of `Int`
    - At this point our signature is: `[Int] -> [Int]`
        - Notice how we are not asking for an `(a -> b)`, as this is curried
          into `map` by providing it the argument of `sqr`.
        - We are now returning the `[a] -> [b]` part of the signature.
- The result of map is then "piped" into `sum`, which takes a list of `Int`,
  returning an `Int`.
    - We finally reach our final signature of `[Int] -> Int`!

## The Monoid

> A type is said to be a Monoid over some binary function or operation if the
> result remains within the domain of the type, AND there exists an identity
> element.

Or essentially, if you have some binary function `f` over some type `a`,
meaning both arguments of `f` be of type `a`, and the result is still of type
`a`. And there exists an element of type `a` that when applied to any other
element of type `a` over `f`, results in the same element.

Here are some examples of Monoids:

- We can say `Int` is a Monoid over `+` (addition) because whatever two `Int`s we add
  will always yield another `Int`.
  - The identity element of this Monoid would be the number `0`, as adding `0`
    to any `Int` will give you the *same* `Int`.
- We can say `Int` is a Monoid over `*` (multiplication) because whatever two
  `Int`s we multiply, will always yield another `Int`.
  - The identity element of this Monoid would be the number `1`, as multiplying
    `1` to any `Int` will give you the *same* `Int`.
- We can't say `Int` is a Monoid over `/` (division) because there exists
  division operations between two `Int`s that do not yield an `Int` (i.e. `1 /
  2`)

A useful property of monoids is that— as long as you are only applying
functions to a type in which in it is a Monoid over, you can easily guarantee
type safety.

```hs
(+) :: Int -> Int
(*) :: Int -> Int

incrementNumber :: Int -> Int
incrementNumber a = 4 * 5 + 3 + a

```

As you can see, they all share the same signature, and this is because they are
a Monoid over the two composed functions, and therefore `Int` is also a Monoid
over `incrementNumber`!

## Usage

> Goes without saying, there will be other uses for Monoids that you might
> encounter yourself, and so I'll leave that to you to discover yourself :>

Let's say we are creating an Auto Moderator that filters characters from chat
messages based on arbitrary predicates set by us, the developer. Essentially,
we want to run several checks and make sure a character passes all of them.

Let's take a look at the signatures of the predicates we will be using:

```hs

isBraille :: Char -> Bool
isUpper :: Char -> Bool
isNumber :: Char -> Bool
isEmoji :: Char -> Bool

```

Very strange predicates for a chatting service indeed. But from these
signatures, we cannot immediately see where the Monoid pattern applies, after
all none of these take the same type as the type it returns!

So let's apply the naive solution to this problem.

```hs

isValid :: Char -> Bool
isValid c = not (isBraille c || isUpper c || isNumber c || isEmoji c)
```

**Disgusting and abhorrent**. This degree of repetition in code should already
be raising some alarms for you.

Let's think it over again, what do we really need here? We need some function
`(Char -> Bool)` that acts as the *disjunction* of all the `(Char -> Bool)`s we
have.

Let's take a look at the signature for the `->` function (yes, it is a function
as well).

```hs
type (->) :: * -> * -> *  -- this just means it is not a concrete type
                          -- it needs 2 concrete types to be one.
                          -- i.e (Char -> Bool) is a type but (Char ->) is not.

-- ...

instance Monoid b => Monoid (a -> b) -- important!
```

There it is! This *instance* signature states, that if the return of a function
is a Monoid, the entire function *is* a Monoid! And this definition does not have
any conflict with our definitions we've previously established.

And if you think about it, a `Bool` is actually a Monoid over *disjunction*!
For any boolean you perform a logical `OR` on, you will always get another
boolean.

Moreover, if you logical `OR` any boolean with `False`, you will end up with
the same boolean, fulfilling the condition for an identity element!

And the last piece of our puzzle, the Haskel's `fold` function. Let's take a look at its
signature.

```hs
import Data.Foldable

fold :: (Foldable t, Monoid m) => t m -> m
```

What this means for our context is that the `fold` function requires a list
(which falls under the `Foldable` constraint) of our Monoid type.

However, to define a proper Monoid type, we have to specify what it is a Monoid
*over*. Thankfully, this is already done for us by Haskell, by its `Any` wrapper.

And so we're left with:

```hs
import Data.Monoid
import Data.Foldable

isBraille :: Char -> Bool
isUpper :: Char -> Bool
isNumber :: Char -> Bool
isEmoji :: Char -> Bool

isValid :: Char -> Bool
isValid = not . getAny . fold predicates
    where predicates = map (Any .) [isBraille, isUpper, isNumber, isEmoji]

```

First, we convert all of our list of `(Char -> Bool)` to a list of `(Char -> Any)`
by using a `map (Any .)`.

So now we have a list of `(Char -> Any)`, which if you remember, is now a list of
Monoids that can be combined into one `(Char -> Any)` using `fold`, which is
equal to applying them one after another!

> NOTE: The Monoid's binary function is associatve, meaning it can be applied in
> any order.

And then lastly, we extract our value from the `Any` wrapper, and then negate it
with `not`. And now we have a much more elegant solution.

Equivalent Go code:

```go

func isValid(c rune, predicates []func(rune) bool) bool {
    result = false  // our identity element

    // fold over our Monoid
    for _, predicate := range predicates {

        if result = result || predicate(c); result {
            break;  // early return on first true (also done by Haskell under the hood)
        }
    }

    return !result
}
```

And that should be it! I hope you learned something new from this article, and
maybe even get to apply this pattern in your future coding endeavours.
