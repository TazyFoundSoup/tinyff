# Streams

tinyff is essentially a bootstrappable engine which can parse and write different forms of
multimedia. This is the only thing it does, being an engine that needs a chassis and wheels
to be able to work properly. This is where the only thing that tinyff will write to comes into
place, streams.

## How streams work?

Data before being sent into tinyff to do its function must be put into a stream. While tinyff
is still young, this is very subject to change, however the main idea is that each stream has
a user which is just a way for your streams to remember things in between the calls. It could
be actual format data (for example, a raw PNG datastream) or it could be pointing to another
external context. The second option will probably be more realistic for doing things like
cursor actions or storing length and position and other data:

```c
// Simple: user IS the data pointer (no state needed)
ff_stream s1 = { .read = simple_read, .write = simple_write, .user = raw_buffer };

// Realistic: user points to a struct that tracks the data + position
ff_stream s2 = { .read = mem_read, .write = mem_write, .user = &ctx }; // ctx.data = raw_buffer
```

Also in this example, it shows how you also have to pass your own reading and writing functions because
**tinyff is just an engine** and it doesn't understand your context. Most of the time,
you will probably use hosted layers on top of these streams to help you, such as files and other
forms of retrieval which will be built in which can be accessed with defining `USE_STDIO` (for stdio file retrival).

When defining read and write functions, make sure they follow the ff_read/write_cb file signiture:

```c
// Reading callback
typedef size_t (*ff_read_cb)(void *ptr, size_t size, void *user);

// Writing callback
typedef size_t (*ff_write_cb)(const void *ptr, size_t size, void *user);
```

