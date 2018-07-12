---
layout: post
title: "TODO - Swap Model: a design pattern"
published: true
category: general
tags: [design, C++]
---

Several times happened to me to encounter the same sort of problem when developing 
a certain kind of applications
(I guess there is not yet a term globally acknowledged for such a kind of software),
and to solve it using everytime the same object structure, interacting in the same way
(with some slight difference).

<!-- There is a solution that I used several times in my applications, under specific circumstances. -->

Since the solution worked very well for me, and it's proving robustness in many production systems,
I thought it could be useful to others (or anyway, it can be useful at least formalize a problem/solution
that maybe others are already using).

Furthermore, I could not find any reference to a similar solution in literature,
and this convinced me to write this post.
Of course, if you know it is already documented and formalized somewhere, please let me know.

Being a structure that solves specific software forces, I decided to document it under the well-known form of a *design pattern*.
First of all, because it is exactly that:
a *pattern* (meaning something we encounter often)
and about *design* (i.e., the structure of the software).

On the other hand, I believe that the "design pattern way" could be a form
useful to discuss *Software Design*.
Be aware: not *micro design* at the code level, that sadly is the aspect
on which nowadays C++ gurus focus most.

Since most of the young developers haven't read the book "Design Patterns"
(because C++ gurus always speak about micro-optimization -- but this is another story and blog post)
I hope reading this post is enough to make them curious about the subject.

So, let's begin with the real pattern, described in the commonly used documentation format
made famous by the GoF book
(See 
http://en.wikipedia.org/wiki/Software_design_pattern#Documentation
or -- even better -- read the real book! :-). 

<!--

C’è una soluzione che mi è capitato di usare diverse volte nel mio software ###

... pattern [...] credo sia una forma utile per discutere di design, come lo intendo io. Non microdesign, che ad esempio è la cosa su cui si concentrano maggiormente i guru del C++.

E visto che qualcuno tra i più giovani non sa nemmeno cosa sono i design pattern, approfitto di questo post per descrivere la mia soluzione sotto forma di design pattern.

Design pattern (in forma di pattern).

Applicazione che deve recuperare dati e diagnostica da dei sensori (processo lento) e deve renderli disponibili tramite web service.
Recuperare i dati è un’operazione lenta (protocolli di vario tipo, anche proprietari),
che deve essere eseguita continuamente.
Come garantire la consistenza e coerenza per il web server?

Si usano due strutture dati (ad albero) identiche. Una viene riempita coi dati raccolti, l’altra letta dal web service. Si sfruttano gli Smart pointer (shared) per fare in modo che i client possano continuare a navigare lo stato dei sensori in maniera coerente, e visto che lettori e scrittori lavorano sempre su strutture dati diverse, l’unico lock necessario è quello della variabile che contiene gli Smart pointer corrente e futuro.

(Elaborare)

Due i problemi da risolvere:
- Come garantire ai client una vista coerente di tutto il sistema da diagnosticare?
- Come può il server aggiornare i dati mentre vengono usati dai client?

-->

----

## Pattern name and Classification

Swap model (behavioral)

## Intent

The pattern allows multiple clients to read a complex data model
that is continuously updated by a unique producer, in a thread-safe fashion.

<!--
Allow ...

Represent ...
Swap lets you ...

Provide ...

Separate ...

A short statement that answers the following questions: What does the
design pattern do? What are its rationale and intent? What particular design
issue or problem does it address?
-->

## Also known as

Model publisher, Pressman. Newsagent

## Motivation (Forces)

<!--
A scenario that illustrates a design problem and how the class and object
structures in the pattern solve the problem. The scenario will help you
understand the more abstract description of the pattern that follows.

TODO general explanation
-->

Sometimes it's necessary to decouple the source of a complex data
from its consumers, in such a way that every actor can run at its own pace
without interfering with each other.

Consider for example an application that periodically retrieves data from a large sensor network,
to perform some kind of statistical elaboration on the collected data set and send alarms when some criteria are met.
The retrieval operation is a complex long task, involving several network protocols,
and is completely independent (TODO separated, uncorrelated, ) from the statistical analysis and alarms evaluation,
and can possibly run in separated threads.
Besides, the data retrieval and its usage have different timing
(e.g., the sensor network is scanned every 5 minutes,
while the statistical elaboration is performed on request by a human operator
on the most recent collected dataset).
The data collected from the sensor network is structured in a complex lattice of objects
resembling the ones you find in the physical world,
in such a way that the modules doing the data elaboration can navigate
the data in a more natural way.
<!--
similar (TODO resemble, che mima) to the one of the real sensor network, in such a way that the elaboration modules
can navigate the lattice in a simple way (TODO domain?).
-->

In this scenario, how can all the modules of the application work together on the same data structure?
How can all the clients use the most updated data available
in a consistent fashion?
And how can the application get rid of the old data
only when no one is still using it?

<!--
The main idea behind this pattern is to use two shared_ptr (in C++) or two variables (in languages with garbage collection):
one (named `filling`) holding the object structure currently retrieving the sensor data,
the other (named `current`) holding the most recent complete acquisition.
-->

