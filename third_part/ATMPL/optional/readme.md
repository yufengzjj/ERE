examples:

```c++
ATMPL::optional<int> int_;
if(int_)
  std::cout<<*int_;//it woulnd not get here!
int_ = 'f';
ATMPL::optional<char> char_ = 's';
int_ = char_;
std::cout << *int_<<"\n";
```