# Notes

Notes read top (most recent)

## hotel room runthrough
* 10m hot features
* 20m to demo'ing CFG
* 28m behind scenes
* 52m to Q
flowed well, needs more practice on the demo part

## Third runthru at home 55m
* Timeline
  - 12m to "Hot Features"
  - 31m to "Behind Scenes"
  - 55m to Qs
* Went well

## Second CppCon runthrough at work (1h40
* Audience didn't know much about C++ or the site, so took longer
* BUT still skipped over favourite opts....so...culled them
* Need to roll some good optimisation examples into the "what's hot"
* Steer -> show conformance view

## First CppCon runthrough at home (1h5)

* Timeline
  * 10m to "2018"
  * 18m was fiddling with llcm-mca
  * roughly 30m to end of CE demo
  * 44m to "summation"
    * dropped summation and devirtualisation
  * Behind the scenes took til 1.05, with improv'd isolation talk
* TODO
  * FINALISE WHAT GOES INTO DEMO AND MAKE SHORT LINKS
    * Rust vs c++ "square"
     * demos languages, diff mode, window mgmt -- DONE
    * Load up sum optimised:
      - llvm-mca demo; llvm tool -- DONE
    * info on ASM ctrl-f10
    * code higlighting ctrl-f10 toggle
    * show:
       * CFG
       * clang opt view, ast
       * gcc pass view
    * `#include <url>`
  * Install and get working locally:
    * llvm-mca "analysis" -- DONE
    * clang-tidy -- DONE
  * Decide whether to drop whole "favourite optimisations", or only
    use them to show things above.
    - used divide to show off llvm-mca...
  * Write "how sandboxing works" slide(s) -- DONE (ish)
  

## Readying for CppCon

* update stats for up-to-date - done
* add a section on "power features of CE":
  * execution, new pane as well as normal
  * Herb surprised by #include URL and mouse-over asm

So, what features to showcase?
* multiview, stacking windows?
* diff view
* pahole
* llvm-mca
* mouse over
* `#include` URLs
* GCC and clang tree/RTL viewer
* other languages

## From first presentation April 2019

Raw form notes below...

* Stats
  * Roughly 3000 short URLs made a week: https://www.stathat.com/s/3cKhl15ATGVF
  * What do people mostly use?
* when did it get renamed compiler explorer?
* Show tools behind the scenes
  * AWS
  * `ce`
  * stathat
  * papertrail
  * sentry
* Show docker stuff
* Show daily compiler build
* Show daily trunk update
* Terraform
* PRs for adding stuff
* EFS stuff
* Hacking attempts
  * Why "execution support" has been Coming Soon™ for so long
* Outages
  * EFS transfer limit (May 2017) post OS upgrade
  * S3 outage (Feb 2017)
* Little known stuff
  * Tool windows
    * Show llvm mca
  * LLVM IR
  * CE API mode -- count compilers example?
* cost

* Top 5 Favourite compiler optimisations
  1. `POPCNT`
  2. constant-time sum
  3. `isspc`
  4. divide -> multiply
  5. ???
  
Conclusion
* Community
* Thanks to
  * Rubén
  * Partouf
  * Chedy
  * Jared
  * Patreons!

* THE FUTURE
