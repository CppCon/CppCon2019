# Path tracing, done three ways

Some code sketchings for an idea I have to do a presentation on the effects of different
coding styles on design and performance. It's a trivial path tracer (an extended homage to smallpt.cpp)
implemented three different ways. It's not meant to be complete, or correct. Or even performant in a traditional sense
(at the time of writing it has no bounding volume hierarchy acceleration).

"Ways" of implementing the code:

1. Traditional OO with interfaces and virtual calls
2. Functional style using std::variant, optional, ranges
3. Data-oriented design
