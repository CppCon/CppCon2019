<div class="white-bg">

## Data-oriented Design

<img src="images/image.owl.png" height="400" class="no-border" alt="An image of a dot-matrix owl drawn with shiny spheres">

</div>

---

<div class="white-bg">

* Design around most common operations
* Design data by access patterns
* Avoid branches
* Polymorphism by separation

</div>

---

<div class="white-bg">

### Changes

* Intersection is most common operation
  * Only need nearest
* Two types of object
  * Separate into two lists

</div>

---


### Scene <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
class Scene {
<div class="fragment highlight-current-code">  std::vector&lt;TriangleVertices> triangleVerts_;
</div><div class="fragment highlight-current-code">  std::vector&lt;TriangleNormals> triangleNormals_;
  std::vector&lt;Material> triangleMaterials_;
</div>
<div class="fragment highlight-current-code">  std::vector&lt;Sphere> spheres_;
</div><div class="fragment highlight-current-code">  std::vector&lt;Material> sphereMaterials_;
</div></code></pre>

---

### Intersection - Spheres <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
double nearestDist = nearerThan;
std::optional&lt;size_t> nearestIndex;

<div class="fragment highlight-current-code" data-fragment-index="1">for (size_t sphereIdx = 0; sphereIdx &lt; spheres_.size(); ++sphereIdx) {
</div>  <span class="fragment highlight-current-code" data-fragment-index="2">// ...maths...</span>
<div class="fragment highlight-current-code" data-fragment-index="3">  if (!hit) continue;
</div>
<div class="fragment highlight-current-code" data-fragment-index="4">  if (distance < currentNearestDist) {
    nearestIndex = sphereIdx; nearestDist = distance;
  }
</div><div class="fragment highlight-current-code" data-fragment-index="1">}
</div>
<div class="fragment highlight-current-code" data-fragment-index="5">if (!nearestIndex) return {}; // missed all spheres
</div>
<div class="fragment highlight-current-code" data-fragment-index="6">// ...more maths to calc hit positions, normal...
return IntersectionRecord{Hit{...}, sphereMaterials_[*nearestIndex]};
</div></code></pre>

---


### Intersection - Triangles<!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
<div class="fragment highlight-current-code" data-fragment-index="1">for (size_t triIdx = 0; triIdx &lt; triangleVerts_.size(); ++triIdx) {
</div><div class="fragment highlight-current-code" data-fragment-index="2">  // ...calc u...maths, only needs triangle vertices...
  if (u < 0 || u > 1) continue;
</div><div class="fragment highlight-current-code" data-fragment-index="3">  // ...calc v...
  if (v < 0 || u + v > 1) continue;
</div><div class="fragment highlight-current-code" data-fragment-index="4">  // ...calc dist...
  if (dist < nearest) { /* note this as nearest */ }
</div><div class="fragment highlight-current-code" data-fragment-index="1">} 
</div>
<div class="fragment highlight-current-code" data-fragment-index="5">// ...more maths to calculate actual normal...
return IntersectionRecord{Hit{...}, triangleMaterials_[*nearestIndex]};
</div></code></pre>
---

<div class="white-bg">

### Rest broadly the same

</div>

---

<div class="white-bg">

### Things I liked

* Ability to optimise
* Performance
  - with caveat<!-- .element: class="fragment" -->

</div>

---

<div class="white-bg">

### Things I didn't like

* Testability
* Difficulty to change

</div>
