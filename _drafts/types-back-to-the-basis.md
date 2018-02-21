---
layout: post
title: "Types: back to the basis"
published: true
category: general
tags: [design, C++]
---

Lately, I had to choose a C++ library to use for an industrial product I'm working on.
I found what it seemed the best fit (according to some constraint I had),
however, some choices of the author left me bewildered about the library interface.
And not just me, since in the issue tracker I found other suggestions to improve the interface.

Don't get me wrong: it's a good and useful modern C++ library, that does what it claims to do.
I'm just using it as an example to make some considerations for a broader point of view.

So, let's review some things that I had done in a different way.

## What do 10 and 11 mean?

The library deals with a protocol, and the user has to specify the protocol version in every message
he wants to send (and this already could be improved).
The supported protocol versions are v. 1.0 and 1.1 (guess what protocol we're speaking about? :-)
so, in the public interface, the author provides an `int` to set the version,
where the valid values are `10` and `11` (checked at runtime with an `assert`).

It works. If you look at the examples that come with the library,
you see something like this:

{% highlight cpp %}
msg.version = 11;
{% endhighlight %}

and you understand immediately how it works.

Everything's fine, then? Well yes, the library is usable, but the interface could be improved a bit,
both in terms of readability and extensibility but, above all, to avoid bugs.

I suggested using an `enum class` instead of an `int`. Something like that:

{% highlight cpp %}
enum class Version { http_1_0, http_1_1 };
{% endhighlight %}

this is clear and extensible, and it's not too much code to write by the library's author.

If the implementation really requires an `int` (but I guess the author used an integer as a simple label, see later) you can add the type specifier and the initializers:

{% highlight cpp %}
enum class Version: int { http_1_0 = 10, http_1_1 = 11 };
{% endhighlight %}

(the type specifier is int by default, so it could be ommitted).

<p style="background-color:yellow;">ecco: aveva bisogno di un'etichetta ma ha usato un int (la cosa più semplice). le etichette sono enum, non int e non stringhe</p>

Together with other users, I proposed to the author to switch to an `enum` / `enum class`,
but he refused, replying:

> Really, 10 and 11 are "so different" from 1.0 and 1.1?<br>
> [...]<br>
>  A label is just a label, how is http_1_0 any different from just 10? It seems like that is just gratuitously
> adding identifiers. My intent is to keep this simple, without over-engineering when possible.

Yes, I indeed think that 10 and 11 are so different from 1.0 and 1.1.
And not because of the period between the two numbers,
but rather because I think a numeric type (e.g., int) has a different meaning than a label.
The author admits that he need a *label*, but I would rather not use an int for that purpose.
<span style="background-color:yellow;">[elaborate]</span>
When you look at an interface, the right type can tell you at first sight which values are valid.
When you read the code, you understand straight away what `Version::http_1_1` means, as opposed to 11.

Moreover, think of the general case: if I had to handle versions 1.13 and 11.3,
how can I differentiate them using the current mapping?
At the very least, there will be some ambiguity in the usage.

The author wrote that introducing the enum was "gratuitously adding identifiers" and that he wants to keep the library simple,
without over-engineering. The reality is that using an enum is not gratuitous but useful,
and by using an `int` instead of an `enum` he's not keeping simple the library,
but more complex and error-prone for the users.
C'mon, it's not much work using the `enum`, it doesn't even add any overhead.
On the contrary: you can also remove the runtime assert with the version check here and there:
because, when you have *labels* instead of *int*s,
checks are moved from run-time to compile-time.
Isn't this the first rule we learned with C++?
I believe that as a library author, one should provide the best interface possible, hiding the hacks internally.
Even when that means writing a lot of code (and this was not the case!) because the meaning of a library
is to factor complex and repetitive code in one unique place, so that each user does not need to repeat that code.
After all, a library is written once but used many times :-)

## Back to the basis

It's the main point of having types in the first place.
Using everywhere `int`s and `string`s is possible, but it's advisable to use the type that fits better your data.
Why?
Well, compare:

{% highlight cpp %}
const int version = 42; // my application version
const int http_version = 11;

req.version = version; // boom
{% endhighlight %}

to

{% highlight cpp %}
const AppVersion version{ 4, 2, 0 };
const beast::http_version http_version = beast::http_version_1_1; // enum class

req.version = version; // compile time error :-)
{% endhighlight %}

<p style="background-color:yellow;">ALTRO</p>

In another part of his library, the author uses a boolean to discriminate requests from responses when
creating a message.

And when he writes state machines, he uses something like that:

{% highlight cpp %}

int state;

...

switch(state)
{
// did accept
case 0:
    ...

// start
case 1:
    ...

// got message
case 2:
    ...
}
{% endhighlight %}

and guess... `state` is an `int`. It seems the author doesn't like `enum`s very much :-)
Instead of using explicit labels (that could be checked at compile time), he prefers to use `int`s
and write the label in the comments!

Why?! Give me only one good reason not to write instead:

{% highlight cpp %}

enum class State { did_accept, start, got_message };

State state;

...

switch(state)
{
case State::did_accept:
    ...

case State::start:
    ...

case State::got_message:
    ...
}
{% endhighlight %}

## A label is a label (and it deserves its own type)
This library's author uses basic types when he needs labels.
`int's for versions and states, 'bool's for request/response and so on...

Maybe this is the simplest thing he came up with, so that he doesn't need to introduce
new types. 
But types <span style="background-color:yellow;">[sono la cosa più importante e è per quello che abbiamo un linguaggio tipizzato etc...]</span>
Labels are labels, and labels deserve their own type. And 99 out of 100 the type
is an `enum` (`class`).
