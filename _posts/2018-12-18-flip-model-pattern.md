---
layout: post
title: "Flip Model: A Design Pattern"
published: true
category: general
tags: [design, C++]
excerpt_separator: <!--more-->
---

*This article was originally published in the
[December 2018 issue of ACCU Overload Journal](http://accu.org/index.php/journals/c393/){:target="_blank"}.*

----

In this article, I describe a design solution that I have adopted several times in the past
while working on applications for the diagnosis of complex distributed systems.
This solution worked well in several contexts, and it’s still proving robust in many running systems.

<!--more-->

Although I know for sure it’s used by other developers, after some research I could not
find any reference to it in the literature, and this finally convinced me to write about it.

After some thought, I decided to document it under the well-known form of a *Design Pattern*
as I believe that it’s still a convenient way to discuss *software design* and *architectural design*
(that – from my point of view – remain fundamental topics in Software Engineering
and should not be neglected in favor of more mundane topics).

Furthermore, some young developers might not know the book
[*Design Patterns*](https://www.amazon.com/Design-Patterns-Elements-Reusable-Object-Oriented-dp-0201633612/dp/0201633612/){:target="_blank"}
that made history, so I hope this article might fill a gap and makes them curious about patterns
and software design in general.

In the remainder of the article, I present the pattern following the classic documentation format
proposed in the original book (see the ‘documentation’ section in the
[Wikipedia article](http://en.wikipedia.org/wiki/Software_design_pattern#Documentation){:target="_blank"}
or – even better – read the original book).

## Pattern name and Classification

Flip Model (behavioral).

## Intent

The pattern allows multiple clients to read a complex data model
that is continuously updated by a unique producer, in a thread-safe fashion.

## Also known as

Model publisher, Pressman, Newsagent.

## Motivation (Forces)

Sometimes it’s necessary to decouple the usage of a complex data structure from its source,
in such a way that every actor can run at their own pace without interfering with each other.

Consider, for example, an application that periodically retrieves information from a large
sensor network to perform some kind of statistical elaboration on the collected data set
and send alarms when some criteria are met. The data collected from the sensor network is
structured in a complex lattice of objects resembling the ones you would find in the
physical world so that the elaboration modules can navigate the data in a more natural way.
The retrieval operation is a long, complex task, involving several network protocols, that
is completely uncorrelated from the statistical analysis and alarms evaluation, and can
possibly run in separated threads. Moreover, data retrieval and its usage have different
timing (e.g., the sensor network is scanned every 5 minutes, while the statistical
elaboration is performed on request by a human operator on the most recent collected
dataset).

In this scenario, how can all the modules of the application work together on the same data structure?
How can all the clients use the most updated data available in a consistent fashion?
And how can the application get rid of the old data when it is no longer needed?

The main idea of this pattern is to pass the sensor data structure from the producer
to the consumers by means of two shared pointers (in C++) or two variables
(in languages with garbage collection): one (named `filling`) holding the object structure
currently retrieving the sensor data, the other (named `current`) holding
the most recent complete acquisition.

A class `SensorNetwork` decides when it’s time to start a new acquisition and replaces
`current` with `filling` when the acquisition is concluded. When a client needs to
perform some tasks on the data acquired, it contacts `SensorNetwork`, which returns
`current` (i.e., the most recent data acquired). An object of class
`SensorAcquisition` is kept alive and unchanged during the whole time a
client holds the smart pointer (and the same is still valid in garbage collected languages).

The data acquisition (performed by `SensorAcquisition`) and its reading
(performed by the various clients: `Statistics`, `ThresholdMonitor` and
`WebService`) are possibly executed in multiple threads. The safety of
the code is ensured by the following observations:

* a `SensorAcquisition` object can be modified only by the thread of `SensorNetwork`,
  and never changed after it becomes public (i.e., the smart-pointer `current`
  is substituted by `filling`)
* the smart pointer exchange is protected by a mutex.

It is worth noting that here the mutex is required because `std::shared_ptr`
provides a synchronization mechanism that protects its control-block but not
the `shared_ptr` instance. Thus, when multiple threads access the same `shared_ptr`
and any of those accesses uses a non-const member function, you need to provide
explicit synchronization. Unfortunately, our code falls exactly under that case
since the method `SensorNetwork::ScanCompleted` assigns the `shared_ptr` to a new value.

However, if the presence of a mutex makes you feel back in the eighties,
please see the ‘Implementation’ section for some modern alternatives.

The following figure shows a typical Flip Model class structure.

![motivation](/images/flip-pattern/motivation.png)

## Applicability

Use Flip Model when:

* You have a complex data structure slow to update.
* Its clients must asynchronously read the most updated data available
  in a consistent fashion.
* Older information must be discarded when is no longer needed.

## Structure

The following figure shows the structure.

![structure](/images/flip-pattern/structure.png)

## Participants

* `Snapshot` (`SensorAcquisition`)
  * Holds the whole set of data acquired by the source.
  * Performs a complete scan.
  * Possibly provides const function members to query the acquisition.
  * Possibly is a set of (heterogeneous) linked objects (e.g., a list of `Measure` objects).
  
* `Source` (`SensorNetwork`)
  * Periodically asks the source to perform a new scan.
  * Provides the latest complete scan to its clients.

* `Client` (`WebService`, `ThresholdMonitor`, `Statistics`)
  * Asks the `Source` for the latest `Snapshot` available and uses it (in read-only mode).

## Collaborations

* Periodically, `Source` creates a new `Snapshot` instance, assigns it to the `shared_ptr`
  `filling`, and commands it to start the acquisition.
* When the acquisition is terminated, `Source` performs the assignment `current=filling`
  protected by a mutex.
  If no clients were holding the previous `current`, the pointed
  `Snapshot` is automatically destroyed (by the shared pointer).
* When a client needs the most updated `Snapshot`, it `calls Source::GetLastSnapshot()` 
  that  returns `current`.

The following figure shows the collaborations between a client, a source and the snapshots it creates.

![structure](/images/flip-pattern/collaboration.png)

## Consequences

* Flip Model decouples the producer from the readers:
  the producer can go on with the update of the data (slow)
  and each reader gets each time the most updated version.
* Synchronization: producer and readers can run in different threads.
* Flip Model grants the coherence of all the data structures that are read in a given
  instant from a reader, without locking them for a long time.
* Memory consumption to the bare minimum to ensure that every reader has a coherent access
  to the most recent snapshot.

## Implementation

Here are 8 issues to consider when implementing the Flip Model pattern:

1. A new acquisition can be started periodically (as proposed in the example) or
   continuously (immediately after the previous one is completed). In the first case, the
   scan period must be longer than the scan duration. Should the scan take longer, it is
   automatically discarded as soon as the timer shoots again.

2. The pattern is described using C++, but it can be implemented as well in languages
   with garbage collection.
   In C++, `std::shared_ptr` is necessary to ensure that a `Snapshot` is deleted when no
   client is using it and `Source` has a more updated snapshot ready.
   In a language with garbage collection, the collector will take care of deleting
   old snapshots when they’re no longer used (unfortunately this happens at some
   unspecified time, so there can be many unused snapshots in memory).

3. The `std::shared_ptr` (or garbage collection) mechanism will work correctly
  (i.e., old snapshots are deleted) only if clients use `Source::GetLastSnapshot()`
  every time they need a snapshot.

4. `Snapshot` (and the application in general) can be synchronous or asynchronous.

   In the first case, the method `Snapshot::Scan` is a blocking function and the
   caller (i.e., `Source`) must wait until the data structure is completed before acquiring the
   mutex and assigning `current` to `filling`. Within a synchronous application, clients
   will run in other threads.

   In the second case, the method `Snapshot::Scan` starts the acquisition operation and
   returns right away. When the data structure is completed, an event notification
   mechanism (e.g., events, callbacks, signals) takes care to announce the end of the
   operation to `Source`, that can finally acquire the mutex before assigning `current` to
   `filling`.
   An asynchronous application can be single-thread or multi-thread.

5. The pattern supports every concurrency model: from the single thread (in a fully
   asynchronous application) to the maximum parallelization possible (when the acquisition
   has its own threads, as well as each client).

   When the acquisition and the usage of the snapshots run in different threads,
   a synchronization mechanism must be put in place to protect the `shared_ptr`.
   While the simplest solution is to add a mutex, starting from C++11 you can
   use instead the overload functions
   [`std::atomic_...<std::shared_ptr>`](https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic){:target="_blank"}
   (and maybe from C++20 [`std::atomic_shared_ptr`](https://en.cppreference.com/w/cpp/experimental/atomic_shared_ptr){:target="_blank"}).
   A point worth noting here is that the implementation of the atomic functions
   might not be lock-free (as a matter of fact, my tests with the latest gcc
   version show that they’re not): in that case, the performances are likely
   worse than the version using the mutex.

   A better solution could be to use an atomic `int` as a key to select the right `shared_ptr`
   (see the ‘Sample code’ section for more details).

6. The objects composing `Snapshot` (usually a huge complex data structure) are (possibly)
   deleted and recreated at every scan cycle. It’s possible to use a pool of objects
   instead (in this case `shared_ptr` must be replaced by a reference counted pool object
   handler).

7. Please note that `Snapshot` (and the classes it represents) is immutable. After its
   creation and the scan is completed, the clients can only read it. When a new snapshot is
   available, the old one is deleted, and the clients will read the new one. This is a big
   advantage from the concurrency point of view: multiple clients running in different
   threads can read the same snapshot without locks.

8. Be aware of stupid classes! `Snapshot` (and the classes it represents) should not be a
   passive container of data. Every class should at least contribute to retrieve its own
   data, and one could also consider whether to add methods and facilities to use the data.

## Sample Code

The C++ code shown here sketches the implementation of the Flip Model classes
described in the ‘Motivation’ section.

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

The previous code uses a mutex for clarity. A lock free alternative is shown in
the following code:

```c++
class SensorNetwork
{
public:
  SensorNetwork() :
    timer( [this](){ OnTimerExpired(); } )
  {
    // just to be sure :-)
    static_assert(current.is_always_lock_free); // c++17

    timer.Start(10s);
  }
  shared_ptr<SensorAcquisition> GetLastMeasure() const
  {
    assert(current < 2);
    return measures[current];
  }
private:
  void OnTimerExpired()
  {
    // filling = 1-current
    assert(current < 2);
    measures[1-current] = make_shared<SensorAcquisition>();
    measures[1-current]->Scan([this](){ OnScanCompleted(); });
  }
  void OnScanCompleted()
  {
    current.fetch_xor(1); // current = 1-current
  }

  PeriodicTimer timer;
  shared_ptr<SensorAcquisition> measures[2];
  // c++14
  atomic_uint current{0}; // filling = 1-current
};
```

Just in case you were wondering, you do need an *atomic* integer type here,
although only one thread is writing it
(have a look at
[C++ memory model](https://en.cppreference.com/w/cpp/language/memory_model){:target="_blank"}
to go down the rabbit hole).

## Known Uses

Flip Model is used to retrieve the periodic diagnosis of network objects
in several applications I worked on. Unfortunately, I cannot reveal the
details given the usual confidentiality constraints that apply to these projects.

## Related Patterns

* The pattern is somewhat similar to ‘Ping Pong Buffer’ 
  (also known as
  "[Double Buffer](http://gameprogrammingpatterns.com/double-buffer.html){:target="_blank"}"
  in computer graphics),
  but Flip Model allows multiple clients to read the state, each at its convenient pace.
  Moreover, in Flip Model, there can be multiple data structures simultaneously,
  while in ‘Ping Pong Buffer’/‘Double Buffer’ there are always two buffers
  (one for writing and the other for reading).
  Finally, in ‘Ping Pong Buffer’/‘Double Buffer’, buffers are *swapped*, while in
  Flip Model the data structures are *passed* from the writer to the readers and eventually deleted.
* `Snapshot` can/should be a
   "[Façade](http://en.wikipedia.org/wiki/Facade_pattern){:target="_blank"}"
  for a complex data structure.
* `Source` can use a
  "[Strategy](http://en.wikipedia.org/wiki/Strategy_pattern){:target="_blank"}"
  to change the policy of update (e.g., periodic VS continuous).

