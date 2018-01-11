---
layout: post
title: C++ forward declarations
published: true
category: general
tags: [C++]
---

Sometimes, I discover that nowadays experts ###.

Recently, I heard two C++ experts speaking about forward declarations, and
one said he was surprised discovering that a reference function parameter
and function return can be forward declared.
And be careful: when I say "C++ experts" I'm not speaking about two random people
encountered in a commuter train in the morning, but two guys regular speakers
at international C++ conferences, with a great visibility and influence on the C++ programmers.
Well, it's not a new thing that most "young" C++ gurus ignore some topic like software design and architecture,
but forward declarations... wow, that's really C++ 101, and I think that understanding *why* they work
(let alone when they should be used) is important.

But what are forward declarations and why they're useful?

TODO: explain reduction compile time

And what's the basic rule?

One can simply deduce the rules for when forwarding declaration can be used
by understanding that the compiler need the whole type in two cases:

* when the compiler must generate code to allocate an instance of that type
* when the compiler must access a member (data or function) of that type.

To generate the code to allocate an instance (e.g., passing by value an object)
the compiler must know at least the size of the object but, ### (si può anche togliere)

When we're passing a parameter by reference (or by pointer),
the compiler generates the code to put a pointer on the stack,
so it doesn't need to know the size of the whole object
(while a pointer has standard size). In this case, a forward declaration is enough.

This example show the cases ###:

{% highlight c++ %}
// derived.h

#ifndef DERIVED_H_
#define DERIVED_H_

#include “base.h”
#include “part.h”
#include “indirect.h”
#include “reference.h”
#include “inline.h”

class Derived : public Base
{
private:
    Part part;
    Indirect* indirect;
    Reference& reference;
    Inline* inl;
// ...
};

inline int Derived::f()
{
    return( inl->g() );
}

#endif

{% endhighlight %}

becomes:

{% highlight c++ %}
// derived.h

#ifndef DERIVED_H_
#define DERIVED_H_

#include “base.h”
#include “part.h”
#include “inline.h”

class Indirect;
class Reference;

class Derived : public Base
{
private:
    Part part;
    Indirect* indirect;
    Reference& reference;
    Inline* inl;
// ...
};

inline int Derived::f()
{
    return( inl->g() );
}

#endif

{% endhighlight %}
