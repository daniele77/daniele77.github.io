This is my very first post.

## This is a header 2

Now I want to add some code. 

```c++
// some code
#include <iostream>

using namespace std;

int main()
{
  auto f = [](){ cout << "Hello, blogging world!\n"; }
  f();
  return 0;
}
```

Let's see how it gets formatted...

this is some edit I made later.

I'd like to put also a bit of html in my raw file:

<pre style="background:#fff;color:#000"><span style="color:#1a921c">#<span style="color:#0c450d;font-weight:700">include</span> <span style="color:#d80800">&lt;iostream></span></span>

<span style="color:#0100b6;font-weight:700">using</span> namespace <span style="font-style:italic">std</span>;

int <span style="color:#0000a2;font-weight:700">main</span>()
{
    auto f = [](){ cout &lt;&lt; "Hello, lambda\n"; }
    auto answer = 42;
    if ( answer > 0 ) f();
    return 0;
}
</pre>

... and let's see what happens.
