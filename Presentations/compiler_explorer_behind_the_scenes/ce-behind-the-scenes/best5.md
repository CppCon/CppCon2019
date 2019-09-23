## Favourite Optimisations
<!-- .element: class="white-bg" -->

---

### Division by a constant <!-- .element: class="white-bg" -->

```cpp
unsigned divideXbyY(unsigned x, unsigned y)
{
  return x / y;
}
```

---

### Counting set bits <!-- .element: class="white-bg" -->

```cpp
int countSetBits(unsigned a)
{
  int count = 0;
  while (a != 0)
  {
    count++;
    // clear bottom set bit
    a &= (a - 1);
  }
  return count;
}
```

---

### Chained conditionals <!-- .element: class="white-bg" -->

```cpp
bool isspc(char c)
{
  return c == ' '
    || c == '\r'
    || c == '\n'
    || c == '\t';
}
```

---

```x86asm
  movabs rax, 0x100002600  ; 0b100000000000000000010011000000000
                           ;   ^                  ^  ^^
                           ;   +- bit 32          |  ++- b10 & 9
                           ;              bit 13 -+
  shrx rax, rax, rdi       ; shift right 'rdi' times
  and eax, 1               ; pick the lowest set bit
```

---

### Summation <!-- .element: class="white-bg" -->

```cpp
int sumToX(int x)
{
  int result = 0;
  for (int i = 0; i < x; ++i)
  {
    result += i;
  }
  return result;
}
```

---

## Devirtualisation <!-- .element: class="white-bg" -->

```cpp
/// g82:-O3 -march=haswell -fno-tree-vectorize
// setup
  #include <vector>
  #include <numeric>
  using namespace std;

struct Func
{
  int operator()(int x)
  {
    return x * x;
  }
};

int sumFunc(const vector<int> &v,
            Func &func)
{
  int res = 0;
  for (auto i : v) res += func(i);
  return res;
}
```
