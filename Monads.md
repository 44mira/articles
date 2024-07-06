> This is the final part of a series of articles entitled *Functional Patterns*.
>
> Make sure to check out the rest of the articles!
>
> 1. [The Monoid](https://dev.to/if-els/functional-patterns-the-monoid-22ef)
> 2. [Compositions and Implicitness](https://dev.to/if-els/functional-patterns-composition-and-implicitness-4n08)
> 3. [Interfaces and Functors](https://dev.to/if-els/functional-patterns-interfaces-and-functors-359e)
> 4. [Recursion and Reduces](https://dev.to/if-els/functional-patterns-recursions-and-reduces-jhk)
> 5. [Zips and the Applicative](https://dev.to/if-els/functional-patterns-zips-and-the-applicative-14om)

# Functional Patterns: The Monad

## What's the problem?

> A monad is just a monoid in the category of endofunctors, what's the problem?

Is a quote from [A Brief, Incomplete, and Mostly Wrong History of Programming Languages](http://james-iry.blogspot.com/2009/05/brief-incomplete-and-mostly-wrong.html), when he mentioned Haskell.

Though memed throughout the times, this statement actually manages to hold some truth to it still, being a pretty good description on what *monads* are.

No pattern had *fascinated* me more than the Monad, and for a while I had obssessed over being able to understand it, only for it to slip out of my grasp every single time. Monads had been so notable to me as there's a long running joke that monads are a mystery— because when you learn it, you forget all ability to teach and describe it.

For a while I had been reading about this pattern that kept getting *praised* in the functional programming community, but I hadn't come across a definition— an explanation, that did *it* for me.

But somewhere along that road, after all those times sunk into understanding this pattern— I felt like I could comfortably say that I had reached an understanding on it. It stopped being a "Aha! I think I got it!"

And that really was the motivation behind this article series, to help curious individuals tackling this niche I devled into a year prior, have a better time than I did. I told myself:

> Six articles, building up to a decent explanation on the Monad.

And here we are. I hope I hadn't lost you on the way here, but we've made it. What's left now is tackling the main pattern itself.

## In the Category of Endofunctors

Let's slowly take apart the quote describing monads.

> ... in the *category* of *endofunctors*.

These are terms we had already encountered, and what this is telling us is that the `Monad` *deals* with endofunctors, not any normal value we're used to. Let's take a look at its Haskell definition.

```hs
class Applicative m => Monad m where
    (>>=) :: m a -> (a -> m b) -> m b
    return :: a -> m a
-- ...
```

There it is! We see that to be a `Monad`, you first must be under the `Applicative` typeclass (which are endofunctors that can be applied using `<*>`), which further requires you be under the `Functor` type class in the first place (you can map `endofunctors` using `fmap`).

So, like the article on `Functor` and `Applicative`, we are going to be talking about functions applying onto some data type, some struct, we have defined. Notably using this `>>=` operator here, also referred to as `bind`.

We also have this function `return`, which should already be pretty familiar to us.

```hs
class Functor f => Applicative f where
    pure :: a -> f a
    -- ..

class Applicative m => Monad m where
    return :: a -> m a
    -- ..
```

It's essentially an alias for our `pure` function previously defined in `Applicative`! Surely there has to be a reason why it's called `return` now? We'll discover that shortly.

## A Monoid
Recall our definition of a `Monoid`.

> A type is said to be a Monoid over some binary function or operation if the result remains within the domain of the type, AND there exists an identity element.

That can only mean one thing, a `Monad` is just defining some *binary* operation *over* endofunctors! This is the next piece of our puzzle, let's take a look at the definition of `bind` specifically:

> If you're a bit confused, remember that a `Monoid` is merely an interface that requires you have an operation that takes two arguments of the same type, and produce the same type. This is why we can say `+` is a Monoid over `Int`, and also `Int` is a monoid over `+`.

```hs
class Applicative m => Monad m where
    (>>=) :: m a -> (a -> m b) -> m b
    -- ..
```

We see that `bind` is not only a *binary* function, but also that it returns a data type `m b`, which corresponds to the same category as our input `m a`, despite them having differing internal types— this has to be our `Monoid` operation! Let's compare it with the other operations defined in the previous type classes leading up to it, renaming all type constructors as `f`, for clarity.

```hs
(<$>) :: (a -> b) -> f a -> f b     -- fmap
(<*>) :: f (a -> b) -> f a -> f b   -- apply
(>>=) :: f a -> (a -> f b) -> f b   -- bind
```

Let's add a few spaces to highlight the pattern and swap out `>>=` with `=<<`, its flipped equivalent (arguments are swapped places).

```hs
(<$>) ::   (a ->   b) -> f a -> f b
(<*>) :: f (a ->   b) -> f a -> f b
(=<<) ::   (a -> f b) -> f a -> f b
```

So we see that they all in fact *deal* with some functor, but in slightly different ways. 

- `fmap` takes an *function* and then a *wrapped* value, mapping the function *over* the wrapped value.
- `<*>` takes a *wrapped* function and a wrapped value, applying the wrapped function over the wrapped value.
- `>>=` takes a wrapped value, a function that *returns* a wrapped value, and then returns that wrapped value.

Moreover, because it is a monoid *over* endofunctors, this means:

- We can chain these together *associatively*
- Our final return type is an endofunctor in the *same* category
    - which in turn means, our final type *stays* in the same category.
