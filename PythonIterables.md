---
id: PythonIterables
aliases: []
tags:
  - articles
---

# Understanding Python Iterables: Generators and Laziness

It is inarguable that one of Python's strongest suits is how it deals with *iterables*, or traversable objects.

When you're new to Python, something that immediately sticks out is the syntax of the `for` loop.

```python
# prints 0 to 9
for i in range(10):
    print(i)
```

You'll find that Python strictly enforces the `foreach` control structure that can be found in other languages, and if you're used to the C-like way of writing `for`-loops, this might have, pun intended, thrown you into a loop.

But in understanding this fundamental concept in Python, we actually allow ourselves to write more elegant and terse code. So let's incrementally tackle this topic.

![image](https://github.com/user-attachments/assets/cfdcccea-b92d-4d51-adf6-64de62d3f673)

## Iterators

We'll start by defining *iterators* as objects we can traverse through one item at a time. Think anything you can pass into a `for` loop.

> Note that this *traversal* cannot go backwards.

Furthermore, we can actually create our own `iterable` classes.

```python
class Fibonacci:
    def __iter__(self):
        self.a = 0
        self.b = 1
        return self

    def __next__(self):
        temp = self.a
        self.a = self.b
        self.b = temp + self.a

        return temp

for n in Fibonacci():
    print(n)
```

All that we have to define are the `__iter__` and `__next__` methods!

`__iter__` returns the object to be used for iteration whenever our object is invoked in the context of an iterable (in this case it is invoked as the iterable for a `for` loop). In this example, we use it as an *initialization* function of sorts, but you can just think of it as the *constructor* for your iterable instance.

> If that had too much jargon, all that you need to know is that what `__iter__` returns is the object to be used in iteration!

`__next__` is the function that gets called on every iteration, its return being the value for that current loop.

> Side note: dunder (double underscore) methods are just special methods for objects that you can call by passing the object in the dunder method's basename (without the underscores), or by invoking the dunder method on the object as an attribute.

An example of this is:

```python
a = Fibonacci() # we create our object (this isn't an iterable yet)

# Let's try calling the __next__ dunder method
next(a) # AttributeError: 'Fibonacci' object has no attribute 'b' (because iter hasn't been called)

b = iter(a) # We bring our object into an iterable context (this is done implicitly in for loops)

# both ways of calling the dunder method works
next(b) # 1
b.__next__() # 1

next(b) # 2
next(b) # 3
next(b) # 5
```

For the keen-eyed, you might have notice that even though we're allowed to traverse this iterable using `__next__` or a `for` loop, you can actually keep getting the next value *infinitely*!

![image](https://github.com/user-attachments/assets/ba9d772c-1681-40b5-bb72-568617f64b4a)

If this infinite looping behavior is not your intention, you can simply add a *base case* to your `__next__` method, by raising a `StopIteration` exception.

```python
def __next__(self):
    # stop the looping 
    if self.a > 100:
        raise StopIteration

    temp = self.a
    self.a = self.b
    self.b = temp + self.a

    return temp
```

This allows your `for` loops to stop iterating on your set condition. However if you try to call `__next__` on an iterator that has hit `StopIteration`, it will throw a runtime error.

In fact, we can see these dunder methods `__iter__` and `__next__` on any iterable object in Python such as lists and tuples! We can use the `__dir__` dunder method to check for all of the methods found in an object.

```python
a = [1,2] # a list, a common iterable

print('__iter__' in dir(a)) # True
print('__next__' in dir(a)) # False
```

That's odd, the list object has an `__iter__` method, but no `__next__`! Why is that?

## Generators

![image](https://github.com/user-attachments/assets/f46e1279-9046-4ef4-b1bd-5ecc014cc3eb)

Generators are special kinds of functions that instead of returning a single value, returns an *iterator* object. But instead of using the `return` keyword, it *implicility* returns the iterator object using `yield`. Let's see this in action by reimplementing our Fibonacci iterator.

```python
def Fibonacci():
    a = 0
    b = 1

    while a < 100:
        temp = a
        a = b
        b = temp + a

        yield a


a = Fibonacci() # our iterator object

print(next(a)) # 1

for num in a:
    print(num)  # 2 .. 144
```

So how do we parse this syntax? On the initial call of our function `a = Fibonacci()`, this is when we got our iterator object, as seen by how we could call `__next__` and use it as an iterable in a `for` loop, but as we can clearly see, there is no explicit definition for the `__next__` method, as it is baked into the logic of the definition itself.

Now what does this mean? Lets try to follow the execution of our iterator on the first 3 `__next__` calls (or `for` loop iterations):

```
--- first __next__ ---

a is defined
b is defined

we enter the while loop
we move forward with the fibonacci pattern
we return a

--- second __next__ ---

we enter the while loop
we move forward with the fibonacci pattern
we return a

--- third __next__ ---

we enter the while loop
we move forward with the fibonacci pattern
we return a ...
```

We can see here that for every `__next__` call, we can think of our iterator running through our defined function up until it hits a `yield`, where it uses that value to return for that specific iteration. Moreover, for the next iterations, we simply pick up from the line after the `yield` where we left off.

> This means that instead of having to raise `StopIteration` by ourselves, we can simply just let the function exit, greatly simplifying our conditions.

> Note that this also means `return` in generators become analogous to `raise StopIteration`! (and are only provided values in advanced cases we won't cover here).

So really, we aren't learning anything new here in the context of iterators, but a terser syntax and a more accessible interface for creating iterators!

Let's go back to our cliffhanger from the last section, by taking a look at the `__dir__` of a generator.

```python
def a():        # a function becomes a generator in the presence of a `yield` keyword
    yield 3

x = a()         # we create our generator object

print('__iter__' in dir(x)) # True
print('__next__' in dir(x)) # True
```

Since generators do have instances of the `__iter__` and `__next__` method, we can confirm that it is infact an iterator. And how is this relevant?

![image](https://github.com/user-attachments/assets/1433856a-3fda-49d3-9c1c-1afec52ef3aa)

Recall that:
- *Any* function becomes a generator when it has a `yield` inside of it.
- Generators are simply functions that return an iterator object, with it's `__next__` function as its definition (kind of)
- `__iter__` method is used to implicity return the object used for iteration whenever the context calls for it
        - These contexts include being casted into a iterator with `iter`, being used as an iterable for a `for` loop, and as we'll later learn: being *unpacked*.

We can infer that, under the hood, native Python iterables define their `__iter__` methods as generators, making the `__next__` absent from the class definition, but not from its iterator instance.

```python
a = [1,2]

print('__iter__' in dir(a)) # True
print('__next__' in dir(a)) # False

b = iter(a)                 # we bring it into the context of an iterator

print('__iter__' in dir(a)) # True
print('__next__' in dir(a)) # True
```

Putting a generator in place of the `__iter__` method is not only an elegant way of writing it, but it also serves a purpose for *encapsulating* the `__next__` method inside of the iterator instance itself, since if you can remember-- we weren't even able to use `__next__` until our object became an iterator anyway!

## Laziness

Besides the fact that knowing how a language works is pretty cool, generators (and by extension, iterators) also serve as a massive point of optimization for most Python programs. Which we can just derive from its name, it generates values.

In fact, I would go as far as to say that coding in Python becoems infeasible without generators, all because of their *lazy evaluation*.

Lazy evaluation is *not* a new concept in Computer Science, in fact it's a byproduct of functional programming, wherein we may delay the execution of a transformation on data up until the point where we actually need it.

A good analogy of this is being given ingredients for a meal that you have to cook tonight. With all of those ingredients and your cooking gear and whatnot, you can argue that you technically already have the meal, you are simply not making it until tonight. This could be for various reasons, one of which could be that it is easier to store the ingredients than the entire meal itself.

![image](https://github.com/user-attachments/assets/8baa39c0-aa9c-41a6-9467-4f04647cdcfa)

When we created the `Fibonacci` generator, we could argue that we did have the Fibonacci numbers up to 144, we just simply weren't calculating them until we had to. This approach saves resources from our program as we don't end up keeping all of the memory we need *up front*, i.e. if we wanted to keep the Fibonacci numbers less than ten million, but we're still *technically* storing the numbers.

Think of our ubiquitous `range` function, an example of a generator, as we know that `range` doesn't *keep* a list in memory, but rather *generates* the numbers sequentially (we know this by the fact that we can only traverse the iterable in the direction indicated by the `step` parameter).

In fact, you should go ahead and try to implement some pre-existing generators in your free time, to see how well you understand Python! Notable generators are `enumerate`, `zip`, `map`, `filter`. As for `range`, it actually requires a bit more pre-requisite knowledge that you may originally expect, so we'll be covering it on a follow-up article.

Good luck!
