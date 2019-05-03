---
layout: post
title: "Explicit instantiation to reduce compile time"
published: false
category: general
tags: [C++]
---

In my [previous post]({{ site.baseurl }}{% post_url 2019-02-12-cpp-extern-template %}),
I explained how to use the C++11 feature *extern template* to reduce compilation times
when the same template instantiation occours in multiple source files.

<!--
A reader of the blog asked me some recommendations on how to choose between
external templates and explicit template instantiation in a source file,
to solve the same problem.

Actually, explicit instantiation in a source file is an alternative approach
to the problem of reducing compile times due
to multiple instantiation of the same template functions/class used in many
object files.
In this post I'm going to show this technique and point out
the difference with the solution explained in the previous post.

Finally, I will  outline some design implication of the explicit template instantiation solution.
-->

A few days ago, a reader of the blog asked me some recommendations on how to choose between
external templates and explicit template instantiation in a source file.
Indeed, the latter is an alternative approach
to the problem of reducing compile times due
to multiple instantiation of the same template functions/class used in many
source files.

In this post I'm going to show this technique and point out
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

Using the technique presented in this post inside a library,
forces us to decide in advance
which template arguments we want to provide to the users
(because all explicit instantiations go in the very same
source file where the template body is implemented).
Unfortunately, this breaks down entirely the extensibility of the library,
because you cannot use the template with other types.
And this is a shame because *extensibility* is the main raisons d'être of template.

And keep in mind that a template is *reusable* primarily because it's *extensible*.
If for some reason a template library is not extensible,
it becomes also less *reusable*
because you cannot use it with types unplanned by the library author.

So, this solution reduce the *reusability* of the template
in addition to make it *inextensible*.
However, the *reusability* property is not completely lost,
because sometimes templates are thought only for a predefined set of arguments
(not often, to tell the thruth).

<!--
-----

By putting the body of the function template in an implementation file
(and this is exactly the point of this solution)
we are forced to put all the explicit instantiation
in the very same file.
We're forced to predict all the instantiations of the template
that our application will need, and put them in the
same file together with the function body.

So, what's the problem? During the development of our application,
we can add the explicit instantiations to the implementation file
each time we need them in other places of the software.

This however can't be done if our template function / class
is part of a library. Or if it could become part of a library in the future.

TODO rivedere:
- ci sono casi in cui il template ha senso solo su un set limitato e predefinito
  di tipi (e in questo caso conosciuti alla libreria) => nessun problema
- negli altri casi, però, non posso prendere la funzione e riutilizzarla
  sotto forma di libreria perché -- appunto -- mancherebbe della necessaria
  proprietà di estendibilità => viene meno anche la riusabilità (della funzione).
- la soluzione riduce comunque la modularità dell'applicazione (vedi sotto)


the function/class template cannot be considered anymore part 
  of a library (in the case it ever was), because you cannot use the
  template with other types. So it becomes less *reusable*.
  <!--
*we’re giving up the partitioning between “library” and “application”
  (in the case this has ever been a concern for your specific project).
-->


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

On the contrary, in my projects I always try to have an “incremental design”
i.e., a design where a new requirement can be implemented
by just adding/updating a single artifact (e.g., a single .cpp file).
An "incremental design" brings additional positive properties
that I will elaborate in a future post,
but can't be achieved when
the application has got a single center (the cpp file)
entangled with an unstable, unbounded set (the template arguments).
