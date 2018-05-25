---
layout: post
title: "TODO - Swap Model: a design pattern"
published: true
category: general
tags: [design, C++]
---

C’è una soluzione che mi è capitato di usare diverse volte nel mio software...

... pattern [...] credo sia una forma utile per discutere di design, come lo intendo io. Non microdesign, che ad esempio è la cosa su cui si concentrano maggiormente i guru del C++.

E visto che qualcuno tra i più giovani non sa nemmeno cosa sono i design pattern, approfitto di questo post per descrivere la mia soluzione sotto forma di design pattern.

Design pattern (in forma di pattern).

Applicazione che deve recuperare dati e diagnostica da dei sensori (processo lento) e deve renderli disponibili tramite web service.
Recuperare i dati è un’operazione lenta (protocolli di vario tipo, anche proprietari),
che deve essere eseguita continuamente.
Come garantire la consistenza e coerenza per il web server?

Si usano due strutture dati (ad albero) identiche. Una viene riempita coi dati raccolti, l’altra letta dal web service. Si sfruttano gli Smart pointer (shared) per fare in modo che i client possano continuare a navigare lo stato dei sensori in maniera coerente, e visto che lettori e scrittori lavorano sempre su strutture dati diverse, l’unico lock necessario è quello della variabile che contiene gli Smart pointer corrente e futuro.

(Elaborare)

# Pattern name and Classification:
Swap model (behavioral)

# Intent
The pattern allows a client (or multiple clients) to read a complex data model
that is continuously updated by a unique producer in a thread safe fashion.

Allow ...

Represent ...
Swap lets you ...

Provide ...

Separate ...

A short statement that answers the following questions: What does the
design pattern do? What is its rationale and intent? What particular design
issue or problem does it address?


# Also known as

TODO Questo non lo mettiamo

# Motivation (Forces)

A scenario that illustrates a design problem and how the class and object
structures in the pattern solve the problem. The scenario will help you
understand the more abstract description of the pattern that follows.

TODO general explanation

Sometimes it's necessary ...

Consider for example an application that periodically retrieves data from a large sensor network,
to performs some kind of statistical elaboration on the collected data set and send alarms when some criterial are met.
The retrieval operation is a task completely separated (@@@ uncorrelated, ) from the statistical analysis and alarms evaluation,
and can possibly run in separated threads.
Besides, the data retrieval and its usage have different timing (e.g., the sensor network is scanned each 5 minutes,
while the statistical elaboration is performed on request by a human operator).
The data collected from the sensor network is structured in a complex lattice of objects
similar (@@@ resemble, che mima) to the one of the real sensor network, in such a way that the elaboration modules
can navigate the lattice in a simple way (@@@ domain?).
In this scenario, how can all the modules of the application work together on the same data structure?

The idea of this pattern is @@@ to have the pass the sensor data structure (@@@) from the producer to the consumers?



![motivation]({{ site.baseurl }}/images/swap-pattern-motivation.png)

# Applicability

What are the situations in which the design pattern can be applied?
What are examples of poor designs that the pattern can address? How can
you recognize these situations?

Use Swap Pattern when
- choice 1
- choice 2
- choice 3

# Structure

A graphical representation of the classes in the pattern using a notation
based on the Object Modeling Technique (OMT) [RBP+91]. We also use
interaction diagrams [JCJO92, Boo94] to illustrate sequences of requests
and collaborations between objects. Appendix B describes these notations
in detail.

![structure]({{ site.baseurl }}/images/swap-pattern-structure.png)

# Participants

The classes and/or objects participating in the design pattern and their
responsibilities.

- Snapshot (SensorAcquisition)
  - hold the entire set of data acquired by source
  - possibly provides const function members to query the acquisition
  - possibly a set of (etherogeneous) objects linked (e.g., a list of Measure objects)
  - perform a complete scan
- Source (SensorNetwork)
  - periodically asks source to perform a new scan
  - provide to its clients the latest complete scan
- Client (WebService, ThresholdMonitor, Statistics)
  - asks the Source for the latest Snapshot available and use it (in read-only mode)

# Collaboration

How the participants collaborate to carry out their responsibilities.

# Consequences

How does the pattern support its objectives? What are the trade-offs
and results of using the pattern? What aspect of system structure does it
let you vary independently?

(Non so se va qui. Questi sono gli effetti del pattern)
- disaccoppiamento del produttore e dei lettori (come li chiamiamo? In realtà non è proprio un produttore) dal punto di vista temporale. Il produttore può procedere all’aggiornamento col suo ritmo, i lettori ogni volta ottengono la versione più recente.
- gestione della concorrenza. Il produttore e ogni lettore possono essere in thread diversi
- viene garantita la coerenza di tutta la struttura dati che viene letta in un dato istante da un consumatore senza bisogno di lock presi per tanto tempo.
- occupazione di memoria ridotta al minimo indispensabile per garantire che ogni consumatore abbia accesso coerente allo snapshot più recente disponibile.

# Implementation

What pitfalls, hints, or techniques should you be aware of when
implementing the pattern? Are there language-specific issues?

<TODO>

DataSource può essere sincrono (come descritto prima) oppure asincrono.
In quest'ultimo caso DataSource potrebbe avere un evento "ScanCompleted"
su cui si registra Snapshot, a seguito del quale modifica il proprio campo current.

Il pattern è descritto in C++, ma può essere vantaggiosamente utilizzato un linguaggio
con garbage collection. Anzi, in C++ occorre usare gli shared_ptr in modo che
venga distrutto uno snapshot quando nessun client lo utilizza più e è già disponibile quello nuovo.

Perché il meccanismo degli shared_ptr (o di garbage collection) funzioni
(cioè che sgli snapshot obsoleti vengano distrutti),
occorre che i client invochino XXX::Get() ogni volta che hanno bisogno di Snapshot.

Modello di concorrenza: TODO.

Gli oggetti che compongono Snapshot (potenzialmente una grande e complessa struttura dati)
vengono distrutti e ricreati ad ogni ciclo di scansione. E' possibile utilizzare un pool
di oggetti (potenzialmene solo una coppia di strutture dati Snapshot).

Si noti che Snapshot (e la struttura dati che rappresenta)) è immutabile,
cosa che va molto di moda ultimamente. Con tutti i vantaggi che porta dal punto di vista della
concorrenza (e.g., multipli client che girano in thread diversi).

Be aware of stupid classes! Snapshot (and the classes it represents) should not be data only classes, filled by Source. Every class should instead contribute to retrieve its own data.

(Problema analogo ma difficile soluzione è quello dell’utilizzo dei dati: dobbiamo mettere anche questa responsabilità dentro snapshot?)

# Sample Code

Code fragments that illustrate how you might implement the pattern in
C++ or Smalltalk.

# Known Uses

Examples of the pattern found in real systems. We include at least two
examples from different domains.

# Related Patterns

What design patterns are closely related to this one? What are the
important differences? With which other patterns should this one be used?

- Snapshot can be a Facade @@@
