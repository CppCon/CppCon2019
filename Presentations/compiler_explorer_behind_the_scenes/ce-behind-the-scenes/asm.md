## Assembly!
<!-- .element: class="white-bg" -->

<div class="fragment white-bg">
<img src="images/One_Ring_inscription.svg" height="200" class="no-border">
<div class="attribution">
<a href="https://commons.wikimedia.org/wiki/User:Ssolbergj">Ssolbergj</a>, Wikimedia Commons CC SA 4.0
</div>
<aside class="notes">
Ash nazg durbatulûk, ash nazg gimbatul,
Ash nazg thrakatulûk agh burzum-ishi krimpatul.
</aside>


### x86-64 Assembly
<!--- .element: class="white-bg" -->

```x86asm
  instr
  instr dest_operand
  instr dest_operand, source_operand
  instr dest_operand, source_operand, source_operand
```

```x86asm
  ret                           ; return
  inc rax                       ; increment "rax"
  mov edx, 1234                 ; set "edx" to the value 1234 
  add rsi, rdi                  ; "rsi" += "rdi" 
  vpaddd ymm1, ymm2, ymm0       ; "ymm1" = "ymm2" + "ymm0"
```
<!-- .element: class="fragment" -->


### Instructions
<!--- .element: class="white-bg" -->

<div class="w30 white-bg"><ul class="instr fragment highlight-current-red">
<li>`mov`</li>
<li>`movzx`</li>
<li>`movsxd`</li>
<li>`lea`</li>
</ul><ul class="instr fragment highlight-current-red">
<li>`call`</li>
<li>`ret`</li>
<li>`jmp`</li>
</ul></div>

<div class="w30 white-bg"><ul class="instr fragment highlight-current-red">
<li>`push`</li>
<li>`pop`</li>
</ul><ul class="instr fragment highlight-current-red">
<li>`cmp`</li>
<li>`test`</li>
<li>`je`</li>
<li>`jne`</li>
</ul></div>

<div class="w30 white-bg"><ul class="instr fragment highlight-current-red">
<li>`and`</li>
<li>`xor`</li>
<li>`add`</li>
<li>`sub`</li>
<li>`shl`</li>
<li>`shr`</li>
<li>`sar`</li>
</ul></div>

<div><br><br></div>

<div class="fragment white-bg">
And many, many more...
</div>


### Operands
<!--- .element: class="white-bg" -->

```x86asm
  register                          ; e.g. rax, rbx, ecx...
  constant                          ; e.g. 1234
  <size> ptr [register]             ; e.g. DWORD PTR [rax]
  <size> ptr [register + offset]    ; e.g. BYTE PTR [rcd + rsi]
  <size> ptr [register + offset + register2 * (1,2,4,8)]
```

```x86asm
  add eax, dword ptr [rdi + 12 + rsi * 4] 
                                ; eax += *(int *)(rdi + 12 + rsi * 4)
```
<!-- .element: class="fragment" -->


### Registers
<!--- .element: class="white-bg" -->

<div class="white-bg">
<ul>
<li>`rax` (return value)</li> 
<li>`rdi` (1st param)</li> 
<li>`rsi` (2nd param)</li> 
<li>`rdx` (3rd param)</li>
<li>`rbx` `rcx` `rbp` `r8-r15` `rsp`</li>
<li>`xmm0-15` (`ymm0-15`... `zmm0-31`... `k0-7`)</li>
</ul></div>


<table class="registers white-bg">
    <thead>
    <tr>
        <th>63...56</th>
        <th>55...48</th>
        <th>47...40</th>
        <th>39...32</th>
        <th>31...24</th>
        <th>23...16</th>
        <th>15...8</th>
        <th>7...0</th>
    </tr>
    </thead>
    <tbody>
    <tr>
        <td colspan="8" class="register rax">rax</td>
    </tr>
    <tr>
        <td colspan="4" class="regnote">(zeroed on write)</td>
        <td colspan="4" class="register eax">eax</td>
    </tr>
    <tr>
        <td colspan="6"></td>
        <td colspan="2" class="register ax">ax</td>
    </tr>
    <tr>
        <td colspan="6"></td>
        <td colspan="1" class="register ah">ah</td>
        <td colspan="1"></td>
    </tr>
    <tr>
        <td colspan="7"></td>
        <td colspan="1" class="register al">al</td>
    </tr>
    </tbody>
</table>


<!-- .slide: data-background="./images/bg/weave.jpg" -->
## Important note on performance
<!-- .element: class="white-bg" -->

<div class="fragment white-bg">
<ul>
<li><a href="https://github.com/google/benchmark">Google Benchmark</a></li>
<li><a href="https://github.com/nickbruun/hayai">hayai</a></li>
<li><a href="https://github.com/rmartinho/nonius">Nonius</a></li>
<li><a href="http://quick-bench.com/">Quick Bench</a></li>
</ul>
</div>