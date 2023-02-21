# Vulkan Tutorial
I am following a vulkan tutorial on youtube, found [here](https://www.youtube.com/watch?v=lr93-_cC8v4&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=2).

There are some coding style things that I don't agree with, and sometimes I'll change things depending on my own feelings.

## Video 1 - Introduction 
No notes; they set up a build environment using vscode on macos which I am not going to use.

## Video 2 - Create a Window
Created a window for our thing to live in!

## Video 2 - Graphics Pipeline Overview
Learned about the graphics pipeline, which parts of it are configurable and what a shader is. Implemented and compiled an extremely simple shader using glsl.

![Screendump from youtube showing an outline of a graphics pipeline](assets/readme/pipeline.png)

Very interesting! I'm excited to learn more about all of this stuff, we'll see when the anxiety and/or ADHD hits 😅

## Video 3 - Device Setup
Learned how to contact the graphics device, and set up a kind of interface (?) to it. This video was hard to grok, partly because it relied on downloading a bunch of boilerplate code. I do think is the right call by mr Galea (the author of the tutorial series) because there was quite a bit of it and doing all of this setup was already a bit challenging. It is also a bit tricky to keep track of all the components since it isn't trivial to understand how they will all interact in the final product.

## Video 4 - Fixed Function Pipeline Stages
Configured the fixed function pipeline stages. The configuration pattern of creating structs and passing them to Vulkan is a bit foreign but intuitive - I wish the tutorial would have taken the time to explain the division of responsibility a bit more here, it feels as though we're setting up something that is the way we want it but skipping past the naïve approach of just hard-setting things first. Maybe things will become a bit clearer, but this video was a lot of typing with a bit less understanding even though I looked up the concepts and skipped back whenever I missed something.

I also figured out a game I might want to make! More on that in another repo. Maybe.

## Video 5 - Swap Chain Overview
I now understand a tiny bit about swap chains, image buffers and difference image buffer modes - or at least the existence of them. All the errors are now gone, and the code opens... an empty window! Yay!

## Progress

![](https://geps.dev/progress/13)

- [x] Video 1 - Introduction
- [x] Video 2 - Graphics Pipeline Overview
- [x] Video 3 - Device Setup
- [x] Video 4 - Fixed Function Pipeline Stages
- [x] Video 5 - Swap Chain Overview
- [ ] Video 6
- [ ] Video 7
- [ ] Video 8
- [ ] Video 9
- [ ] Video 10
- [ ] Video 11
- [ ] Video 12
- [ ] Video 13
- [ ] Video 14
- [ ] Video 15
- [ ] Video 16
- [ ] Video 17
- [ ] Video 18
- [ ] Video 19
- [ ] Video 20
- [ ] Video 21
- [ ] Video 22
- [ ] Video 23
- [ ] Video 24
- [ ] Video 25
- [ ] Video 26
- [ ] Video 27
- [ ] Video 28
- [ ] Video 29
- [ ] Video 30
- [ ] Video 31

