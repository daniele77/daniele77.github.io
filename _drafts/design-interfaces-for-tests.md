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

During a talk I gave some times ago, I presented an idea I used successfully in the last few years i.e., creating C++ desktop applications with html UIs. At the conference, I explained how — using html5 websockets — one can add a html UI to a desktop application without using a webserver to serve the pages. Maybe this will be a subject for another post, in the meantime if you’re interested, you can have a look to the slide of my talk [insert link]. This post, instead, is about ###.

The people attending the conference liked the approach above all because of the test ability of my solution, that I remarked when summarizing the benefits of the approach. In particular, I highlighted the ###

Near the end of my talk, summarizing the pros and cons of the approach, I explained that — given we already have a network protocol in place — the UI can be easily removed and replaced by a websocket client able to automatically test our C++ application. For example, given an application with a UI that interacts with custom hardware we can do something similar to the following diagram:

The test application can stimulate the inputs and check the outputs of our application to test every path we want to cover.

Many people were very impressed from this property of the solution I proposed, so much so that, after the talk more than one told me he were going to try the solution because it was so easy to test and hey, testing is so important!

Well, I’m very glad if many people will adopt a solution I proposed. After all, that was the goal of my presence at the conference in the first place: spread an idea I tried in the past that worked for me. But to tell the truth, actually you don’t need this architecture to have testable applications: as I said at the beginning of my talk, the websocket protocol forces us to do something we usually don’t do: carefully design the interface between application and UI (and, more in general: between different components of our application). But this does not mean we can’t have a modular application without a network protocol in the middle! Of course we can always have a good separation between UI and business model.
[Having a protocol between the two components obliges us to separate the application in a sharp way, decide which parts put in each component, and carefully think about their interactions.]

To achieve this goal, we should cut the application in the right way, and have the business logic not polluted with concepts about the presentation. So, the business logic components should not know about widgets or UI events.
