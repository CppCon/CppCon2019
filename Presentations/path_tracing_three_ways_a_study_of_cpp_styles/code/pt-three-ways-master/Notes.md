* Path tracing three ways
  - Trad OO style
  - Functional (optionals, variants for polymorph, free functions?)
  - Data oriented design

### GOALS

* Demonstrate C++'s as a multi-paradigm language
* Compare and contrast approaches
  - performance
  - testability
  - ease of use

## Path tracing

* fire ray from camera. intersect with a world. World is a hierarchy of objects, each bounded by a Bounding sphere/box
* Need to determine intersections. result of intersection:
  - position
  - surface normal
  - material
* Primitives:
  * Spheres
  * Triangles
* Materials
  - emission
  - specular
  - diffraction/transparency - TODO maybe not?

## Maybe...
* Show "pigeon" OO (like, no optional etc)
  * No mesh, just lots of triangles? that's kinda DoD?
* Show "pure" functional
* Show DoD
* Show blend (C++ lets us have :allthethings:)
* Maybe strong types?
* More complex model
* "Mock optimization" / perf... show things are super slow, show it's the divide,
  then actually segue into k-d tree?
* Things to add after initial impl
  * (strong type?)
  * k-d tree
  * "intersection if nearer than XXX" optimisation?

Materials? Inline, or "OO" style?

## Timeline plan...

Written on 27th July
```
     July 2019
Su Mo Tu We Th Fr Sa
                  27
28 29 30 31
    August 2019
Su Mo Tu We Th Fr Sa
             1  2  3
 4  5  6  7  8  9 10
11 12 13 14 15 16 17
18 19 20 21 22 23 24
25 26 27 28 29 30 31
   September 2019
Su Mo Tu We Th Fr Sa
 1  2  3  4  5  6  7
 8  9 10 11 12 13 14
```

* Mondays: Compiler Explorer day
* Conference: Sep 15-20
* Ideally two weeks for practice, so all of September.
* A week clear to write talk, at least. So all versions need to be finished by Aug 24th
* Family visiting July 27th<->Aug 10th
* Away from computers 1st Aug <-> 6th Aug
* So! Only clear weeks: 11->17, 18->24


### UPDATE 10th August

* family gone
* OO and FP separated fairly well
* Need to start on DoD
* Need to get a good "v1"; may need to "undo" multithreading (or have slideware versions for single threading)
* then add k-d?
* Need to re-add (or write) tests
* Maybe relegate "strong type" to "further work" ?

New timeline:
* Aug 11th "get DoD version working"
* 12, 13, 14, 15, 16, 17 - 12, 13 out, 14, 15, 17 available. MUST GET FULL v1 by then:
  * tests
  * three different "ways"
  * Sketch at some slides!

## Interesting occurrances
While extracting OO version from FP/OO blend: got bug!

Bisect:

Noticed error in 9b8afd3cbba34411c60cccb0927b40ae7fce31dd (seemingly broken light bounce?)

```
bisect at 3557d6fd071230f9f5a922885eadefca084fa0fa

~/d/pt-three-ways (master|✔) 30.5s $ git bisect
usage: git bisect [help|start|bad|good|new|old|terms|skip|next|reset|visualize|view|replay|log|run]
~/d/pt-three-ways (master|✔) $ git bisect start
~/d/pt-three-ways (master|BISECTING|✔) $ git bisect bad
~/d/pt-three-ways (master|BISECTING|✔) $ git checkout 3557d6fd071230f9f5a922885eadefca084fa0fa
~/d/pt-three-ways ((3557d6fd…)|BISECTING|✔) $ eog image.png
^C⏎                                                                                                                                         ~/d/pt-three-ways ((3557d6fd…)|BISECTING|✔) 13.8s $ git bisect good
Bisecting: 4 revisions left to test after this (roughly 2 steps)
[eddeeaa83b1bf1ce9942f45aa0c6d92bcf9219ab] Move object loader into util, move more things out of main
~/d/pt-three-ways ((eddeeaa8…)|BISECTING|✔) $ eog image.png
^C⏎                                                                                                                                         ~/d/pt-three-ways ((eddeeaa8…)|BISECTING|✔) 16.4s $ git bisect bad
Bisecting: 2 revisions left to test after this (roughly 1 step)
[0e7c51e8c68dd523a8b53d0449160b87e642310c] First wave of de-FPing main, and separating OO/FP
~/d/pt-three-ways ((0e7c51e8…)|BISECTING|✔) $ eog image.png
^[[A^[[A^C⏎                                                                                                                                 ~/d/pt-three-ways ((0e7c51e8…)|BISECTING|✔) 20.2s $ git bisect good
Bisecting: 0 revisions left to test after this (roughly 1 step)
[bccb0229c8f25d3988eb2e1267ba39511dd5ab03] More stuff into fp
~/d/pt-three-ways ((bccb0229…)|BISECTING|✔) $ eog image.png
^[[A^C⏎                                                                                                                                     ~/d/pt-three-ways ((bccb0229…)|BISECTING|✔) 29.3s $ git bisect good
eddeeaa83b1bf1ce9942f45aa0c6d92bcf9219ab is the first bad commit
commit eddeeaa83b1bf1ce9942f45aa0c6d92bcf9219ab
Author: Matt Godbolt <matt@godbolt.org>
Date:   Wed Jul 31 16:39:21 2019 -0500

    Move object loader into util, move more things out of main

:040000 040000 39439171bb841691c5d54eecf172987c23a99d48 81055b801024c199a9321ac12790d6daeb85b9a3 M      src
:040000 040000 316a4ec815f3e2f0ed553391c9ff29d4480d02e0 0c3e43d162d003a4a0290e10fc669196ac252cb4 M      test
```

WAH WAHhhhhh

```cpp
bool Scene::intersect(const Ray &ray, IntersectionRecord &intersection) const {
  Primitive::IntersectionRecord *currentNearest{};
  IntersectionRecord rec;
  for (auto &primitive : primitives_) {
    if (!primitive->intersect(ray, rec))
      continue;
    if (!currentNearest || rec.hit.distance < currentNearest->hit.distance) {
      currentNearest = &rec;
    }
  }
  if (!currentNearest)
    return false;
  intersection = *currentNearest;
  return true;
}
```

Fixed in a9d700b4f236de3199ed8e4f96e21b2aaa15ff17 but highlights:
* lack of test!
* std::optional more obviously correct! 
