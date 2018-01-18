---
layout: post
title: "TODO - Useful C++ tools"
published: true
category: general
tags: [C++]
---

TODO

# gprof

It's a profile for gcc/g++ compilers:

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
