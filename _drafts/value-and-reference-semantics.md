---
layout: post
title: Value and Reference Semantics
published: true
category: general
tags: [design, C++]
---

* Value and reference semantics: definition using object identity.
* Sul web si trova spesso una semplice valutazione basata su:
  * efficienza
  * semplicità
  * polimorfismo
  in realtà l'argomento è molto più profondo, e la scelta dev'essere legata al tipo di oggetto che stiamo progettando
* Oggetti che rappresentano una risorsa del mondo reale (un mutex, una socket, una chiamata telefonica, ...)
  e che devono stare allineati (definire), devono avere reference semantics, indipendentemente dal polimorfismo.
* std::shared_ptr per trasformare una value semantic in reference semantic.
* Adesso che in C++ abbiamo la move semantics, possiamo fare move-only classes: esiste al più un'istanza di oggetto
  con una certa identità
* Purtroppo è abbastanza complicato da usare (es. di un vettore di oggetti. In un vettore i riferimenti vengono invalidati. Usare list o deque)