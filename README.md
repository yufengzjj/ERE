# ERE
**Educational Regular expression Engine--purpose for education of automata theory.**

all base on C++11 standard library and a tiny C++11 template library [ATMPL](https://github.com/yufengzjj/ATMPL) .

this regular expression engine only support core operations:alternate,concat and closure.

it uses recursive descent parsing to parse regular expression and then generate finite state automata.it can generate DOT(Graphviz) file of NFA/DFA/miniDFA of the regular expression automata.

examples:

> ERE.exe --help
>
> -h [ --help ] produce help message
>
> -e [ --expr ] arg set regular expression
>
> -s [ --string ] arg string to check by regular expression
>
> -a [ --alphabet ] arg set regular expression alphabet

ERE.exe -e "a(b|cd)*e" -s abbbcdcde

![](doc/1.png)

dot -T png -O NFA.dot DFA.dot min_DFA.dot



![](doc/NFA.dot.png)

![](doc/DFA.dot.png)

![](doc/min_DFA.dot.png)