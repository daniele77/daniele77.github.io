---
layout: post
title: "Compile the compiler"
published: true
category: general
tags: [design]
---

* Problema spesso nell'embedded (di avere una piattaforma vecchia)
* Utilizzo di nuove funzionalità, meno bug etc.
* Compila il compilatore: step fondamentali per il gcc
* E' sufficiente esportare le variabili d'ambiente CC e CXX
* A run-time occorre impostare LD_LIBRARY_PATH per le nuove librerie
