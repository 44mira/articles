# Rust-like Iteration in Lua

> This reading was inspired by Neovim's api `Iter` namespace. Be sure to check that out!

It is no secret that I adore [functional patterns](https://dev.to/if-els/functional-patterns-the-monad-cc2) in programming. So any excuse I can get to use them is more than welcome in my code.

One of these patterns involve using functions to abstract [transformations over lists](https://dev.to/if-els/functional-patterns-interfaces-and-functors-359e), which we can do in a language like Python as:

```python
a = [1,2,3]

map(lambda b: b + 1, a)
# or [ b + 1 for b in a ]
```

And in Lua as so:

```lua
function map(tbl, fn)
    local ret = {}
    for i, v in ipairs(tbl) do
        ret[i] = fn(v)
    end

    return ret
end

local a = { 1, 2, 3 }
map(a, function(b)
    return b + 1
end)
```

But this way of transforming, that being passing the iterable to be transformed as an argument, is quite cumbersome. Especially when it comes to composing or chaining these transformations together.

```python
a = [1, 2, 3]
reduce(lambda ac, x: ac * x, filter(lambda c: c > 2, map(lambda b: b + 1, a)))
```

Elixir does a great job of eliminating this redundancy, by making it convention to have the iterable as the first argument, being able to leverage syntax sugar, implicitly passing the iterable as the first argument of the consecutive transformations.

```elixir
a = [1, 2, 3]

a
|> Enum.map(fn b -> b + 1 end)
|> Enum.filter(fn c -> c > 2 end)
|> Enum.reduce(fn x, ac -> x * ac end)
```

## Leveraging state

But using syntax sugar isn't the only way we can achieve this way of chaining transformations without deeply nesting our parameters. In Elixir, they have to do that because of state being immutable, but we aren't working within the confines of a strictly functional language when we are talking about Lua.

Let's take a look at how Rust tackles this problem.

```rust
let a: Vec<i32> = vec![1,2,3];

a.iter()
 .map(|b| b + 1);
 .filter(|&&c| c > 2);
 .fold(1, |ac, x| ac * x);
```

> I use fold here with an initial value as to avoid doing an `.unwrap()` at the end.

Elegant chaining is achieved without the use of syntax sugar! And if we really read into it, we are first wrapping it into the `std::iter` *class* (trait to be more technically correct), which has *self*-returning methods!

So to achieve this in Lua, all we have to do is figure out how to *wrap* new methods ontop of our existing array so we can chain the transformations.

## Metatables

Lua does *not* have "classes" in the strict sense, but we can implement them using *metatables*. In fact, we can straight up just ignore the idea of a class and tackle this problem as a matter of *implementing* interface methods for our array.

First, we define a table that's going to contain all of the implementations we want to apply to our array, and a *constructor* function that sets that table as our *indexing* table.

```lua
local Iter = {}

function iter(tbl)
    -- do note that this will replace any existing metatables on tbl
    setmetatable(tbl, { __index = function(self, key)
        return Iter[key]
    end})
    return tbl
end
```

What's happening here is we are telling our `tbl` array that whenever we cannot find a specific key in our array (which we won't, assuming the table is just used as an array and not a hashmap), to call the `__index` function instead of just returning `nil`.

This `__index` function is what we call a [*metamethod*](https://www.lua.org/pil/13.html), functions that get called implicitly by Lua under specific conditions. In the definition we are saying to look for the key in the `Iter` table whenever it doesn't exist in the current table. Moreover, Lua actually provides a shorthand for this.

```lua
function iter(tbl)
    setmetatable(tbl, { __index = Iter })
    return tbl
end
```

So clean! However, there is an issue here, that being we are mutating our state *directly*. Transformations are usually done on *copies* of arrays as to maintain immutability, so let's do that here.

```lua
function iter(tbl)
    local cpy = {} 
    
    for i, v in ipairs(tbl) do
        cpy[i] = v
    end
    
    setmetatable(cpy, { __index = Iter })
    return cpy
end
```

> Do note that this method of copying will still use references when copying over nested tables, so be careful about that!

Now we can do our magic.

## Implementations

We'll define the implementations on our `Iter` table.

```lua
function Iter.map(self, fn)
    for i, v in ipairs(self) do
        self[i] = fn(v)
    end

    return self
end
```

This works, but it doesn't exactly solve our initial issue of passing in the iterable as an argument. Here we can leverage syntax sugar, ontop of our leveraging state, to get there!

Lua provides syntax for `:` calls, which can be used in definitions to declare an implicit `self` argument as the first parameter of the function, and in calling to use the left-hand side of the `:` as the `self` argument. Here it is in action.

```lua
function Iter:map(fn)
    for i, v in ipairs(self) do
        self[i] = fn(v)
    end

    return self
end

local arr = iter({1,2,3})

arr:map(function(a) return a+1 end)
   :map(function(b) return b*2 end)
```

And this works, and looks very cool (which should ultimately be the standard for all of your code). All that's left now is to implement the rest of the transformation functions you need (a little brain exercise for the reader) and you're set.
