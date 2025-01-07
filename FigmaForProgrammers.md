# Figma for Programmers

> This article assumes you have fundamental knowledge of HTML/CSS, and won't cover prototyping and design systems. Moreover, this is not a tutorial, but a brief overview of notable features within Figma.

It is very likely that a major roadblock you will encounter as programmer who does side-projects is tackling with how the the user interface should look like. This roadblock won't really go away unless you end up with a UI/UX designer on your team, if you even have a team in the first place.

And in contrast to other problems, you should immediately know how to resolve this; that being a mockup. However, this doesn't exactly get rid of the problem but actually introduces another problem into the process. You now have to learn how to make mockups, and make them effectively, and most importantly, *translatable* into code.

I had to deal with this when I decided to try and create my own custom desktop interface from scratch using the [*Astal*](https://aylur.github.io/astal/guide/getting-started/introduction) library, recognizing that in order to make the process productive and bearable, I had to grit my teeth and delve into UI/UX design, being primarily a programmer.

The time had come, I *had* to learn Figma. And these are the major takeaways I have from that experience.

Before I go into it, I would like to thank Sharmaigne Mabano and Kaye Jose for guiding me through the initial steps to properly setup a Figma mockup.

## Preparing your palette, literally

Like every hobby programmer, I once tried my hand at game development which also led me down the path of [pixel art](https://x.com/dotsbymira). And a good habit I picked up from that is by preparing *swatches* of colors at a corner of my canvas that I will use for the drawing I was going to make. Not only did this give the art more *direction* but also a bit more organization that is just pleasing to my OCD.

This is also a good idea to do in Figma as not only can you visually see the colors beside eachother, but you can also set *Library variables* for your current project, letting you refer to the colors via their names instead of their hardcoded hex codes.

> NOTE: Programmers love making variables for repeating things!

<!-- insert image here of palette -->

A good way of deciding colors for your palette is by getting existing ones online (a site I used a lot back when I was doing pixel art was [Lospec](lospec.com)), using [Realtime Colors](https://www.realtimecolors.com/?colors=050315-fbfbfe-2f27ce-dedcff-433bff&fonts=Inter-Inter), or as a last resort, making your own using Color Theory. Of course I won't be able to tackle color theory in an article about Figma but do check out [hue shifting](https://www.blue-canary.net/miniature-painting/painting-tips-and-guides/hue-shifting/) for creating more vibrant shades of colors.

> KEY FEATURE: Library variables

## On `div`s and `flexbox`es

In making an effective design, it is not only important for it to be visually pleasing, but also feasible for translation into code. I like to think this is where the programmer perspective really comes into play, by already thinking of the code while designing, you are ensuring a smooth transition into that medium.

That is, to equate Figma frames as `div`s. This doesn't mean to just imagine them as such, but to quite literally use them everywhere you would see yourself using a `div` in. This includes grouping anad nesting other `div`s together, and creating separate backgrounds for sections.

For example: when creating a mockup for a navbar, it is common to have a split design, to have a logo on the left and the menus on right. In this example, I am designing my taskbar which has workspaces on the left and laptop battery power on the right.

<!-- navbar example -->

We could do this by absolutely positioning everything and just figuring it out when coding, or:

<!-- navbar example but highlighted divs -->
> The divs are highlighted here for demonstration purposes

We could setup the mockup in a way that the divisions are already declared. Not only does this make the design process easier, but also makes the coding require less overhead from laying out the `div`s in a way that resembles the design.

Another reason to do this, and probably one of the best features of Figma, is that we can set our frames into [*auto-layout*](https://help.figma.com/hc/en-us/articles/5731482952599-Add-auto-layout-to-a-design) mode. This initially feels confusing to look at, but my god does it save so much time.

A good parallel to auto-layout is the `flexbox` property of `div`s, wherein the arrangement of content can be declared with properties, including padding, spacing, and alignment. If you only get to take away one thing from this article I hope it is this one. I cannot emphasize how much easier designing gets when you start using this feature, especially since it can just get translated into `display: flexbox` in CSS, or `flex` in Tailwind.

Auto-layout will allow you to create scaleable and responsive designs fast, and translate them into code even faster. If you start desigining with the idea of `div`s and `flexbox` in mind, you are setting yourself up for success.

> KEY FEATURES: Auto-layout and frames

## Components and variants

If you have used *any* web development framework in the past decade or so, you have probably encountered the idea of *component-based* interfaces. We can use this mindset in Figma as well as they provide the option to create components from frames. These components act like the ones you know from web frameworks, letting you make instances from the parent component by using copy-paste, and edit properties from instances independent from the parent and other related components.

Like palette swatches, I create a separate frame just to hold all of my parent components, for easy reference and editing.

<!-- insert picture of components -->

Moreover, components can be setup to have *variants*, that being the same component *technically* speaking, but with specific properties differing, which can be used to trigger appearance changes.

An example of this would be `hover=true` and `hover=false` variants. This naming is convention in Figma, denoting the properties that dictate that variant's appearance.

Setting up the variants allow you to make an instance of the parent component and simply assign the property in the instance, with a checkbox for boolean attributes, and a textfield for anything else.

<!-- insert pictures of variants -->

## Conclusion

Don't be scared to try out Figma (like I initially was)! I can attest to it being a pleasure to work with. Making mockups is definitely an underrated skill for programmers, often times increasing a team's size solely to add a designer for that purpose.

But I would be lying if I don't mention how immensely gratifying it is to create a mockup from your vision as fast as you can in Figma, and then code with a well-engineered design as reference.

Conclusion: Learn Figma, even as a programmer not looking for a UI/UX career.