The main idea of this pattern is to pass the sensor data structure
from the producer to the consumers by means of two shared pointers (in C++)
or two variables (in languages with garbage collection):
one (named `filling`) holding the object structure currently retrieving the sensor data,
the other (named `current`) holding the most recent complete acquisition.

A class `SensorNetwork` decides when it's time to start a new acquisition and
substitutes `current` with `filling` when the acquisition is completed.
When a client needs to perform some task on the data acquired,
it asks `SensorNetwork` that returns `current` (i.e., the most recent data acquired).
An object of class `SensorAcquisition` is granted to remain in life
and not change for all the time a client holds the smart pointer
(and the same is still valid in garbage collected languages).

<!--
While the client holds the smart pointer, the data structure is granted to remain in life
and not change (and the same is still valid in garbage collected languages).
-->

The data acquisition (performed by `SensorAcquisition`)
and its reading (performed by the various clients: `Statistics`, `ThresholdMonitor` and `WebService`)
are possibly performed in multiple threads.
The safety of the code is granted by:

1. a `SensorAcquisition` can be modified only by the thread of `SensorNetwork`,
   and never changed after it becomes public (i.e., the smart-pointers `current` is
   substitute by `filling`)
2. the smart pointer exchange is protected by a mutex.

The following diagram shows a typical PATTERNNAME (TODO) class structure:

![motivation](/images/swap-pattern/motivation.png)

## Applicability

<!--
What are the situations in which the design pattern can be applied?
What are examples of poor designs that the pattern can address? How can
you recognize these situations?
-->

Use PATTERNNAME (TODO) when

* you have a complex data structure slow to update.
* Its clients must asynchronously read the most updated data available
  in a consistent fashion.
* The old data must be discarded when is no longer needed.

## Structure

<!--
A graphical representation of the classes in the pattern using a notation
based on the Object Modeling Technique (OMT) [RBP+91]. We also use
interaction diagrams [JCJO92, Boo94] to illustrate sequences of requests
and collaborations between objects. Appendix B describes these notations
in detail.
-->

![structure](/images/swap-pattern/structure.png)

## Participants

<!--
The classes and/or objects participating in the design pattern and their
responsibilities.
-->

* `Snapshot` (`SensorAcquisition`)
  * hold the entire set of data acquired by the source
  * possibly provides const function members to query the acquisition
  * possibly a set of (heterogeneous) objects linked (e.g., a list of Measure objects)
  * perform a complete scan
* `Source` (`SensorNetwork`)
  * periodically asks the source to perform a new scan
  * provide to its clients the latest complete scan
* `Client` (`WebService`, `ThresholdMonitor`, `Statistics`)
  * asks the `Source` for the latest `Snapshot` available and uses it (in read-only mode)

## Collaboration

<!--
How the participants collaborate to carry out their responsibilities.
-->

* `Snapshot` creates a new `Source` instance, assigns it to the `shared_ptr` `filling`,
  and commands it to start the acquisition.
* When the acquisition is terminated, `Snapshot` performs the assignment
  `current=filling` protected by a mutex.
  If no clients were holding the previous `current`, the pointed `Source` is destroyed.
* When a client needs the most updated `Source`, calls `Snapshot::GetLastSnapshot()`
  that returns `current`.

![structure](/images/swap-pattern/collaboration.png)

## Consequences

<!--
How does the pattern support its objectives? What are the trade-offs
and results of using the pattern? What aspect of system structure does it
let you vary independently?
-->

<!--
(Non so se va qui. Questi sono gli effetti del pattern)

* disaccoppiamento del produttore e dei lettori (come li chiamiamo? In realtà non è proprio un produttore) dal punto di vista temporale. Il produttore può procedere all’aggiornamento col suo ritmo, i lettori ogni volta ottengono la versione più recente.
* gestione della concorrenza. Il produttore e ogni lettore possono essere in thread diversi
* viene garantita la coerenza di tutta la struttura dati che viene letta in un dato istante da un consumatore senza bisogno di lock presi per tanto tempo.
* occupazione di memoria ridotta al minimo indispensabile per garantire che ogni consumatore abbia accesso coerente allo snapshot più recente disponibile.
-->

TODO: CONTROLLARE

* PATTERNNAME (TODO) decouples the producer from the readers:
  the producer can go on with the update of the data (slow)
  and each reader gets each time the most updated version.
* Synchronization. Producer and readers can run in different threads.
* PATTERNNAME (TODO) grants the coherence of all the data structures that are
  read in a given instant from a reader, without locks taken for long times.
* Memory consumption to the bare minimum to grant that every reader
  has a coherent access to the most recent snapshot.

## Implementation

<!--
What pitfalls, hints, or techniques should you be aware of when
implementing the pattern? Are there language-specific issues?
-->

Here are 8 issues to consider when implementing the PATTERNNAME (TODO) pattern:

