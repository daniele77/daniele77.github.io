---
layout: post
title: "Reduce compilation times with extern template"
published: true 
category: general
tags: [C++]
excerpt_separator: <!--more-->
---

*I first wrote this post on [Simplify C++!](https://arne-mertz.de/){:target="_blank"},
Arne Mertz’s blog on clean and maintainable C++.*

----

In the last few years, the compilation times of C++ projects increased dramatically,
in spite of the availability of fast computers with multiple CPU/cores and more RAM.

<!--more-->

This is to a large extent due to:

- the fact that some elaboration moved from
  run-time to compile-time through templates and `constexpr`,
- the increasing number of header-only libraries.

Although the first is unavoidable (and in fact it's desirable),
the second is a questionable trend
usually only motivated by the convenience of distributing a header-only library
rather than providing a compilation mechanism whatsoever.
Since I'm myself guilty to have developed a few header-only libraries, however,
I won't address this issue here :-)

In some cases, build times can be reduced taking advantage of appropriate techniques,
such as
improving modularity,
turning off optimizations,
using the *pimpl* idiom,
forward declarations,
precompiled headers,
and so on.

In addition, C++11 introduced *extern template declarations*
([n1448](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2003/n1448.pdf){:target="_blank"})
that, to some extent, can help speed up compilation times.
This is analogous to extern data declaration
and tells the compiler not to instantiate the template in the current translation unit.

## How does extern template declaration work?

The simplest way to figure out how extern template declarations work
is to reason over a snippet of code.
Consider these files:

```c++
///////////////////////////////
// bigfunction.h

template<typename T>
void BigFunction()
{
    // body
}

///////////////////////////////
// f1.cpp

#include "bigfunction.h"

void f1()
{
    ...
    BigFunction<int>();
}

///////////////////////////////
// f2.cpp

#include "bigfunction.h"

void f2()
{
    ...
    BigFunction<int>();
}
```

This will result in the following object files
(on Linux you can check it yourself using the utility `nm`):

<!--
```
f1.o
    void f1()
    void BigFunction<int>()  // compiled first time
f2.o
    void f2()
    void BigFunction<int>()  // compiled second time
```
-->

```shell
> nm -g -C --defined-only *.o

f1.o:
00000000 W void BigFunction<int>()
00000000 T f1()

f2.o:
00000000 W void BigFunction<int>()
00000000 T f2()
```

Eventually, when the two object files are linked together, one `BigFunction<int>()` will be discarded
(that's the precise meaning of the symbol type "W" that `nm`
puts near the function).
Hence, the time used to compile `BigFunction<int>()` multiple times is wasted.

To avoid this waste, the `extern` keyword can be used:

```c++
///////////////////////////////
// bigfunction.h

template<typename T>
void BigFunction()
{
    // body
}

///////////////////////////////
// f1.cpp

#include "bigfunction.h"

void f1()
{
    ...
    BigFunction<int>();
}

///////////////////////////////
// f2.cpp

#include "bigfunction.h"

extern template void BigFunction<int>();

void f2()
{
    ...
    BigFunction<int>();
}
```

Resulting in:

<!--
```
f1.o
    void f1()
    void BigFunction<int>()  // compiled just one time
f2.o
    void f2()
	// BigFunction<int>() missing because of the extern clause
```
-->

```shell
> nm -g -C --defined-only *.o

f1.o:
00000000 W void BigFunction<int>()
00000000 T f1()

f2.o:
00000000 T f2()
```

And the same applies for template classes, with the following syntax:

```c++
///////////////////////////////
// bigclass.h

template<typename T>
class BigClass
{
    // implementation
};

///////////////////////////////
// f1.cpp

#include "bigclass.h"

void f1()
{
    ...
    BigClass<int> bc;
}

///////////////////////////////
// f2.cpp

#include "bigclass.h"

extern template class BigClass<int>;

void f2()
{
    ...
    BigClass<int> bc;
}
```

## Something missing

Unfortunately, it's not that simple.

For example, when you try to compile the code above
with optimization enabled (let's say -O2 on gcc or clang),
the linker might complain that `BigFunction<int>()` is undefined.
Why?

The problem is that when compiling `f1.cpp` with the optimization enabled,
the template function is expanded inline at the point of the function call
instead of being really generated,
so when the linker comes across `f2` object file, it can't find it anymore.

<!--
If you're using gcc, you can verify that the problem is caused by
the inline expansion of the function:
-->

You can use `nm` to check again the symbols exported by the object files,
and verify that the issue here is the inline expansion of the function:

```shell
> nm -g -C --defined-only *.o

f1.o:
00000000 T f1()

f2.o:
00000000 T f2()
```

in `f1.o` the symbol is missing because of the optimization,
while in `f2.o` the symbol is missing because of the `extern` clause.

If you’re using gcc, you can get further evidence of this by trying:

```c++
// bigfunction.h

template<typename T>
void __attribute__ ((noinline)) BigFunction()
{
    // body
}
```

Here, the gcc-specific attribute `noinline` prevents the compiler to
expand the function inline,
so that the linker can find it and does not complain anymore.

## A global strategy

The gcc-specific attribute `noinline` is obviously not the
final solution to our problem.

A point worth noting here is that the strategy to reduce compilation time
is relative to an entire project, and so is the usage
of the extern template clause.

One strategy at the project scope to take advantage of the extern template
mechanism while ensuring that all the code needed by the linker is generated
might be the following:
*include in every translation unit where the template appears
a header file containing the clause `extern template`,
and add to the project a source file with the explicit instantiation.*

```c++
///////////////////////////////
// bigfunction.h

template<typename T>
void BigFunction()
{
    // body
}

extern template void BigFunction<int>();


///////////////////////////////
// bigfunction.cpp

#include "bigfunction.h"

template void BigFunction<int>();

///////////////////////////////
// f1.cpp

#include "bigfunction.h"

void f1()
{
    ...
    BigFunction<int>();
}

///////////////////////////////
// f2.cpp

#include "bigfunction.h"

void f2()
{
    ...
    BigFunction<int>();
}
```

Please note that the solution still applies
when the template function/class is part of a third-party library:
in that case, it's enough to add your own header file
including the library that adds the extern template clause.

```c++
///////////////////////////////
// third_party_bigfunction.h

template<typename T>
void BigFunction()
{
    // body
}

///////////////////////////////
// bigfunction.h

#include <third_party_bigfunction.h>

extern template void BigFunction<int>();

///////////////////////////////
// bigfunction.cpp

#include "bigfunction.h"

template void BigFunction<int>();

///////////////////////////////
// f1.cpp

#include "bigfunction.h"

void f1()
{
    ...
    BigFunction<int>();
}

///////////////////////////////
// f2.cpp

#include "bigfunction.h"

void f2()
{
    ...
    BigFunction<int>();
}
```

## Summary

Reducing compile times by using extern template is a project scope strategy.
One should consider which are the templates most expensive
that are used in many translation units
and find a way to tell the build system to compile it only one time.

But let’s consider for a moment what we’ve done in the previous paragraph.

We had a template function/class.
To minimize the build time we decided to instantiate it only one time
for a given template parameter.
In doing so, we had to force the compiler to generate
*exactly* one time the function/class for the given template parameter,
preventing the inline expansion (and possibly giving up a run-time optimization).
However, if the compiler decided to inline a function,
chances are that it was not so big,
meaning, after all, we don't save so much build time
by compiling it only once.

Anyway, if you're determined to save both the goats and the cabbages,
you can try to enable the link time optimization flags (`-flto` on gcc):
it will perform global optimizations (e.g., inlining) having visibility of the whole project.
Of course this, in turn, will slow down the build process, but you'll get
your function template inlined but instantiated only once.

Bottom line: programming is always a trade-off between conflicting facets,
and you should measure carefully whether a template function
is slowing down your build (because e.g., it's instantiated with the same parameter in many compilation units)
or your run-time execution (because e.g., it's called in just one location but in a tight loop)
and -- above all -- consider your priorities.

After all, the observation *“premature optimization is the root of all evil”*
and the rule that immediately follows *“measure before optimize”*
can also be applied to compile time.
You can easily measure what happens to build times and run times
after declaring `extern template` an item and then choose accordingly.

At the end of the day,
it is inevitable that we decide whether to optimize for compilation or execution.
After all, that's exactly what I wrote at the very beginning of this article:
one of the methods to speed up build time is to turn off optimizations :-)
