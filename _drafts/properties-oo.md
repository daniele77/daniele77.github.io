---
layout: post
title: Properties of different designs
published: true
category: general
tags: [design, C++]
---

Look at this code:

{% highlight cpp %}
RestApi::Response RestApi::process(
  std::string const& requestUri,
  std::string const& requestMethod,
  std::string const& requestContent) const
{
  using ResourceFunction = std::function<Response(std::string const&, std::smatch const&)>;
  struct Resource {
    std::string uriPattern;
    std::string allowedMethod;
    ResourceFunction impl;
  };

  std::vector<Resource> resources {
      {
          "/issue/new",
          "POST",
          [this](std::string const& requestContent, std::smatch const& id_match) {
            return issue_new(requestContent);
          }
      },
      {
          "/issue/list",
          "GET",
          [this](std::string const& requestContent, std::smatch const& id_match) {
            return issue_list();
          }
      },
      {
          "/issue/([0-9]*)",
          "GET",
          [this](std::string const& requestContent, std::smatch const& id_match) {
            auto id_string = id_match[1].str();
            return issue_id(id_string);
          }
      }
  };

  for (auto const& resource : resources)
  {
    std::regex uriRegex{resource.uriPattern};
    std::smatch uriMatch;
    if (std::regex_match(requestUri, uriMatch, uriRegex)) {
      if (requestMethod != resource.allowedMethod) {
        return Response::methodNotAllowed();
      }
      return resource.impl(requestContent, uriMatch);
    }
  }

  return Response::notFound();
}
{% endhighlight %}

As you can see, a vector of Resources is filled so that, when a request arrives, the function looks in the vector of 
Resources for an item with a path that matches the uri request and its method.


* do you have real extensibility?
* It's fake extensibility.
* See Pescio definition of OO structure.
* How can I add methods by adding artifacts?

* OO polymorphism (virtual functions) VS struct with std::function.
* With the first you have access to object data member.

What's the difference with a switch case? What's the benefit? Maybe this code seems better because it doesn't use 
explicitly a switch-case, but do you remember why we should avoid the switch case in the first place? If we remember 
the deep motivation, maybe we can understand that something like that:

=> put the switch case example here>

Has the same non functional properties that the code above.
In particular, every time I must add a new rule, I need to modify RestApi::process to include the new condition and 
a call to a private method, and add a new private method to handle the new kind of request.
In the original code, I'd need to do more or less the same changes, with the difference that instead of writing 
a new "if condition" I must add a new vector element (containing exactly the same code).
It's not that much different, right?

I think this is pretty good code, surely above the mean. But can we do better?
... well, first we should define what we mean with "better code" :-)
Every time we face a design choice we should have clear in mind what are our non-functional goals 
(because, well, I hope our functional requirement @@@). How do we choose among different shapes of our code, 
otherwise? @@@ elaborate.
I'll try to enumerate some of the possible goals we can have in mind (more or less consciously): correctness, 
understandability,  testability, reusability, extensibility... Please note that none of these properties has (yet) 
a shared formal definition. However, we can empirically say if a piece of code is more testable than another, 
or if it's extensible in a simpler way, and so on.
[@@@ yeah, I know, someone will assert that he doesn't care about non functional properties, because he writes 
the simpler solution he can think, and then there is refactoring]

@@@ enough chat, lemme see some code :-)

Let's see another possible design and compare the non-functional properties of the two solutions.

=> insert code with request classes, and context

As you can see, this time I used good old run-time polymorphism. I create a list of Activity (@@@ change name), 
select the one correspondent to the request, invoke its Exec method, send the response.

## Two problems

The new solution is not very different from the original code, but has a new interesting property: 
adding a new kind of request requires only adding a new class, without touching existing code. 
Why should we care? Well, because if we just add an artifact, we can expect we're not breaking the existing code. 
(@@@ expand here?)

Our new code has this property. Well, almost. To get the full potential out of it we have to face two problems.

