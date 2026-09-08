# tinyff

tinyff is a generic, embeddable and very quite small, multimedia library written completely in C.
It is **not** trying to be ffmpeg, but instead going in a different direction of making it
completely embeddable and can run on any architecture using our [BYO*](byo.md) philosophy.
> Right now, tinyff is very WIP and most things will be unavailable, but there is work going on under the hood,
so if you know basic C and you're not scared of specs, why don't you join in?

tinyff currently, and in the future is aimed to feature:
- completely embeddable design with no assumptions of stdlib or OS
- very pipelinable with it's ability to take in data from streams and out of streams
- completely blind of any type of data, so you can send in files, network requests, even serial ports
- c99 compatible

 
