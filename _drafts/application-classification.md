---
layout: post
title: "A [slightly] more formal application classification"
published: true
category: general
tags: [develop]
---

... dopo un po' mi sono rotto le scatole e ho deciso di fare
un po' di chiarezza e classificare i software secondo @@@:

@@@

Nella mia testa, è sempre stata evidente una suddivisione tra le applicazioni che sono
intrinsecamente ad eventi e quelle intrinsecamente sequenziali.

La definizione potrebbe coincidere con cpu-bound e io-bound applications,
in reatà è fuorviante, perché ad esempio:
- un'applicazione che faccia pesanti richieste ad un database per elaborarne statisticamente i dati
  potrebbe essere io-bound (in quanto il fattore limitante è l'I/O verso il DB),
  ma intrinsecamente sequenziale (il flusso di controllo non viene interrotto da eventi)

keywords:

* I/O bound task
* CPU bound task
* CPU intensive calculation
* ...

* When something is X-bound, it means that it is the slowest/most important factor with regards to possible performance improvements.

Most problems that do not perform large IO are CPU bound, as most of their execution time is spent in solving the problem rather than reading/writing. Some examples include:

* Search algorithms
* Video/Audio/Content conversion/compression algorithms
* Video streaming
* Graphics processing/rendering e.g games, videos
* Heavy mathematical computations like matrix multiplication, calculating factorials, finding prime numbers etc.

Quello che interessa a me è invece proprio la natura intrinseca delle diverse applicazioni:
- intrinsecamente asincrone (reagiscono ad eventi esterni asincroni)
- intrinsecamente sincrone (@@@ TODO)

Il primo tipo di applicazione è quella che in qualche modo interagisce in tempo reale col mondo esterno,
e da esso riceve input in momenti casuali.
Esempi di queste applicazioni:
- UI: l'utente normalmente può effettuare operazioni in ogni momento
- Server: le richieste possono arrivare in ogni momento
- Sistemi reattivi: ogni applicazione collegata a sensori che @@@

Il secondo tipo di applicazione è quella che non ha interruzioni (se non in momenti prestabiliti)
durante la sua esecuzione.
Esempi di queste applicazioni:
- Applicazioni numeriche: i dati in ingresso sono forniti all'inizio (o in momenti prestabiliti, magari leggendo un file)
  ma in seguito il flusso di esecuzione viene deciso esclusivamente dall'applicazione e dagli ingressi (ma non dal momento in cui sopraggiungono nuovi input)
- @@@

Si noti che la classificazione precedente riguarda esclusivamente il tipo di problema,
e non l'architettura usata dall'applicazione.
Un problema intrinsecamente sincrono può essere risolto con un'applicazione implementata ad eventi,
o multithread,
così come un problema intrinsecamente asincrono può essere risolto con una coda e un unico thread.

Ovviamente, però, è vantaggioso allineare l'architettura del problema a quella dell'applicazione.
