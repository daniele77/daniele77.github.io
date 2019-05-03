---
layout: post
title: "Explicit template instantiation, reusability and modularity"
published: false
category: general
tags: [C++]
---

In my [previous post]({{ site.baseurl }}{% post_url 2019-02-12-cpp-extern-template %}),
I explained how to use the C++11 feature *extern template* to reduce compilation times
when the same template instantiation pccurs in multiple source files.

A few days ago, a reader of the blog asked me some recommendations on how to choose between
external templates and explicit template instantiation in a source file.
Indeed, the latter is an alternative approach
to the problem of reducing compile times due
to multiple instantiations of the same template
scattered among many source files.

In this post, I'm going to show this technique and point out
the difference with the solution explained in the previous post.
Finally, I will  outline some design implication of the explicit template instantiation solution.

## How does it work

The technique consists of:

* a header file with the declaration of the template function/class,
* a source file with the function body **and** the explicit instantiations.

Using the same example of the previous post, the code becomes:

```c++
///////////////////////////////
// bigfunction.h

template<typename T>
void BigFunction();


///////////////////////////////
// bigfunction.cpp

#include "bigfunction.h"

template<typename T>
void BigFunction()
{
    // body
}

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

In this way, the compiler generates the code exactly once.

## Drawbacks

On the other hand, using the above technique:

* we prevent the compiler from expanding the code inline,
* the template becomes *inextensible* and less *reusable*
  (in the case it’s part of a library or if it could become part of a library in the future).

While the first point is straightforward, I think the second might need some extra explanation.

The original problem we're trying to solve is caused by the fact that we're using
the template in multiple translation units of the same project.
This likely means that our template is useful and reusable,
maybe in many projects: that's why it should be considered part of a library.
Even if the template is not shared with other projects
but we're reusing it multiple times in a large application,
we should maintain it as if it were a library.
This has to do with *reusability*.

Using the technique presented in this post inside a library
forces us to decide in advance
which template arguments we want to provide to the users
(because all explicit instantiations go in the very same
source file where the template body is implemented).
Unfortunately, this breaks down entirely the extensibility of the library,
because you cannot use the template with other types.
And this is a shame because *extensibility* is the main raison d'être of template.

And keep in mind that a template is *reusable* primarily because it's *extensible*.
If for some reason a template library is not extensible,
it becomes also less *reusable*
because you cannot use it with types unplanned by the library author.

So, this solution reduces the *reusability* of the template,
in addition to make it *inextensible*.
However, the *reusability* property is not completely lost,
because sometimes templates are thought only for a predefined set of arguments
(not often, to tell the truth).

## Choices, as usual

A rule of thumb to decide when to use external templates or
explicit instantiation in this way might be:

* use *external templates* when the template is part of a library (or should be considered a library),
* use *external templates* when you want the template to be expanded inline,
* otherwise, define the template in a .cpp file and add all the explicit instantiations
  you’re going to use in your application.

The last point also reveals that the “explicit instantiation”
approach reduces greatly the modularity of a project.
Since you need to put all the template instantiations
in one artifact (the source file),
adding new features to the application
will likely result in adding/updating more than just one artifact.

On the contrary, in my projects, I always try to have an “incremental design”
i.e., a design where a new requirement can be implemented
by just adding/updating a single artifact (e.g., a single .cpp file).
An "incremental design" brings additional positive properties
that I will elaborate in a future post,
but can't be achieved when
the application has got a single center (the cpp file)
entangled with an unstable, unbounded set (the template arguments).
