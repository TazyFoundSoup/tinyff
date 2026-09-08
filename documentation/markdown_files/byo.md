# BYO* philosophy

Since tinyff is 100% embeddable on any platform that can run c99, it lets you give it resources
so it can run better on any system. The biggest one (and only for now) is the ability to bring your
own memory allocator to a project, using your exposed functions so tinyff can access them and use
them for memory options.

However, if your project is running on a hosted environment, tinyff can use the `USING_HOSTED` c definition
to use standard stdlib functions instead of setting up an adapter which already exists

> We would put a code example here, but tinyff just had to be rewritten recently because watson
had a breakdown and got angry

