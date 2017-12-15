---
layout: post
title: "IO bound and CPU bound applications"
published: true
category: general
tags: [design]
---

Nella mia testa, è sempre stata evidente una suddivisione tra le applicazioni che sono
intrinsecamente ad eventi e quelle intrinsecamente sequenziali.

La definizione potrebbe coincidere con cpu-bound e io-bound applications,
in reatà è fuorviante, perché ###

keywords:
* I/O bound task
* CPU bound task
* CPU intensive calculation
* 

* When something is X-bound, it means that it is the slowest/most important factor with regards to possible performance improvements. 

Most problems that do not perform large IO are CPU bound, as most of their execution time is spent in solving the problem rather than reading/writing. Some examples include:
1. Search algorithms
2. Video/Audio/Content conversion/compression algorithms
3. Video streaming
4. Graphics processing/rendering e.g games, videos
5. Heavy mathematical computations like matrix multiplication, calculating factorials, finding prime numbers etc.
