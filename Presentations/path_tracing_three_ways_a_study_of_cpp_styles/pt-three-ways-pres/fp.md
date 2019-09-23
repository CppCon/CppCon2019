<div class="white-bg">

## Functional Programming

<img src="images/image-multi-ball.png" height="400" alt="An array of spheres with subtly different appearances">

</div>

---

<div class="white-bg">

## Functional Programming

* `ranges_v3`
* `tl::optional`

</div>

---

### Scene <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
struct TrianglePrimitive {
  Triangle shape;
  Material material;
};

struct SpherePrimitive {
  Sphere shape;
  Material material;
};

<div class="fragment highlight-current-code" data-fragment-index="1">using Primitive = std::variant&lt;TrianglePrimitive, SpherePrimitive>;
</div></code></pre>

---

### Render <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
<div class="fragment highlight-current-code" data-fragment-index="3">auto renderOnePixel = [...](auto tuple) {
  auto [y, x] = tuple;
  std::mt19937 rng(...);
  return radiance(scene, rng, 
        camera.randomRay(x, y, rng), 0, renderParams);
};
</div>
return ArrayOutput(width, height, 
<div class="fragment highlight-current-code" data-fragment-index="1">  view::cartesian_product(view::ints(0, height),
                          view::ints(0, width))
</div><div class="fragment highlight-current-code" data-fragment-index="2">  | view::transform(renderOnePixel));
</div>
</code></pre>

---

### Radiance <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
<div class="fragment highlight-current-code" data-fragment-index="4">auto radianceForRay = [&](const Ray &ray) {
  return radiance(scene, rng, ray, depth + 1, renderParams);
};
</div>
const auto incomingLight = <span class="fragment highlight-current-code" data-fragment-index="5">accumulate(</span>
<div class="fragment highlight-current-code" data-fragment-index="1">    views::cartesian_product(views::ints(0, numVSamples),
                             views::ints(0, numUSamples))
</div><div class="fragment highlight-current-code" data-fragment-index="2">        | views::transform(toUVSample)
</div>        | views::transform([&](auto s) {
<div class="fragment highlight-current-code" data-fragment-index="3">            return radianceAtIntersection(
                <span class="fragment highlight-current-code" data-fragment-index="4">radianceForRay</span>, *intersectionRecord,
                ray, s.first, s.second, unit(rng));
</div>          }),
    <span class="fragment highlight-current-code" data-fragment-index="5">Vec3());</span>
</code></pre>

---

<pre><code class="cpp" data-trim data-noescape>
template &lt;typename RadianceFunc>
Vec3 radianceAtIntersection(
        <span class="fragment highlight-current-code" data-fragment-index="1">RadianceFunc &&radiance</span>,
        const IntersectionRecord &intersectionRecord,
        const Ray &ray, double u, double v, double p) {
  // [...lots of material stuff...]
  if (p < reflectivity) {
    const auto newRay =
        Ray(hit.position, coneSample(/*...*/, u, v));
    return <span class="fragment highlight-current-code" data-fragment-index="1">radiance(newRay)</span>;
  } else {
    const auto newRay = Ray(hit.position, 
        hemisphereSample(/*...*/, u, v);
    return mat.diffuse * <span class="fragment highlight-current-code" data-fragment-index="1">radiance(newRay)</span>;
  }
}
</code></pre>

---

### Intersection <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
struct IntersectVisitor {
  const Ray &amp;ray;

  template &lt;typename PrimT>
  auto operator()(const PrimT &primitive) const {
    return <span class="fragment highlight-current-code" data-fragment-index="2">primitive.shape.intersect(ray)</span>
        .<span class="fragment highlight-current-code" data-fragment-index="3">map([&primitive](auto hit) {</span>
<div class="fragment highlight-current-code" data-fragment-index="4">            return IntersectionRecord{hit, primitive.material};
</div>        });
  }
};

<div class="fragment highlight-current-code" data-fragment-index="1">tl::optional&lt;IntersectionRecord> intersect(
    const Primitive &primitive, const Ray &ray) {
  return std::visit(IntersectVisitor{ray}, primitive);
}
</div></code></pre>

---

<pre><code class="cpp" data-trim data-noescape>
tl::optional&lt;Hit> Sphere::intersect(const Ray &ray) const noexcept {
  const auto determinant = /*...maths...*/;
  return <span class="fragment highlight-current-code" data-fragment-index="1">safeSqrt(determinant)</span>

      .<span class="fragment highlight-current-code" data-fragment-index="2">and_then</span>([&b](double root) -> tl::optional&lt;double> {
        /* ...calc minusT, plusT.. */
<div class="fragment highlight-current-code" data-fragment-index="3">        if (/* ...too close to zero...*/)
          return tl::nullopt;
</div><div class="fragment highlight-current-code" data-fragment-index="4">        return minusT > Epsilon ? minusT : plusT;
</div>      })

      .<span class="fragment highlight-current-code" data-fragment-index="5">map</span>([this, &ray](double t) {
<div class="fragment highlight-current-code" data-fragment-index="6">        return Hit{t, /*...*/};
</div>      });
}
</code></pre>

---

<div class="white-bg">

### Things I liked

* `const` `:allthethings:`
* Code clearer...maybe?
* Testability

</div>

---

<div class="white-bg">

### Things I didn't like

* Cryptic compiler error messages
* Concern I've broken FP rules (`rng`...)
* Performance
  - `std::mt19937 rng` per pixel?

</div>