---
layout: post
title: "An interactive command line interface for your applications"
published: true
category: general
tags: [C++]
---

In my work I often develop applications meant to run for a long period of time,
without a classic interaction with the user (i.e., a gui).
Sometimes, they are applications that run in a server, sometimes in a custom board,
but almost never they are desktop applications.
Their other characteristic is that they're not cpu bounded applications,
i.e., they're not the kind of number crunching applications that you start
and then you wait for an output.

I soon found out that it's very useful to have some kind of console
to interact with my applications, particularly the embedded kind,
where the software is running around the clock,
so you can easily monitor, configure and manage your system.

Cisco routers have a command line interface, and so many devices that run unattended.

If you're working on similar software, you should definitely considering adding
a command line interface at least for debugging purpose. For example: it wouldn't
be great if you could connect using a telnet client to your embedded software to
ask some internal state, to view a dump of some internal structure, to
change the log level at runtime, to change the working mode, to enable or disable some modules,
to load or unload some plugin.

When you're in production, the benefit of an interactive [possibly remote] command line is obvious.
But think at the initial phases of development. When you're writing the first prototype of
a [@@@ IO bounded???] application. Chances are that you start writing some core features,
some protocol, some module that does IO, just to speak with the real world.
How do you test it? You can write a `main` that drives your piece of software,
but usually you need some FSM... or a more interactive solution :-)

Let's imagine: you're writing your protocol stack to speak with a legacy machine. You've got
primitives to tell the machine to start the electric motor, to stop it, to change direction, and
the notifies for meaningful events. You can insert in your `main` a command line interface,
and plug the various primitives to its commands. So you can start to test 
use cases complex at will on your module, in an incremental manner (well,
this requires also to be able to write incremental modular code, but this is
a subject for another post :-).

As every good developer, when I need something, I start by looking at open source libraries,
to see if there is something that works for me. Unfortunately, I couldn't find anything
completely fitting my needs. In particular, the vast majority of libraries available
worked only on linux, or they weren't libraries at all, but applications
in which you have to hook external programs to commands. None of them
provide remote sessions. Few were written in C++, none of them in modern C++.

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
(in case you want keep single threaded)
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