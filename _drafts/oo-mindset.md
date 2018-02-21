---
layout: post
title: "TODO (OO mindset)"
published: true
category: general
tags: [design]
---

Ho notato in alcuni colleghi, provenienti dal C, e comunque non troppo esperti di OO,
la tendenza ad avere un approccio diverso anche in contesti non strettamente legati
a @@@.

Ad esempio, anche in un'applicazione SPA javascript gli approcci sono molto diversi.
Supponiamo di avere due prodotti, simili ma con alcune differenze.
Nel caso di un front-end web, le differenze saranno sicuramente nella parte grafica
(layout, css, lingua), nella validazione, etc.
Tradotto: la parte html sarà probabilmente diversa, i css lo saranno sicuramente,
una parte di codice javascript sarà diversa e una parte uguale (o simile, con alcune varianti).

I due approcci sono:
- avere un unico repository, in cui i file sorgenti sono condivisi da entrambe le applicazioni,
  e contengono l'insieme di tutte le funzioni / classi / oggetti [nel caso css?] di entrambe le applicazioni. Le funzioni simili vengono parametrizzate secondo un parametro che dice quale applicazione, il file delle traduzioni contiene tutte le lingue, etc.
- avere un repository per ogni applicazione, più uno delle parti comuni / librerie.
  In questo caso ogni applicazione conterrà esclusivamente gli elementi di propria competenza,
  gli elementi (funzioni/classi/oggetti) simili vengono parametrizzate tramite politiche (o simili), ogni file di traduzione contiene una sola lingua e così via.

Nel primo caso si producono degli artifatti che sono il mcm delle applicazioni che vogliamo
realizzare, nel secondo si fattorizzano parti in comune ad almeno due applicazioni e si
mettono in una libreria condivisa, sforzandosi di rendere comuni le parti simili anche
se non proprio identiche.

Vantaggi del secondo approccio rispetto al primo:
- quando aggiungiamo una nuova applicazione non tocchiamo quelle esistenti (v. test)
- quando aggiungiamo nuove funzionalità [TODO]

Il secondo approccio porta ad una struttura ad oggetti (come definita da C. Pescio),
cioè che l'aggiunta di funzionalità predilige l'aggiunta di manufatti.

V. C++ variant vs inheritance:

http://cpptruths.blogspot.it/2018/02/inheritance-vs-stdvariant-based.html

