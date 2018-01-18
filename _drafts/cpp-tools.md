---
layout: post
title: "TODO - Useful C++ tools"
published: true
category: general
tags: [C++]
---

TODO

# valgrind
Memory check
(http://pages.cs.wisc.edu/~bart/537/valgrind.html)

{% highlight %}
# run
valgrind application
{% endhighlight %}

Dice gli errori di lettura/scrittura

# gprof

(non ne parlano tanto bene)

It's a profiler for gcc/g++ compilers:

Usage:

{% highlight %}
# compile
g++ -Wall -g -pg main.cpp -o application
# run
./application
# extract data
gprof application > result.txt
# show profile result
cat result.txt
{% endhighlight %}

# Callgrind (kcachegrind)

Profiler.

(see https://baptiste-wicht.com/posts/2011/09/profile-c-application-with-callgrind-kcachegrind.html)
Usage:

{% highlight %}
# run
valgrind --tool=callgrind program [program_options]
# show profile result
kcachegrind callgrind.out.XXX
{% endhighlight %}

