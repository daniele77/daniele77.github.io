---
layout: post
title: "TODO - Good interfaces and components separation"
published: true
category: general
tags: [C++, design]
---

<!--
•	Talk. Brief summary. Another future post. Link to the slides.
•	Summarizing the benefits, I told them about testing.
•	Show draft and explain the test part
•	You don’t need this architecture to have testable applications: as I said at the beginning, the protocol force us to do something we usually don’t do...
•	Of course we can always have a good separation between UI and business model
•	Show OO technique of separation, and how test it by using polymorphism 
•	... the property of test ability is not provided by the websocket protocol in se, but form the right separation (that is mandatory to have a protocol of communication)
•	We always need this separation? (No)
•	We always need this kind of test? (No) 
-->

During a talk I gave some times ago, I presented an idea I used successfully in the last few years i.e., creating C++ desktop applications with html UIs. At the conference, I explained how — using html5 websockets — one can add a html UI to a desktop application without using a webserver to serve the pages. Maybe this will be a subject for another post, in the meantime if you’re interested, you can have a look to the slides of my talk [insert link]. This post, however, is about the importance of splitting an application into the right abstractions, designing carefully their interfaces and choose the right mechanism of interaction.

The people attending the conference liked the approach above all because of the testability of my solution, that I remarked when summarizing the benefits of the approach. In particular, I highlighted the ###

Near the end of my talk, summarizing the pros and cons of the approach, I explained that — given we already have a network protocol in place — the UI could be easily removed and replaced by a websocket client able to automatically test our C++ application. For example, given an application with a UI that interacts with custom hardware we can do something similar to the following diagram:

![testing the application]({{ site.baseurl }}/images/design-interfaces-for-tests_split.png)

The test application can stimulate the inputs and check the outputs of our application to test every path we want to cover.

Many people were very impressed from this property of the solution I proposed, so much so that, after the talk more than one told me he were going to try the solution because it was so easy to test and hey, testing is so important!

Well, I’m very glad if many people will adopt a solution I proposed. After all, that was the goal of my presence at the conference in the first place: spread an idea I tried in the past that worked for me. But to tell the truth, you actually don’t need this architecture to have testable applications: as I said at the beginning of my talk, the protocol simply forces us to do something we usually don’t do: carefully design the interface between application and UI (and, more in general: between different components of our application). But this does not mean we can’t have a modular application without a network protocol in the middle! Of course we can always have a good separation between UI and business model.
[Having a protocol between the two components obliges us to separate the application in a sharp way, decide which parts put in each component, and carefully think about their interactions.]

To achieve this goal, we should cut the application in the right way, and have the business logic not polluted with concepts about the presentation. So, the business logic components should not know about widgets or UI events. In the exact same way, the UI part should not know about details of implementation of the business logic part.
In this way, we can have a robust interface and we will be able to remove the UI component and replace it with some test component.

One simple way to do so is by using the good old runtime polimorphysm. 

...

Having a network protocol in place certainly gives us the possibility to test the application by simply attach a new client to the other end of the channel that stimulates the logic as if it were the Ui. But what really makes our application testable is not the protocol, but 

...

(conclusions)
[almost] everything I wrote is correct under the assumption that my goal is to be able to perform automatic black box tests of the business model of my application. This is not always the case. Sometimes I want other properties in my application, and those properties requires a different form of my software.
In general, a universal solution / technique does not exist. When we face a new project, we should focus on the (non functional) properties we want to give to it, and decide which is the form most @@@ (adatta).

But every application need test, isn't it?! So, testability should be at the top of our list of non-functional requirements, whatever is the application we're writing. Well... yes and no. The only property that every software must have, regardless its kind (embedded, real-time, enterprise,...) is *correctness*. And to be confident this property holds, we must test the application in some way. But tests come in different forms: unit, integration, automatic, manual, black box, white box, ... In this post I wrote about automatic tests, but often applications with a UI are tested as black boxes, in a manual way. And sometimes, we’re called to design applications in which extendibility is more important than automatic testing. For a certain kind of applications, a different partitioning of components is better. We’re used to split applications in horizontal layers (e.g., DB, business logic, UI) every concept having its representation in each layer. But what if we instead cut the software in vertical slices, each containing every technological aspect of a single concept? Adding a new concept would require a new slice. On the other hand, in a single slice maybe we need to mix presentation aspects with persistence aspects.
