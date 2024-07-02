> This is part 3 of a series of articles entitled *Functional Patterns*.
>
> Make sure to check out the rest of the articles!
>
> 1. [The Monoid](https://dev.to/if-els/functional-patterns-the-monoid-22ef)
> 2. [Compositions and Implicitness](https://dev.to/if-els/functional-patterns-composition-and-implicitness-4n08)


# Functional Patterns: Interfaces and Functors
## Generics and Typeclasses

To be correct, a function must type-check, and is therefore provable. But in the case of *generalized* functions, meant to deal with various types, this immediately shows as a pain point. To make a `double` function work across types, we would have to define them separately!

```haskell
doubleInt :: Int -> Int
doubleChar :: Char -> Char
doubleFloat :: Float -> Float
-- ...
```

And for any self-respecting programmer, you should already be finding yourself absolutely **appalled** by this. We'd just learned about a pattern for building case-handling using *partial application* but we can't really apply it here since our type signatures won't allow that, and our function **has** to type-check.

Thankfully, this is already a feature in most modern programming languages. We are allowed to define a `generic` type. A *hypothetical* type that only has to verify matching positions in the function signature or variable declarations.

```c
// c++
template <typename T>
T double(T x) {
    return x*2;
}
```

```rust
// rust
fn double<T>(x: T) -> T {
    return x*2;
}
```

```hs
-- haskell
double :: a -> a
double = (*2)       -- partially applied multiplication
```

And that should solve our problem! As long as the compiler is given these *generics*, it can figure out what types it has to use at run-time (Rust actually still does this inference at compile-time!).

However, even though there is merit in this implementation— there is still a glaring flaw, that actually gets pointed out by the Haskell compiler, as the above Haskell code actually raises an error.

> No instance for ‘Num a’ arising from a use of ‘*’...

We've defined a type, but we aren't always going to be sure this type has the *capacity* to be doubled. Sure, this immediately works on numbers, but what's stopping the user from calling `double` on a String? A list? Without a predefined *method* for doubling these types, they should not be allowed as arguments, in the first place.

So contrary to the name of *generics*, we're going to have to get a bit more *specific, but still general*.

This is where **typeclasses** come in, or also known more commonly in the imperative world as **interfaces**. Again, if you're using any language that has been made later than C++, you should have access to some implementation of interfaces.

Interfaces, compared to generics, specify some sort of *capability* of types that can be *categorized* under it.

Here is a fixed version of our previous code.

```haskell
double :: (Num a) => a -> a     -- a has to be of typeclass Num
double = (*2)
```

or in Go:

```go
// We first create an interface that is the intersection of floats and integers.
type Num interface {
    ~int | ~float64
    // ... plus all other num types
}

func double[T Num](a T) T {
    return a * 2
}
```

For brevity's sake we'll say that Haskell doesn't really deal with embedded state in their interfaces, such as Typescript's and Go's interfaces (a constraint brought upon by pure functional rules). So even though you might be able to define required *attributes* of a type to be under an interface, know that *pure* interfaces only have to define **functions** or **capabilities** of the type.

And by capabilities in this context, we are talking about if the type has a *dependency* in the form of a doubling function— is the compiler *taught* how to double it?

```hs
import Control.Monad (join)

class CanDouble a where
  double :: a -> a

instance CanDouble Int where
  double = (* 2)

instance CanDouble Float where
  double = (* 2)

-- we tell the compiler that doubling a string is concatenating it to itself.
instance CanDouble String where 
  double = join (++)    -- W-combinator, f x = f(x)(x)
```

And now we're pretty much back to where we were at the start when it comes to code repetition, isn't that funny?

But this fine-grained control of implementation is actually where the power of this comes in. If you've ever heard of the *Strategy* pattern before, this is pretty much it, in the functional sense.

```hs
quadruple :: (CanDouble a) => a -> a
quadruple = double . double

leftShift :: (CanDouble a) => Int -> a -> a
leftShift n e
  | e <= 0 = n
  | otherwise = leftShift (double n) $ e - 1
```

These functions type-check now, all because we *taught* the compiler how double types under the `CanDouble` typeclass.

We can achieve something similar in Go, a big caveat being that we can only define interface methods on *non-primitive* types. Meaning, we have to define wrapper structs to primitive types.

```go
type CanDouble interface {
	double() CanDouble
}

type String string
type Number interface {
	~int | ~float64
	// ... plus all other num types
}

type Num[T Number] struct {
	v T
}

func (s String) double() String {
	return s + s
}

func (n Num[T]) double() Num[T] {
	return Num[T]{n.v * 2}
}

func quadruple(n CanDouble) CanDouble {
	return n.double().double()
}

func leftShift(n CanDouble, e uint) CanDouble {
	for i := uint(0); i < e; i++ {
		n = n.double()
	}

	return n
}
```

This honestly is kind of a bummer, but no worries, as most of the time you're going to be dealing with interfaces will be with custom types and structs.

## Categories

> Category theory is a general theory of mathematical structures and their relations.

We've briefly brushed upon *category theory* back in `The Monoid`, and we'd like to keep it that way, only mere close encounters. I will be referencing it here and there, but rest assured: you won't need to have a background in it to grasp whatever follows.

However, there is no doubt that we have encountered *sets* before.

As a brief recap, Sets can be thought of as a **collection** of elements. These elements can be absolutely *anything*.

```
{ 0, 1, 2, 3, ... }             -- the set of natural numbers
{ a, b, c, ..., z}              -- the set of lowercase letters
{ abs, min, max, ... }          -- the set of `Math` functions in Javascript
{ {0, 1}, {a, b}, {abs, min} }  -- the set of sets containing the first 2 elements of the above sets
```

Adding on to that, we have these things called **morphisms**, which we can think of a mapping between elements.

> Very big omission here on the definitions of morphisms, in that they are *relations* between elements, and not strictly functions/mappings,
> you can look it up if you are curious.

We can say a function like `toUpper()` is a morphism between lowercase letters *to* uppercase letters, just like how we can say `double = (*2)` is a morphism from numbers *to* numbers (specifically even numbers).

And if we group these together, the set of elements and their morphisms, we end up with a *category*.

> Again, omission, categories have more constraints such as a Composition partial morphism and identities. But these properties are not that relevant here.

If you have a keen eye for patterns you'd see that there is a parallel to be drawn between categories and our interfaces! The *objects* (formal name for a category's set of elements) of our category are our *instances*, and our *implementations* are our *morphisms*!

```hs
class CanDouble a where
    double :: a -> a

-- `Int` is our set of elements { ... -1, 0, 1, ... }
-- `(* 2)` is a morphism we defined
-- ... (other omissions)
-- ...
-- Therefore, `CanDouble Int` is a Category.
instance CanDouble Int where
    double = (* 2)
```

## Functors

Man, that was a lot to take in. Here's a little bit more extra:

A **Functor** is a type of a function (also known as a mapping) from *category* to *another category* (which can include itself, these are called *endofunctors*).

What this essentially means, is that it is a transformation on some category that maps every element to a corresponding element, and every morphism to a corresponding morphism. An output category based on the input category.

In Haskell, categories that can be transformed by a functor is described by the following typeclass (which also makes it a category in of itself, that's for you to ponder):

```hs
class Functor f where
    fmap :: (a -> b) -> f a -> f b
    -- ...
```

`f` here is what we call a *type constructor*. By itself it isn't a *concrete* type, until it is accompanied by a concrete type. An example of this would be how an *array* isn't a type, but an array *of `Int`* is. The most common form of a type constructor is as a *data type* (a struct).

From this definition we can surmise that all we need to give to this function `fmap` is a function `(a -> b)` (which is our actual functor, don't think about the naming too much), and this would transform a type `f a` to type `f b`, a different type in the *same* category.

> Yes, this means Haskell's `Functor` typeclass is actually a definition for *endofunctors*, woops!

If all of that word vomit was scary, a very oversimplified version for the requirement of the `Functor` typeclass is that you are able to `map` values to other values in the same category.

Arguably the most common `Functor` we use are arrays:

```hs
instance Functor [] where
--  fmap f [] = []
--  fmap f (a:as) = f a : fmap as
    
    -- simplified
    fmap :: (a -> b) -> [a] -> [b]
    fmap f arr = map f arr
```

We are able to map an array of `[a]` to `[b]` using our function (or functor) `f`. The typeconstructor of `[]` serves as our category, and so our functor is a transformation from one type of an array to another.

So, formally: the `map` function, though commonly encountered nowadays in other languages and declarative frameworks such as *React*, is simply the application of an *endofunctor* on the category of *arrays*.

Wow. That is certainly a description. 

Here are more examples of functors in action:

```go
// Go
type Functor[T any] interface {
    fmap(func(T) T) Functor[T]
}

type Pair[T any] struct {
	a T
	b T
}

type List[T any] struct {
    get []T
}

// Applying a functor to a Pair is applying the function
// to both elements
func (p *Pair[T]) fmap(f func(T) T) Pair[T] {
	return Pair[T]{     // apply f to both a and b
		f(p.a),
		f(p.b),
	}
}

func (a *List[T]) fmap(f func(T) T) List[T] {
    res := make([]T, len(a.get))    // create an array of size len(a.get)

    for i, v := range a.get {
        res[i] = f(v)
    }

    return List[T]{res}
}
```

```hs
-- haskell
data Pair t = P (t, t)

instance Functor Pair where
    fmap f (P (x, y)) = P (f x, f y)
```

So all that it takes to fall under the `Functor` (again, endofunctor), interface is to have a definition on how to *map* the contents of the struct to any other type (including its own).

> This is another simplifcation, functors also need to have property of identity and composition.

---

Apologies if this article contained **way** more math than applications, but understanding these definitions will help us greatly in understanding the harder patterns later in this series, namely `Applicatives` and finally `Monads`.

> A monad is a *monoid* in the *category* of *endofunctors*.

We're getting there! :>
