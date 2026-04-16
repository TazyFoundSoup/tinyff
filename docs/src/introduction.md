# welcome to tinyff

tinyff is a small, embeddable C library for reading and writing sensory file 
formats — images, audio, 3D, fonts, and more.

---

## why tinyff?

over here are tinyff inc, we hate the selfishness that gets shipped along with all these other multimedia libraries. they always assume the prescense of an allocator and a file which contains the media. so to help out with people need more accessibility for embedded and unsupported systems, tinyff stands by it [bring your own allocator](concepts/allocators.md) and [custom streams](concepts/streams.md) to make tinyff be the last multimedia library you will ever have to use.

---

## what can i do with it?

not much yet since tinyff has just started (and right now is a perfect time for you to consider becoming a contributer), check out the [github](https://github.com/tazyfoundsoup/tinyff).

---

## where do i start?

> hey there, just letting you know tinyff is still nonproduction level so lots of things will still be a bit messy. maybe consider being a contributer

if you're new — head to [building tinyff](getting-started/building.md) and 
get it compiling. then [load your first image](getting-started/first-image.md).

if you want to understand the design — [the context](concepts/context.md) is 
the best place to start.

if you want to contribute — read about [format stewardship](contributing/stewardship.md). 
own a format. ship something real. put it on your CV.

---

tinyff is open source, actively developed, and looking for format stewards. 
if you build something with it, we'd love to know.