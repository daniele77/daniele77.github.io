---
layout: post
title: "An interactive command line interface for your applications"
published: true
category: general
tags: [C++]
---

In my work, I often develop applications meant to run for a long period of time,
without a classic interaction with the user (i.e., without a proper UI).
Sometimes, they are applications that run in a server, sometimes in a custom board,
but almost never they are desktop applications.
Their other characteristic is that they're not CPU bound applications,
i.e., they're not the kind of number crunching applications that you start
and then wait for an output.

Soon enough, I realized that it's very useful to have some kind of console
to interact with my applications, particularly the embedded kind,
where the software is running around the clock,
so that you can easily monitor, configure and manage your system.

Cisco routers have a command line interface, and so many devices that run unattended.

If you're working on this kind of software, you should definitely considering adding
a command line interface at least for debugging purpose.
For example: it wouldn't be great connecting to your embedded software using a telnet client
and ask the internal state, view a dump of some internal structure,
modify the log level at runtime, change the working mode, enable or disable some modules,
load or unload some plugin?

When you're in production, the benefit of an interactive (and possibly remote) command line is obvious.
But consider the initial phases of development, too.
When you're writing the first prototype of
an application doing I/O on custom devices,
<!-- a I/O bound application [### trovare il giusto termine. in questo caso reactive system sarebbe meglio]. -->
chances are that you start writing some core features to speak with the real word:
some protocol, some module that does I/O, ...
How do you test it? You can write a `main` that drives your piece of software,
but usually you need some sort of FSM to interact correctly with the hardware.
Either that, or a more interactive solution :-)

Let's imagine: you're writing your protocol stack to speak with a legacy machine. You've got
primitives to tell the machine to start the electric motor, to stop it, to change direction
as well as the notifies for meaningful events.
Now you can insert in your `main` function a command line interface,
and register a command for each primitive.
In this way you can start to test use cases complex at will on your module,
in an incremental manner
(of course this requires also to be able to write incremental modular code, but this is
a subject for another post :-).

## Reinventing the whell, as usual...

As every good developer, when I need something, I start by looking at open source libraries,
to see if there is something that works for me. Unfortunately, I couldn't find anything
completely fitting my needs. In particular, the vast majority of libraries available
work only on linux, or they aren't libraries at all, but applications
in which you have to hook external programs to commands. None of them
provides remote sessions. Few are written in C++, none of them in modern C++.

Eventually, I wrote my own library, in C++14. It's available on my github page,
[here](https://github.com/daniele77/cli). It has production code quality,
and has been used in several industrial projects.

A brief summary of features:

* Cross-platform (linux and windows tested)
* Menus and submenus
* Remote sessions (telnet)
* Command history (navigation with arrow keys)
* Autocompletion (with TAB key)
* Async interface
* Colors

It has a dependency from `boost::asio` to provide an asynchronous interface
(when you want a single thread application)
and to implement the telnet server.

That's all I needed for my projects. When I have a remote board running my software,
[@@@ in campo, lontana etc. etc], I find it very handy to telnet on a specific port
of the board to get a shell on my application. I can have a look at the internal
state of the software, increase the log level when something strange happens,
even give commands to [@@@]. @@@

The `cli` library can have several uses, besides the remote supervision of embedded software.
It can be a console for UI-less applications, like game servers;
as a way to configure network devices,
or be useful to test library code.