# Lua as your favorite Programming Language

I personally see myself as a generalist when it comes to programming, growing up on videos by engineers such as Michael Reeves and William Osman, I tend to code things *I* want to make.

And of course that entails looking for a medium to code as much things as I can with the highest amount of enjoyment that can be derived from it. I do not want to constantly be changing languages for a specific project. Categories in which I believe Lua is a perfect fit in.

Lua is a performant, general-purpose language, the likes of Python and Ruby. But why choose it over the alternatives?

## Simplicity

Hot take, but I really enjoyed the feeling of writing in C. There is a much heavier sense of accomplishment to hacking together something that works in C than in a higher level language such as Python as it feels like you can say that you made it brick-by-brick, with as little dependencies as possible.

Now it would be arguable to contest that Lua can be strictly faster than Python as modern-day technologies allow both interpreted languages to use *just-in-time* compilers (code compilation during runtime) which then set the precedent for the speed, not the language. But it is straight up factual to state that Lua is *simpler* than Python and C, boasting only 22 keywords (C has 32!) and a syntax that feels very uncompromising.

A design choice of Lua that took a while to grow on me was the use of `do ... end` syntax to denote blocks, but I found myself preferring it over Python's reliance entirely on indentation. This does not stem from enforcing indentation in the syntax itself, but the *connectedness* I *feel* from explicitly enclosing related code in blocks. Moreover, when you get used to this syntax and encounter a line such as `if a > 5 then print(a) end`, it immediately makes sense what happens, even when you haven't encountered it in one-line form previously.

Lua actually has a built-in character-based string patterns for matching, opting to use that instead of adding built-in regex support as regex in of itself would be bigger than the language itself. This conscious design choice is incredibly admirable, attesting to its dedication to simplicity.

There is definitely merit to making complexity (your code) emerge from simplicity (your building blocks).

## Tables

When you ask someone about this language, it would be very unlikely for them to not bring up tables as the only data structure in Lua. And to me, I think the existence of tables in of itself doubles down on this idea of complexity emerging from simplicity, in how you can implement pretty much anything you will ever need with tables, despite it not being obvious at first glance.

Arrays? Tables. Hashmaps? Tables. Bags? Tables. Linked lists and graphs? Tables!

Moreover, tables even allow you to define OOP-style constructs just by letting you interface with a little bit of metaprogramming. Syntax sugar even exists for self-referencing methods (for any defined function key you can pass in a `self` parameter by using `:` for access instead of `.`)!

Oh, and it's not really that much of a big deal to adjust to 1-indexing.

## Homebrewing

Lua being simple forces you to implement your own libraries for your usecases. I believe this is where a lot of the charm of the language comes from, a design principle undoubtedly inherited from C.

Not only does this ensure your program is as compact as it can be, it also helps you be much more intimate with the code you are writing.

Setting up your codebase with libraries specifically catered for you and your team, and seeing it all come together is incredibly gratifying as Lua provides an easy to use interface for importing user-defined modules.

Not only that, but with Lua LSP type annotations you can also guarantee that your documentation pretty much writes itself, which is a big win for developer experience.

## Usecases

Lua is as general purpose as it can get, ranging from creating games in *Roblox* and the *love2d* framework, modding APIs for various games such as *Hades 2* and *Binding of Isaac: Rebirth*, to being used as configuration languages for *Wezterm* and *Neovim* (the best IDE), and defining desktop widgets via *Aylur's GTK Shell*, and even writing embedded C!

All of that while being a pretty solid scripting language (I did some problems of *Advent of Code* 2024 in Lua!).

It is very hard to miss Lua whenever you immerse yourself in programming as it lends itself as not only a language, but a powerful *framework* for creating interfaces.

The sheer applicability of Lua allows you to rapidly improve as a software developer as you can practice workflows in your controlled environment and release cycles. I cannot stress how important it is to start learn programming with a fast feedback loop as a lot of the concepts and theories can easily be missed with the lack of visualization.

## Conclusion

I have programmed in a lot of languages in search of one that matches my philosophy and preferences, and I have found all of that in Lua. I encourage you to try it with its many usecases as you might discover that its your favorite language too.
