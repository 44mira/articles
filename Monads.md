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

But somewhere along that road, after all those times sunk into understanding this pattern— I felt like I could comfortably say that I had reached an understanding on it. It stopped being an "Aha! I think I got it!"

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

- `fmap` takes a *function* and then a *wrapped* value, mapping the function *over* the wrapped value.
- `<*>` takes a *wrapped* function and a wrapped value, applying the wrapped function over the wrapped value.
- `>>=` takes a wrapped value, a function that *returns* a wrapped value, and then returns that wrapped value.

Moreover, because it is a monoid *over* endofunctors, this means:

- We can chain these together *associatively*
- Our final return type is an endofunctor in the *same* category
    - which in turn means, our final type *stays* in the same category.

## Monads in Practice

Lets say we have an arbitrary *cipher*, wherein characters in the alphabet are converted to any other arbitrary character in the alphabet. We keep this cipher as a *dictionary* (also known as a *hashmap*) that takes an encoded `Char` and returns the decoded `Char`.

```hs
import qualified Data.Map as M

cipher :: M.Map Char Char
cipher = undefined -- definition omitted
```

Now we create a `decode` function, remember that this operation *might* fail (when the key doesn't exist), so we have to use the `Maybe` data type.

```hs
decodeChar :: Char -> Maybe Char
decodeChar = (`M.lookup` cipher)    -- wrapping in backticks turn a function into infix form
                                    -- so we're partially applying the second argument

-- equivalent to:
-- decodeChar = flip M.lookup cipher   -- C-combinator, flips the arguments
```

`M.lookup` is a function that, well, does a lookup on a `M.Map`, with your given key (in this case our encoded `Char`), and returns a `Maybe Char`, because the key might not exist in the `M.Map` in which case it will return a `Nothing`.

Now say we want to *then* lowercase the resulting characters of our decoding. We would have to compose `toLower` function to our call, but there's one issue!

```hs
toLower :: Char -> Char
```

`toLower` doesn't take a `Maybe Char`! If we recall the available functor composers we have, this is actually no problem as this is just an `fmap`.

```hs
decodeChar = fmap toLower . (`M.lookup` cipher)

-- or even ...

-- we wrap the function in Maybe, then apply using `<*>`
decodeChar = (pure toLower <*>) . (`M.lookup` cipher)
```

Now say we want to convert our lowercased character into its **ASCII** value, we would have to do *another* composition on `fmap`.

```hs
decodeChar :: Char -> Maybe Int
decodeChar = fmap ord . fmap toLower . (`M.lookup` cipher)
```

And we can continue this for god knows how long, but the issue is starting to rear its head. This type of composition is kind of annoying to write! Let's try to write it using `Maybe`'s Monad `bind`.

```hs
decodeChar :: Char -> Maybe Int
decodeChar c =
  M.lookup c cipher
    >>= return . toLower
    >>= return . ord
```

This works! And is so much cleaner. We can see `return` make its appearance here, and the reason it's called `return`, is not because it *returns* value in the imperative sense, but because it's usually the last call you make inside a function required by `bind`, because you have to *return* to your Monad type!

Let's think about that even deeper and realize that the reason we have to *return* to our Monad is because: while we're inside the function required by our `bind`, we implicitly *"unwrap"* our value, so we can do our own logic on it, before rewrapping it at the end, so we can now completely abstract "unwrapping"! 

![image](https://github.com/44mira/articles/assets/116419708/4b2492d7-3480-44bc-af9f-fd1b9fba80be)

> Note the double-quotes on *unwrapping*, it's because we *aren't* actually unwrapping the value, as that might cause a runtime error depending on your data type's logic (unwrapping a `Nothing` causes an error).

```hs
decodeChar :: Char -> Maybe Int
decodeChar c =
  M.lookup c cipher
    >>= return . ord . toLower
```

So now we can do our usual compositions, inside our Monad type! Moreover, Haskell has an even more legible syntax for this, the `do`.

```hs
decodeChar :: Char -> Maybe Int
decodeChar c = do
  decoded <- M.lookup c cipher -- Make value inside Maybe accessible

  return . ord . toLower $ decoded
```

And in this form, the return being at the end of the call even resembles an *imperative* language!

Note that because we've done our functions without unwrapping, we never risk unwrapping into a runtime error! Moreover, when you implement a `Monad`, it's up to you how you want to do their compositions, as long as you follow the 3 axioms of *left-identity*, *right-identity*, and *associativity*!

> These axioms won't be covered here, but they shouldn't get in your way that often (and you'll rarely need to implement a monad in the first place)

```hs
concat . map (replicate 2) $ [1, 2, 3]  -- [ 1, 1, 2, 2, 3, 3 ]
concatMap (replicate 2) $ [1,2,3]

-- `bind` for the List monad is equivalent to `concatMap`!

[1,2,3] >>= replicate 2
```

## Imperatively speaking,
> But how is the concept of the Monad relevant to languages outside of Haskell and other functional languages? Is it relevant?

First of all, it's not all about you, imperative programmer! Second, leveraging the concept of monads allow us to write *succinct* (well, after you write the code to force FP into your imperative program), and *chainable* code.

Behold, the `Maybe` monad in Python:

```python
class Maybe:
    def __init__(self, value) -> None:
        self.value = value

    @staticmethod
    def just(value):
        return Maybe(value)

    @staticmethod
    def nothing():
        return Maybe(None)

    def __str__(self) -> str:
        match self.value:
            case None:
                return "Nothing"
            case x:
                return f"Just {x}"

    def __eq__(self, other) -> bool:
        return self.value == other.value

    def bind(self, *fns):

        for fn in fns:
            if self.value == None:
                return self.nothing()
            self.value = fn(self.value).value

        return self.just(self.value)


def search(n, lst) -> Maybe:
    for i, v in enumerate(lst):
        if v == n:
            return Maybe.just(i)
    return Maybe.nothing()
```

```python
result = search(4, [3, 4, 2]).bind(lambda a: Maybe.just(a+5))
print(result) # Just 6

result = search(5, [3, 4, 2]).bind(lambda a: Maybe.just(a+5))
print(result) # Nothing
```

Incredible and outrageous. *Just* the perfect mix.

---

And that just about does it! I hope you enjoyed this entire series, it has been about a year in the making. I hope you learned something, and most importantly, enjoyed the time you invested! If you have any questions, feel free to contact me in my socials, or in the comments down below, I will try to make time.

> What's the problem?

![image](https://github.com/44mira/articles/assets/116419708/f21a9ca2-8bee-4304-be50-9382d490495e)