1. A new acquisition can be started periodically (as proposed in the example) or
   immediately after the previous one is completed.

2. The pattern is described using C++, but it can be implemented as well in every 
   language with garbage collection.
   In C++ `std::shared_ptr` is needed to be sure a `Snapshot` is deleted when no client is using it
   and `Source` has a more updated snapshot ready.
   In a language with garbage collection, the collector will take care of deleting old snapshots
   when they're no more used (unfortunately at some unspecified time so that we can have many unused snapshots in memory).

3. The `std::shared_ptr` (or garbage collection) mechanism will work
   (i.e., old snapshots are deleted) only if clients use `Source::GetLastSnapshot()`
   every time they need a snapshot.

4. `Snapshot` (and the application in general) can be synchronous or asynchronous.

   In the first case, the method `Snapshot::Scan` is a blocking function and the caller
   (i.e., `Source`) waits until the data structure is completed before acquiring the mutex
   and assign `current` to `filling`. Within a synchronous application,
   clients will run in other threads.

   In the second case, the method `Snapshot::Scan` starts the acquisition operation and
   exit immediately. When the data structure is completed, an event notification mechanism
   (e.g., events, callbacks, signals) takes care to announce the end of the operation to
   `Source`, that can finally acquire the mutex before assigning `current` to `filling`.
   An asynchronous application can be single-thread or multi-thread.

5. The pattern supports every concurrency model: from the single thread
   (in a completely asynchronous application) to the maximum parallelization possible
   (when the acquisition has its own threads, as well as each client).

6. The objects composing `Snapshot` (usually a huge complex data structure)
   are (possibly) deleted and recreated at every scan cycle. 
   It's possible to use a pool of objects instead (in this case `shared_ptr`
   must be substituted by a reference counted pool object handler).

7. Please note that `Snapshot` (and the classes it represents) is immutable.
   After its creation (and after the scan is completed), the clients can only read it.
   When a new snapshot is available, the old one is deleted and the clients will read the new one.
   This is a big advantage from the concurrency point of view: multiple clients in different threads
   can read the same snapshot without locks.

8. Be aware of stupid classes! `Snapshot` (and the classes it represents) should not be a passive
   container of data. Every class should at least contribute to retrieve its own data,
   and one could also consider whether to add methods and facilities to use the data
   (the latter is actually an extremely sensitive and complex issue,
   maybe it deserves its own blog post :-).

## Sample Code

<!--
Code fragments that illustrate how you might implement the pattern in
C++ or Smalltalk.
-->

The C++ code shown here sketches the implementation of the PATTERNNAME (TODO)
classes in the Motivation section.

```c++
class SensorAcquisition
{
public:
    // interface for clients
    const SomeComplexDataStructure& Data() const { /* ... */ }
    // interface for SensorNetwork
    template <typename Handler>
    void Scan(Handler h) { /* ... */ }
};

class SensorNetwork
{
public:
    SensorNetwork() :
        timer( [this](){ OnTimerExpired(); } )
    {
        timer.Start(10s);
    }
    shared_ptr<SensorAcquisition> GetLastMeasure() const
    {
        lock_guard<mutex> lock(mtx);
        return current;
    }
private:
    void OnTimerExpired()
    {
        filling = make_shared<SensorAcquisition>();
        filling->Scan([this](){ OnScanCompleted(); });
    }
    void OnScanCompleted()
    {
        lock_guard<mutex> lock(mtx);
        current = filling;
    }

    PeriodicTimer timer;
    shared_ptr<SensorAcquisition> filling;
    shared_ptr<SensorAcquisition> current;
    mutable mutex mtx; // protect "current"
};

class Client
{
public:
    Client(const SensorNetwork& sn) : sensors(sn) {}

    // possibly executed in another thread
    void DoSomeWork()
    {
        auto measure = sensors.GetLastMeasure();
        // do something with measure
        // ...
    }
private:
    const SensorNetwork& sensors;
};
```

## Known Uses

<!--
Examples of the pattern found in real systems. We include at least two
examples from different domains.
-->

PATTERNNAME (TODO) is used
to retrieve the periodic diagnostic of network objects
in several applications I worked on.
 
Unfortunately, I cannot reveal the details due to the usual confidentiality constraints.


## Related Patterns

<!--
What design patterns are closely related to this one? What are the
important differences? With which other patterns should this one be used?
-->

* The pattern is somewhat similar to "Double Buffer", but PATTERNNAME (TODO) 
  allows multiple clients to read the state, each at its convenient pace.
  Moreover, in PATTERNNAME (TODO) there can be multiple data structures simultaneously,
  while in "Double buffer" we have always two buffer
  (one for writing and the other for reading).
  Finally, in "Double Buffer" the buffer are *swapped*, while in PATTERNNAME (TODO)
  the data structures are *passed* from the writer to the readers and, eventually,
  deleted.
* `Snapshot` can/should be a "Façade" for a complex data structure.
* `Source` can use a "Strategy" to change the policy of update (e.g., periodic VS continuous).

----
