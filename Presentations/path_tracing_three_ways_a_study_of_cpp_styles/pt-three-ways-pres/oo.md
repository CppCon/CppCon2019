<div class="white-bg">

## Object Oriented

</div>

---

<div class="white-bg">

### Major components

* Scene
* Renderer
* Radiance calculation
* Materials
* Intersection

</div>

---

### Scene <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
class Primitive {
public:
  virtual ~Primitive() = default;
  
<div class="fragment highlight-current-code" data-fragment-index="1">  struct IntersectionRecord {
    Hit hit;
    const Material *material = nullptr;
  };
</div>
  [[nodiscard]] <span class="fragment highlight-current-code" data-fragment-index="2">virtual</span> <span class="fragment highlight-current-code" data-fragment-index="3">bool</span> intersect(
    const Ray &ray, 
    <span class="fragment highlight-current-code" data-fragment-index="3">IntersectionRecord &intersection</span>) const = 0;
};
</code></pre>

<aside class="notes">
SOME NOTESES
</aside>

---

### Render <!-- .element: class="white-bg" -->

<pre><code class="cpp" data-trim data-noescape>
<div class="fragment highlight-current-code" data-fragment-index="1">for (int y = 0; y < height; ++y) {
  for (int x = 0; x < width; ++x) {
</div>    Vec3 colour;
<div class="fragment highlight-current-code" data-fragment-index="2">    for (int sample = 0; sample < numSamples; ++sample) {
</div>      <span class="fragment highlight-current-code" data-fragment-index="3">auto ray = camera_.randomRay(x, y, rng);</span>
      colour += <span class="fragment highlight-current-code" data-fragment-index="4">radiance(rng, ray, 0);</span>
<div class="fragment highlight-current-code" data-fragment-index="2">    }
</div>    <span class="fragment highlight-current-code" data-fragment-index="5">output.plot(x, y, colour / numSamples);</span>
<div class="fragment highlight-current-code" data-fragment-index="1">  }
}
</div>
</code></pre>

---

### Radiance <!--- .element: class="white-bg" --->

<pre><code class="cpp" data-trim data-noescape>
Vec3 Renderer::radiance(
    std::mt19937 &rng, const Ray &ray, int depth) const {

<div class="fragment highlight-current-code">  if (depth >= MaxDepth)
    return Vec3();
</div>
<div class="fragment highlight-current-code">  Primitive::IntersectionRecord intersectionRecord;
  if (!scene_.intersect(ray, intersectionRecord))
    return Vec3();
</div>
<div class="fragment highlight-current-code">  int numUSamples = depth == 0 ? renderParams_.firstBounceUSamples : 1;
  int numVSamples = depth == 0 ? renderParams_.firstBounceVSamples : 1;
</div></code></pre>

<img src="images/render_pt6.png" height=200 class="no-border fragment" alt="Light scattered randomly from a point on a sphere">

---

<pre><code class="cpp" data-trim data-noescape>
  Vec3 result;
<div class="fragment highlight-current-code" data-fragment-index="2">  Sampler sampler(*this, rng, depth + 1);
</div>  for (auto uSample = 0; uSample < numUSamples; ++uSample) {
    for (auto vSample = 0; vSample < numVSamples; ++vSample) {
<div class="fragment highlight-current-code" data-fragment-index="1">      auto u = (uSample + unit(rng)) / numUSamples;
      auto v = (vSample + unit(rng)) / numVSamples;
      auto p = unit(rng);
</div>
<div class="fragment highlight-current-code" data-fragment-index="2">      result += material.sample(hit, ray, sampler, u, v, p);
</div>    }
  }
<div class="fragment highlight-current-code" data-fragment-index="3">  return material.totalEmission(result / (numUSamples * numVSamples));
</div>}
</code></pre>

---

### Intersection <!--- .element: class="white-bg" --->

<pre><code class="cpp" data-trim data-noescape>
struct SpherePrimitive : Primitive {
  Sphere sphere;
  std::unique_ptr&lt;Material> material;

  [[nodiscard]]  
  bool intersect(const Ray &ray,
                 IntersectionRecord &rec) const override {
    Hit hit;
<div class="fragment highlight-current-code" data-fragment-index="1">    if (!sphere.intersect(ray, hit))
      return false;
</div><div class="fragment highlight-current-code" data-fragment-index="2">    rec = IntersectionRecord{hit, material.get()};
    return true;
</div>  }
};
</code></pre>

---

<pre><code class="cpp" data-trim data-noescape>
bool Sphere::intersect(const Ray &ray, Hit &hit) const noexcept {
  auto op = centre_ - ray.origin();
  auto b = op.dot(ray.direction());
  auto determinant = b * b - op.lengthSquared() + radius_ * radius_;
<div class="fragment highlight-current-code" data-fragment-index="1">  if (determinant &lt; 0)
    return false;
</div>  auto root = sqrt(determinant);

  // [...more maths elided for clarity...]

<div class="fragment highlight-current-code" data-fragment-index="2">  hit = Hit{t, inside, hitPosition, normal};
  return true;
</div>}
</code></pre>

---

```cpp
bool Triangle::intersect(const Ray &ray, Hit &hit) const noexcept {
  // similarly...
}
```

---

## Materials <!--- .element: class="white-bg" --->

<pre><code class="cpp" data-trim data-noescape>
class Material {
public:
  virtual ~Material() = default;

<div class="fragment highlight-current-code">  class RadianceSampler {
  public:
    virtual ~RadianceSampler() = default;

    [[nodiscard]] virtual Vec3 sample(const Ray &ray) const = 0;
  };
</div>
<div class="fragment highlight-current-code">  [[nodiscard]] virtual Vec3 sample(
        const Hit &hit, const Ray &incoming,
        const RadianceSampler &radianceSampler,
        double u, double v, double p) const = 0;
</div>};
</code></pre>

---

<pre><code class="cpp" data-trim data-noescape>
class ShinyMaterial : public Material {
  MaterialSpec mat_;
public:
  Vec3 sample(
        const Hit &hit, const Ray &incoming,
        const RadianceSampler &radianceSampler, double u,
        double v, double p) const override {
        
<div class="fragment highlight-current-code">    if (p < mat_.reflectivity) {
      return radianceSampler.sample(
          Ray(hit.position, coneSample(/*...reflection...*/, u, v)));
    }
</div>    
<div class="fragment highlight-current-code">    return mat_.diffuse
           * radianceSampler.sample(
               Ray(hit.position, hemisphereSample(hit.normal, u, v)));
</div>  }
};
</code></pre>

---

<div class="white-bg">

### Results

<img src="images/image.ex1.png" height="400" alt="A close up of several spheres with differing material types">

<div class="attribution">Scene credit: <a href="https://michaelfogleman.com">Michael Fogleman</a></div>
</div>

---

<div class="white-bg">

### Things I liked

* Code layout
* Testability
* Performance

</div>

---

<div class="white-bg">

### Things I didn't like

* Giving up `std::optional`
* (Arguable) overuse of `virtual`

</div>
