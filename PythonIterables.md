---
id: PythonIterables
aliases: []
tags:
  - articles
---

# Understanding Python Iterables

It is inarguable that one of Python's strongest suits is how it deals with *iterables*, or traversable objects.

When you're new to Python, something that immediately sticks out is the syntax of the `for` loop.

```python
# prints 0 to 9
for i in range(10):
    print(i)
```

You'll find that Python strictly enforces the `foreach` control structure that can be found in other languages, and if you're used to the C-like way of writing `for`-loops, this might have, pun intended, thrown you into a loop.

But in understanding this fundamental concept in Python, we actually allow ourselves to write more elegant and terse code. So let's incrementally tackle this topic.

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

`__iter__` is the object that gets returned whenever our object is invoked in the context of an iterable (in this case it is invoked as the iterable for a `for` loop). In this example, we use it as an *initialization* function of sorts, but you can just think of it as the *constructor* for your iterable instance.

> If that had too much jargon, all that you need to know is that what `__iter__` returns is the object to be used in iteration!

`__next__` is the function that gets called on every iteration, its return being the value for that current loop.

> Side note: dunder (double underscore) methods are just special methods for objects that you can call by passing the object in the dunder method's basename (without the underscores), or by invoking the dunder method on the object as an attribute.

An example of this is:

```python
a = Fibonacci() # we create our object (this isn't an iterable yet)

# Let's try calling the __next__ dunder method
next(a) # AttributeError: 'Fibonacci' object has no attribute 'b' (because iter hasn't been called)

b = iter(a)

# both ways of calling the dunder method works
next(b) # 1
b.__next__() # 1

next(b) # 2
next(b) # 3
next(b) # 5
```

For the keen-eyed, you might have notice that even though we're allowed to traverse this iterable using `__next__` or a `for` loop, you can actually keep getting the next value *infinitely*!

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

That's odd, the list function has an `__iter__` method, but no `__next__`! Why is that?

## Generators