First: if we really want to be able to implement a new request type by only adding a class, all related activities 
should be contained inside the class. If we add a class and then we need to add a method (or more than one) somewhere else, 
we're cheating. So, our Activity should receive references to everything the concrete activities possibly need to complete 
their task. Sometimes it's as simple as to pass a DB reference. Sometimes we need to pass a reference to some articulated 
object structure of our app, sometimes we need a context. This is one of the main problem we face when doing real OO programming: 
make our interfaces general enough to comprehend every need of [yet to be implemented] derived classes.

Second problem: in a perfect world, I'd like very much that *all* I need to do to include a new request type is to add a new 
file with the class implementation. Unfortunately, someone should create an instance of that class... so chances are I end 
up writing somewhere something like this:

{% highlight c++ %}
...
actions.Add(make_unique<NewIssue>(context));
actions.Add(make_unique<IssueList>(context));
actions.Add(make_unique<GetIssue>(context));
...
{% endhighlight %}

that means to add a new action, I have to add a new file *and* insert a new line of code somewhere. 
Actually it's not a big deal, but we can improve it anyway, by making each class to register itself in the @@@ class. 
It takes a singleton (guess I just shocked a few readers :-)  and a @@@. With that, just linking the class to your application 
makes it available in the @@@ set.

It's not much different than plugins, is it? [@@@ pippone sul fatto che ogni piccola classe può diventare plugin]

## Properties comparison

Well, of course both designs work: they both satisfy the functional requirements, and I can say they're both correct. 
But let's try something new :-) Let's discuss the pros and cons of the alternatives, in terms of non-functional properties, 
and try to be as objective as possible.

### Extensibility

What does "extensibility" mean? To cut it short, a piece of code is extensible when you can easily implement new requirements, 
possibly by adding few code in few places.
In the original solution adding a new request is pretty easy. You need to add a new entry in the resources vector and create the 
corresponding RestApi private method, provided the interface Service exposes all the services you need.
In the second design (let's call it "OO solution", just to give it a name), to add a new request we only need to add a new class. 
If we implemented the auto-register mechanism, we just need to link a new .cpp file to our project and that's it. 
Not having to touch existing code makes us confident that we're not introducing regressions.
From this point of view, the second solution is an Object Oriented Structure, as defined here :
Given a set of (meaningful) changes H1…Hk, occurring over time, an OOS is one that minimizes the total size of the artifacts 
you have to update, encouraging the creation or deletion of entire artifacts instead.
This is the only objective definition I know of an OO program. Trust me: it's worth reading it.

### Testability

What about testing our code? I assume the vast majority of developers should like to perform automated test on their code 
(someone before writing the code, someone else, after).
Well, in the OO solution it's possible to test as a black box the activities and the dispatching mechanism.
In the original solution, instead, testing is a bit more complicated...

### Reusability

Maybe reusability is not on your priority list. You can always copy and paste the piece of code you need and then modify it. 
After all, I did the same thing with the code above for a prototype I needed quickly. And it worked. Don't we all do the same 
thing with Stack Overflow snippets? :-) So, what's the harm really?

Well, maybe. But you're not considering the whole thing. Reusability across different projects is hard. You can write libraries 
or frameworks, and put them in separate repositories. But many times we can reuse code inside the same application! Think about it: 
in the same application, you have high probability to manipulate the same concepts...

So: what can be reused (@@@ lasciando invariato) inside the first solution? Well... I'd say nothing. In the second solution, 
instead we can reuse:
the routing mechanism
the single activities
Maybe the routing mechanism can be reused across applications, but single activities could be useful inside our application. 
[@@@ fare un esempio, controllare se è vero]

### Readability 

## TODO 


- extensibility: one can add an action without touching elsewhere (mechanism to auto register the actions)
- little smart classes
- testability: we can test each action in isolation (in the original code, the actions were private member functions, so one could only hope to test the whole class
- reusability: we can reuse asking le action elsewhere in our app, we can move the predicate inside the action, to reuse the entire mechanism

- trade off between reusability of the mechanism and performance [I can have a bool DoesMatch() const method or the class can expose both the http method and the url/regex]

- conclusions about std::function vs first citizen action classes.
