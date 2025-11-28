# `attoboy::Math` User Guide

## Overview

The `attoboy::Math` class is a compact collection of **static mathematical utilities** and **constants** designed to cover the most common numeric operations needed in small utilities and tools. It provides:

* Frequently used constants such as π, e, τ, and √2.
* Random number generation (integers, floats, booleans, and list elements).
* Integer and floating-point helpers (absolute value, clamping, min/max, etc.).
* Rounding and truncation functions.
* Exponentials, logarithms, and power functions.
* Trigonometric and hyperbolic functions (using **radians**).
* Utility functions for interpolation, angle conversion, and classification of floating-point values.
* Simple 64-bit integer arithmetic helpers.

All members are:

* **Static** – you never create a `Math` object. You simply call `Math::FunctionName(...)`.
* **Stateless** – apart from the random number generator’s internal state, there is no persistent state or configuration.
* **Exception-free** – as with the rest of attoboy, functions are `noexcept` and do not throw exceptions.

### Floating-Point Numbers, Infinity, and NaN

Many of the functions in `Math` operate on `float` values. A `float` is a 32-bit floating-point number following the IEEE-754 standard. This representation has a few important properties:

* **Rounding** – most real numbers cannot be represented exactly, so results are approximate.
* **Infinity** – some operations (like overflow or division by zero) produce positive or negative infinity.
* **NaN (Not-a-Number)** – invalid operations (such as `0.0f / 0.0f`) produce a special value called NaN.

The `Math` class provides constants and helpers for working with these values:

* `Math::INF` and `Math::NEG_INF` represent positive and negative infinity.
* `Math::NAN` represents a NaN value.
* `Math::IsFinite(x)` tells you whether `x` is a finite value (not infinite or NaN).
* `Math::IsNaN(x)` tells you whether `x` is NaN.

Understanding these concepts is important when you write numeric code that must handle edge cases robustly.

### Angles in Radians vs. Degrees

Trigonometric functions such as `Sin`, `Cos`, and `Tan` expect **angles in radians**, not degrees. Radians are another way of measuring angles:

* A full circle is `2π` radians (`Math::TAU`).
* 90 degrees is `π/2` radians.
* 180 degrees is `π` radians.

The `Math` class provides:

* `Math::DegToRad(degrees)` – convert degrees to radians.
* `Math::RadToDeg(radians)` – convert radians to degrees.

Use these helpers when your code works with angles expressed in degrees (such as user input) but the math functions expect radians.

---

## Reference

All members of `Math` are static and accessed as `Math::Name(...)` or `Math::CONSTANT`. The sections below group related members for clarity.

> **Note:** All examples assume `using namespace attoboy;`.

---

### Mathematical Constants

#### `Math::PI`

**Signature**

```cpp
static constexpr float PI = 3.14159265358979323846f;
```

**Synopsis**
Pi (π ≈ 3.14159).

**Parameters**

* *(none)*

**Return value**

* *(constant; not applicable)*

**In Depth**

`Math::PI` represents the mathematical constant π, the ratio of a circle’s circumference to its diameter. It is used in almost any trigonometric or circular calculation.

Because it is declared `constexpr`, it can be used in compile-time expressions.

**Example**

```cpp
float radius = 2.0f;
float circumference = 2.0f * Math::PI * radius;  // ≈ 12.56637
```

*This example computes the circumference of a circle from its radius using π.*

---

#### `Math::E`

**Signature**

```cpp
static constexpr float E = 2.71828182845904523536f;
```

**Synopsis**
Euler's number (e ≈ 2.71828).

**Parameters**

* *(none)*

**Return value**

* *(constant; not applicable)*

**In Depth**

`Math::E` is Euler’s number, the base of the natural logarithm. It appears in exponential growth/decay equations, probability theory, and many other areas of mathematics.

**Example**

```cpp
float x = 1.0f;
float expApprox = Math::Pow(Math::E, x);  // ≈ e^1 = 2.71828
```

*This example uses `Math::E` with `Math::Pow` to approximate e¹.*

---

#### `Math::TAU`

**Signature**

```cpp
static constexpr float TAU = 6.28318530717958647693f;
```

**Synopsis**
Tau (τ = 2π ≈ 6.28318).

**Parameters**

* *(none)*

**Return value**

* *(constant; not applicable)*

**In Depth**

`Math::TAU` is defined as `2 * π`. Some formulas are simpler when expressed in terms of τ rather than π, especially when dealing with full rotations.

**Example**

```cpp
float rotations = 0.25f;  // quarter-turn
float angle = rotations * Math::TAU;  // radians
```

*This example converts a fraction of a full rotation into radians using τ.*

---

#### `Math::SQRT_2`

**Signature**

```cpp
static constexpr float SQRT_2 = 1.41421356237309504880f;
```

**Synopsis**
Square root of 2 (√2 ≈ 1.41421).

**Parameters**

* *(none)*

**Return value**

* *(constant; not applicable)*

**In Depth**

`Math::SQRT_2` is the square root of 2, the length of the diagonal of a unit square. It is useful in geometry and normalization computations.

**Example**

```cpp
float diagonal = Math::SQRT_2;  // diagonal length of a 1x1 square
```

*This example uses √2 for a simple geometric calculation.*

---

#### `Math::INF`

**Signature**

```cpp
static const float INF;
```

**Synopsis**
Positive infinity.

**Parameters**

* *(none)*

**Return value**

* *(constant; not applicable)*

**In Depth**

`Math::INF` represents positive infinity. It can be used as a sentinel value or initialized as a “very large” placeholder when searching for minimums.

Be aware that arithmetic with infinity follows floating-point rules:

* `INF + 1` is still `INF`.
* Comparisons such as `x < Math::INF` work as expected for finite `x`.

**Example**

```cpp
float best = Math::INF;
// ... update best with smaller values over time
```

*This example initializes a “best” score to positive infinity so any real score will be smaller.*

---

#### `Math::NEG_INF`

**Signature**

```cpp
static const float NEG_INF;
```

**Synopsis**
Negative infinity.

**Parameters**

* *(none)*

**Return value**

* *(constant; not applicable)*

**In Depth**

`Math::NEG_INF` is the negative counterpart of `Math::INF`. It is useful when searching for maximums or representing an underflowed quantity.

**Example**

```cpp
float best = Math::NEG_INF;
// ... update best with larger values over time
```

*This example initializes a maximum tracker to negative infinity.*

---

#### `Math::NAN`

**Signature**

```cpp
static const float NAN;
```

**Synopsis**
Not-a-number.

**Parameters**

* *(none)*

**Return value**

* *(constant; not applicable)*

**In Depth**

`Math::NAN` represents a NaN (Not-a-Number) value. NaN is used to represent undefined or unrepresentable results (such as `0.0f / 0.0f`).

Important behavior:

* NaN is **not equal** to anything, including itself. Comparisons like `x == Math::NAN` are always false.
* Use `Math::IsNaN(x)` to test for NaN.

**Example**

```cpp
float value = Math::NAN;
// later:
if (Math::IsNaN(value)) {
  // handle invalid result
}
```

*This example uses NaN as a sentinel for “invalid value” and detects it using `IsNaN`.*

---

### Random Number Generation

#### `static int Random() noexcept`

**Signature**

```cpp
static int Random() noexcept;
```

**Synopsis**
Returns a random integer.

**Parameters**

* *(none)*

**Return value**

* A pseudo-random integer value. The exact range depends on the implementation, but it typically spans the full range of `int`.

**In Depth**

`Random()` provides a non-deterministic integer suitable for general use when you do not need a specific range. If you need a bounded range, prefer `RandomRange`.

Because the function is `noexcept` and uses internal state, it is safe and easy to call whenever you need a quick random value.

**Example**

```cpp
int raw = Math::Random();
// raw can be used as a seed or combined with modulo for ad hoc ranges
```

*This example obtains an unbounded random integer.*

---

#### `static float RandomFloat() noexcept`

**Signature**

```cpp
static float RandomFloat() noexcept;
```

**Synopsis**
Returns a random float in [0.0, 1.0).

**Parameters**

* *(none)*

**Return value**

* Pseudo-random `float` in the half-open interval `[0.0f, 1.0f)`.

**In Depth**

`RandomFloat()` is ideal for probability checks, interpolation, or simple noise:

* Includes `0.0f`.
* Excludes `1.0f`.

You can scale and shift the result to other intervals:

```cpp
float value = Math::RandomFloat();          // [0, 1)
float scaled = 10.0f * value;               // [0, 10)
float shifted = 5.0f + 10.0f * value;       // [5, 15)
```

**Example**

```cpp
float chance = Math::RandomFloat();
bool success = chance < 0.25f;  // 25% chance
```

*This example uses a random float to simulate a simple probability check.*

---

#### `static int RandomRange(int start, int end) noexcept`

**Signature**

```cpp
static int RandomRange(int start, int end) noexcept;
```

**Synopsis**
Returns a random integer in [start, end).

**Parameters**

* `start` – Inclusive lower bound of the range.
* `end` – Exclusive upper bound of the range (must be greater than `start`).

**Return value**

* Random integer `n` such that `start <= n < end`.

**In Depth**

`RandomRange()` is the primary way to obtain bounded random integers. It returns integers uniformly distributed in the half-open interval `[start, end)`.

You are responsible for ensuring `end > start`. Passing invalid bounds (for example, `end <= start`) may result in undefined or implementation-specific behavior.

**Example**

```cpp
int roll = Math::RandomRange(1, 7);  // 1..6 inclusive, like a die
```

*This example simulates a six-sided die roll.*

---

#### `static bool RandomBool() noexcept`

**Signature**

```cpp
static bool RandomBool() noexcept;
```

**Synopsis**
Returns a random boolean.

**Parameters**

* *(none)*

**Return value**

* `true` or `false`, approximately with 50% probability each.

**In Depth**

`RandomBool()` is a convenience wrapper around the random number generator for coin-flip style decisions. It is equivalent to `Math::RandomFloat() < 0.5f` but may be more efficient.

**Example**

```cpp
bool heads = Math::RandomBool();
```

*This example simulates a fair coin toss.*

---

#### `template <typename T> static T RandomChoice(const List &list) noexcept`

**Signature**

```cpp
template <typename T> static T RandomChoice(const List &list) noexcept;
```

**Synopsis**
Returns a random element from the list.

**Parameters**

* `list` – `List` of values to choose from. Must not be empty for meaningful results.

**Return value**

* A random element of type `T` from `list`, or `T()` if the list is empty.

**In Depth**

`RandomChoice<T>()` selects a uniform random index in the range `[0, list.length())` and returns the element at that index, cast to `T` using `list.at<T>(index)`.

If the list is empty, the function returns a default-constructed `T`. In many cases, that means `0` for numeric types or an empty `String`.

You are responsible for calling it with a list whose elements can be converted to `T`.

**Example**

```cpp
List options;
options.append("red").append("green").append("blue");

String color = Math::RandomChoice<String>(options);
// color is one of "red", "green", or "blue"
```

*This example selects a random color from a list of strings.*

---

### Basic Integer and Floating-Point Helpers

#### `static int Abs(int x) noexcept`

**Signature**

```cpp
static int Abs(int x) noexcept;
```

**Synopsis**
Returns the absolute value of x.

**Parameters**

* `x` – Integer whose absolute value is required.

**Return value**

* Non-negative integer equal to `|x|`.

**In Depth**

`Abs(int)` removes the sign from an integer, returning a non-negative value. For most inputs, `Abs(-n)` is the same as `-n`. The behavior at the minimum representable integer (for example, INT_MIN) can be implementation-defined due to overflow, so avoid calling `Abs()` with that value.

**Example**

```cpp
int delta = -5;
int distance = Math::Abs(delta);  // 5
```

*This example converts a signed difference into a non-negative distance.*

---

#### `static float Abs(float x) noexcept`

**Signature**

```cpp
static float Abs(float x) noexcept;
```

**Synopsis**
Returns the absolute value of x.

**Parameters**

* `x` – Floating-point value.

**Return value**

* `|x|` as a `float`. For NaN, the result is typically NaN.

**In Depth**

`Abs(float)` removes the sign bit of the float, yielding a non-negative value. It behaves as expected for finite values and infinities.

**Example**

```cpp
float error = -0.25f;
float magnitude = Math::Abs(error);  // 0.25f
```

*This example measures the magnitude of a signed error.*

---

#### `static int Min(int a, int b) noexcept`

**Signature**

```cpp
static int Min(int a, int b) noexcept;
```

**Synopsis**
Returns the smaller of a and b.

**Parameters**

* `a` – First integer.
* `b` – Second integer.

**Return value**

* The smaller of `a` and `b`. If equal, returns either.

**In Depth**

`Min(int, int)` is a simple helper for choosing the lower of two integers.

**Example**

```cpp
int width = Math::Min(80, 120);  // 80
```

*This example picks the smaller of two bounds.*

---

#### `static float Min(float a, float b) noexcept`

**Signature**

```cpp
static float Min(float a, float b) noexcept;
```

**Synopsis**
Returns the smaller of a and b.

**Parameters**

* `a` – First float.
* `b` – Second float.

**Return value**

* The smaller of `a` and `b`. Behavior with NaN depends on the implementation.

**In Depth**

For normal finite values, `Min(float, float)` behaves as expected. If either argument is NaN, the result may be implementation-defined; avoid using NaN as input when possible.

**Example**

```cpp
float alpha = Math::Min(1.0f, 0.5f);  // 0.5f
```

*This example clamps an opacity value by choosing the smaller of two floats.*

---

#### `static int Max(int a, int b) noexcept`

**Signature**

```cpp
static int Max(int a, int b) noexcept;
```

**Synopsis**
Returns the larger of a and b.

**Parameters**

* `a` – First integer.
* `b` – Second integer.

**Return value**

* The larger of `a` and `b`.

**In Depth**

`Max(int, int)` is the counterpart to `Min(int, int)`.

**Example**

```cpp
int height = Math::Max(10, 20);  // 20
```

*This example picks the larger of two integers.*

---

#### `static float Max(float a, float b) noexcept`

**Signature**

```cpp
static float Max(float a, float b) noexcept;
```

**Synopsis**
Returns the larger of a and b.

**Parameters**

* `a` – First float.
* `b` – Second float.

**Return value**

* The larger of `a` and `b`.

**In Depth**

`Max(float, float)` behaves as expected for finite inputs. When NaN is involved, the result is implementation-defined; avoid mixing NaN with valid values unless you handle it explicitly.

**Example**

```cpp
float limit = Math::Max(0.0f, -5.0f);  // 0.0f
```

*This example ensures a value is at least zero by choosing the larger of two floats.*

---

#### `static int Clamp(int x, int minVal, int maxVal) noexcept`

**Signature**

```cpp
static int Clamp(int x, int minVal, int maxVal) noexcept;
```

**Synopsis**
Returns x clamped to [minVal, maxVal].

**Parameters**

* `x` – Input integer.
* `minVal` – Lower bound of the allowed range.
* `maxVal` – Upper bound of the allowed range (should be ≥ `minVal`).

**Return value**

* `x` if `minVal <= x <= maxVal`.
* `minVal` if `x < minVal`.
* `maxVal` if `x > maxVal`.

**In Depth**

Clamping is useful whenever you need to restrict a value to a closed interval.

You should ensure `minVal <= maxVal`; if not, the behavior is implementation-defined.

**Example**

```cpp
int brightness = 150;
brightness = Math::Clamp(brightness, 0, 100);  // 100
```

*This example caps an integer brightness value within 0–100.*

---

#### `static float Clamp(float x, float minVal, float maxVal) noexcept`

**Signature**

```cpp
static float Clamp(float x, float minVal, float maxVal) noexcept;
```

**Synopsis**
Returns x clamped to [minVal, maxVal].

**Parameters**

* `x` – Input float.
* `minVal` – Lower bound.
* `maxVal` – Upper bound.

**Return value**

* `x` clamped to `[minVal, maxVal]` as for the integer version.

**In Depth**

Floating-point clamping is often used for normalized values (for example, 0.0 to 1.0) or limiting outputs to physically meaningful ranges.

As with the integer version, ensure `minVal <= maxVal`.

**Example**

```cpp
float t = 1.5f;
t = Math::Clamp(t, 0.0f, 1.0f);  // 1.0f
```

*This example clamps a parameter `t` into a normalized range.*

---

#### `static int Sign(int x) noexcept`

**Signature**

```cpp
static int Sign(int x) noexcept;
```

**Synopsis**
Returns -1, 0, or 1 based on the sign of x.

**Parameters**

* `x` – Integer input.

**Return value**

* `-1` if `x < 0`.
* `0` if `x == 0`.
* `1` if `x > 0`.

**In Depth**

`Sign(int)` is a compact way to distinguish negative, zero, and positive integers.

**Example**

```cpp
int dx = -3;
int direction = Math::Sign(dx);  // -1
```

*This example converts a signed offset into a direction indicator.*

---

#### `static float Sign(float x) noexcept`

**Signature**

```cpp
static float Sign(float x) noexcept;
```

**Synopsis**
Returns -1.0, 0.0, or 1.0 based on the sign of x.

**Parameters**

* `x` – Floating-point input.

**Return value**

* `-1.0f` if `x < 0`.
* `0.0f` if `x == 0`.
* `1.0f` if `x > 0`.

**In Depth**

`Sign(float)` is useful in vector math and normalization. Behavior for NaN is implementation-defined and should be avoided as an input if possible.

**Example**

```cpp
float value = 2.5f;
float s = Math::Sign(value);  // 1.0f
```

*This example inspects the sign of a floating-point value.*

---

#### `static bool IsEven(int x) noexcept`

**Signature**

```cpp
static bool IsEven(int x) noexcept;
```

**Synopsis**
Returns true if x is even.

**Parameters**

* `x` – Integer input.

**Return value**

* `true` if `x` is divisible by 2; `false` otherwise.

**In Depth**

`IsEven()` determines whether the least significant bit of `x` is zero. It works for negative integers as well.

**Example**

```cpp
int n = 4;
bool even = Math::IsEven(n);  // true
```

*This example checks whether an integer is even.*

---

#### `static bool IsOdd(int x) noexcept`

**Signature**

```cpp
static bool IsOdd(int x) noexcept;
```

**Synopsis**
Returns true if x is odd.

**Parameters**

* `x` – Integer input.

**Return value**

* `true` if `x` is not divisible by 2; `false` otherwise.

**In Depth**

`IsOdd()` is effectively the negation of `IsEven()` for integer values.

**Example**

```cpp
int n = 5;
bool odd = Math::IsOdd(n);  // true
```

*This example checks whether an integer is odd.*

---

#### `static bool IsPowerOfTwo(int x) noexcept`

**Signature**

```cpp
static bool IsPowerOfTwo(int x) noexcept;
```

**Synopsis**
Returns true if x is a power of two.

**Parameters**

* `x` – Integer input (typically positive).

**Return value**

* `true` if `x` is of the form `2^n` for some non-negative integer `n`.
* `false` for zero, negative values, or non-powers-of-two.

**In Depth**

This helper is useful when working with buffer sizes, alignment, or other low-level quantities that must be powers of two.

**Example**

```cpp
int size = 64;
bool ok = Math::IsPowerOfTwo(size);  // true
```

*This example checks whether a size is a power-of-two, which is often required for certain algorithms.*

---

### Rounding and Truncation

#### `static float Floor(float x) noexcept`

**Signature**

```cpp
static float Floor(float x) noexcept;
```

**Synopsis**
Returns the largest integer ≤ x.

**Parameters**

* `x` – Floating-point input.

**Return value**

* Largest floating-point value that is an integer and not greater than `x`.

**In Depth**

`Floor()` rounds toward **negative infinity**:

* `Floor(2.8f)` → `2.0f`
* `Floor(-2.1f)` → `-3.0f`

**Example**

```cpp
float x = 3.7f;
float r = Math::Floor(x);  // 3.0f
```

*This example rounds a float down to the nearest integer.*

---

#### `static float Ceil(float x) noexcept`

**Signature**

```cpp
static float Ceil(float x) noexcept;
```

**Synopsis**
Returns the smallest integer ≥ x.

**Parameters**

* `x` – Floating-point input.

**Return value**

* Smallest floating-point value that is an integer and not less than `x`.

**In Depth**

`Ceil()` rounds toward **positive infinity**:

* `Ceil(2.1f)` → `3.0f`
* `Ceil(-2.9f)` → `-2.0f`

**Example**

```cpp
float x = 3.1f;
float r = Math::Ceil(x);  // 4.0f
```

*This example rounds a float up to the nearest integer.*

---

#### `static float Trunc(float x) noexcept`

**Signature**

```cpp
static float Trunc(float x) noexcept;
```

**Synopsis**
Returns x with fractional part removed (toward zero).

**Parameters**

* `x` – Floating-point input.

**Return value**

* `x` with its fractional part removed, rounding toward zero.

**In Depth**

`Trunc()` behaves like casting to an integer and then back to `float`:

* `Trunc(2.8f)` → `2.0f`
* `Trunc(-2.8f)` → `-2.0f`

**Example**

```cpp
float x = -3.9f;
float r = Math::Trunc(x);  // -3.0f
```

*This example removes the fractional part of a negative value by truncating toward zero.*

---

#### `static float Round(float x) noexcept`

**Signature**

```cpp
static float Round(float x) noexcept;
```

**Synopsis**
Returns x rounded to the nearest integer.

**Parameters**

* `x` – Floating-point input.

**Return value**

* `x` rounded to the nearest integer value as a `float`.

**In Depth**

`Round()` rounds to the nearest integer. The behavior for `.5` ties (for example, `2.5f`) is implementation-dependent; it may round away from zero or to the nearest even integer.

**Example**

```cpp
float x = 2.6f;
float r = Math::Round(x);  // 3.0f
```

*This example rounds a value to the nearest integer.*

---

### Exponentials, Logarithms, and Powers

#### `static float Exp2(float x) noexcept`

**Signature**

```cpp
static float Exp2(float x) noexcept;
```

**Synopsis**
Returns 2^x.

**Parameters**

* `x` – Exponent.

**Return value**

* `2^x` as a `float`.

**In Depth**

`Exp2()` is a convenience for powers of two, commonly used in binary scaling and signal processing.

**Example**

```cpp
float x = 3.0f;
float value = Math::Exp2(x);  // 8.0f
```

*This example computes 2³ using `Exp2`.*

---

#### `static float Log2(float x) noexcept`

**Signature**

```cpp
static float Log2(float x) noexcept;
```

**Synopsis**
Returns log₂(x).

**Parameters**

* `x` – Positive floating-point value.

**Return value**

* Base-2 logarithm of `x`. For `x <= 0`, the result may be NaN or implementation-defined.

**In Depth**

`Log2()` is useful for computing the number of bits required to represent a value, or converting between exponent and scale factors.

**Example**

```cpp
float x = 8.0f;
float bits = Math::Log2(x);  // 3.0f
```

*This example computes the base-2 logarithm of 8.*

---

#### `static float Ln(float x) noexcept`

**Signature**

```cpp
static float Ln(float x) noexcept;
```

**Synopsis**
Returns the natural logarithm ln(x).

**Parameters**

* `x` – Positive floating-point value.

**Return value**

* `ln(x)` as a `float`.

**In Depth**

`Ln()` provides the natural logarithm, the inverse of `Exp()` when using base `e`. For `x <= 0`, the result is typically NaN or negative infinity, depending on the value and implementation.

**Example**

```cpp
float x = Math::E;
float value = Math::Ln(x);  // ≈ 1.0f
```

*This example checks that the natural log of e is approximately 1.*

---

#### `static float Log10(float x) noexcept`

**Signature**

```cpp
static float Log10(float x) noexcept;
```

**Synopsis**
Returns log₁₀(x).

**Parameters**

* `x` – Positive floating-point value.

**Return value**

* Base-10 logarithm of `x`.

**In Depth**

`Log10()` is commonly used for decibels, orders of magnitude, and scientific notation.

**Example**

```cpp
float x = 1000.0f;
float value = Math::Log10(x);  // 3.0f
```

*This example computes the base-10 logarithm of 1000.*

---

#### `static float Exp(float x) noexcept`

**Signature**

```cpp
static float Exp(float x) noexcept;
```

**Synopsis**
Returns e^x.

**Parameters**

* `x` – Exponent.

**Return value**

* `e^x` as a `float`.

**In Depth**

`Exp()` is the exponential function with base `Math::E`. It is the inverse of `Ln()`.

**Example**

```cpp
float x = 1.0f;
float value = Math::Exp(x);  // ≈ 2.71828
```

*This example computes e¹.*

---

#### `static float Pow(float x, float y) noexcept`

**Signature**

```cpp
static float Pow(float x, float y) noexcept;
```

**Synopsis**
Returns x^y.

**Parameters**

* `x` – Base value.
* `y` – Exponent.

**Return value**

* `x` raised to the power `y`.

**In Depth**

`Pow()` is the general power function. For special cases (like powers of two or squares), more specialized functions may be more efficient, but `Pow()` is the most flexible.

Some combinations of `x` and `y` may result in NaN (for example, negative base with non-integer exponent).

**Example**

```cpp
float base = 2.0f;
float exp = 4.0f;
float value = Math::Pow(base, exp);  // 16.0f
```

*This example raises 2 to the 4th power.*

---

#### `static int Pow2(int x) noexcept`

**Signature**

```cpp
static int Pow2(int x) noexcept;
```

**Synopsis**
Returns 2^x (integer version).

**Parameters**

* `x` – Exponent (integer).

**Return value**

* Integer equal to `2^x`, if it fits in `int`.

**In Depth**

`Pow2(int)` is typically implemented as a bit shift, making it efficient for integer powers of two. If `x` is too large, the result may overflow; you are responsible for ensuring `x` is within a safe range.

**Example**

```cpp
int bits = 5;
int value = Math::Pow2(bits);  // 32
```

*This example computes 2⁵ as an integer.*

---

### Square Root and Trigonometric Functions

#### `static float Sqrt(float x) noexcept`

**Signature**

```cpp
static float Sqrt(float x) noexcept;
```

**Synopsis**
Returns the square root of x.

**Parameters**

* `x` – Non-negative float.

**Return value**

* Square root of `x`. For negative `x`, result is typically NaN.

**In Depth**

`Sqrt()` is a standard square root. For negative inputs, the result is not defined in the real numbers; expect NaN in such cases.

**Example**

```cpp
float area = 9.0f;
float side = Math::Sqrt(area);  // 3.0f
```

*This example computes the side length of a square given its area.*

---

#### `static float Sin(float x) noexcept`

**Signature**

```cpp
static float Sin(float x) noexcept;
```

**Synopsis**
Returns the sine of x (radians).

**Parameters**

* `x` – Angle in radians.

**Return value**

* Sine of `x`.

**In Depth**

`Sin()` uses radians, not degrees. Convert degrees to radians with `DegToRad()` if necessary.

**Example**

```cpp
float deg = 90.0f;
float rad = Math::DegToRad(deg);
float s = Math::Sin(rad);  // ≈ 1.0f
```

*This example computes the sine of 90 degrees by converting to radians.*

---

#### `static float Cos(float x) noexcept`

**Signature**

```cpp
static float Cos(float x) noexcept;
```

**Synopsis**
Returns the cosine of x (radians).

**Parameters**

* `x` – Angle in radians.

**Return value**

* Cosine of `x`.

**In Depth**

`Cos()` is the cosine function, also expecting radians. Use it with `Sin()` for basic vector and rotation math.

**Example**

```cpp
float deg = 0.0f;
float rad = Math::DegToRad(deg);
float c = Math::Cos(rad);  // 1.0f
```

*This example computes the cosine of 0 degrees.*

---

#### `static float Tan(float x) noexcept`

**Signature**

```cpp
static float Tan(float x) noexcept;
```

**Synopsis**
Returns the tangent of x (radians).

**Parameters**

* `x` – Angle in radians.

**Return value**

* Tangent of `x`. May approach infinity near odd multiples of π/2.

**In Depth**

`Tan()` should be used with care near points where tangent is undefined (`π/2`, `3π/2`, etc.), as the result can grow extremely large and may overflow or become inaccurate.

**Example**

```cpp
float deg = 45.0f;
float rad = Math::DegToRad(deg);
float t = Math::Tan(rad);  // ≈ 1.0f
```

*This example computes the tangent of 45 degrees.*

---

#### `static float Atan2(float y, float x) noexcept`

**Signature**

```cpp
static float Atan2(float y, float x) noexcept;
```

**Synopsis**
Returns the arctangent of y/x (radians), using signs for quadrant.

**Parameters**

* `y` – Y component.
* `x` – X component.

**Return value**

* Angle in radians between the positive X-axis and the point `(x, y)`, typically in `(-π, π]`.

**In Depth**

`Atan2()` is preferred over `Atan()` when you have both X and Y components, because it correctly identifies the quadrant of the result. It is widely used to convert Cartesian coordinates to polar coordinates.

**Example**

```cpp
float x = 0.0f;
float y = 1.0f;
float angle = Math::Atan2(y, x);  // ≈ π/2
```

*This example computes the angle of a vector pointing straight up.*

---

#### `static float Atan(float x) noexcept`

**Signature**

```cpp
static float Atan(float x) noexcept;
```

**Synopsis**
Returns the arctangent of x (radians).

**Parameters**

* `x` – Input value.

**Return value**

* `atan(x)` in radians, typically in `(-π/2, π/2)`.

**In Depth**

`Atan()` returns the angle whose tangent is `x`. If you have separate X and Y components, prefer `Atan2()` to handle quadrants correctly.

**Example**

```cpp
float slope = 1.0f;
float angle = Math::Atan(slope);  // ≈ π/4
```

*This example converts a slope of 1 into an angle in radians.*

---

#### `static float Asin(float x) noexcept`

**Signature**

```cpp
static float Asin(float x) noexcept;
```

**Synopsis**
Returns the arcsine of x (radians).

**Parameters**

* `x` – Input value in the range [-1, 1].

**Return value**

* Angle in radians whose sine is `x`. For values outside [-1, 1], result is typically NaN.

**In Depth**

`Asin()` is useful for recovering angles from sine values, for example in triangle computations.

**Example**

```cpp
float v = 0.5f;
float angle = Math::Asin(v);  // ≈ 0.5236 (30 degrees)
```

*This example computes the angle whose sine is 0.5.*

---

#### `static float Cosh(float x) noexcept`

**Signature**

```cpp
static float Cosh(float x) noexcept;
```

**Synopsis**
Returns the hyperbolic cosine of x.

**Parameters**

* `x` – Input value.

**Return value**

* Hyperbolic cosine of `x`.

**In Depth**

`Cosh()` is the hyperbolic analogue of cosine, defined in terms of exponentials. It is less commonly used than `Sin`, `Cos`, and `Tan`, but is useful in some advanced mathematical models.

**Example**

```cpp
float x = 1.0f;
float value = Math::Cosh(x);
```

*This example computes the hyperbolic cosine of 1.*

---

### Miscellaneous Float Utilities

#### `static float Mod(float x, float y) noexcept`

**Signature**

```cpp
static float Mod(float x, float y) noexcept;
```

**Synopsis**
Returns the floating-point remainder of x/y.

**Parameters**

* `x` – Dividend.
* `y` – Divisor (should not be zero).

**Return value**

* Remainder of `x / y` as a `float`.

**In Depth**

`Mod()` behaves similarly to `x - y * floor(x / y)` or related floating-point remainder functions. If `y` is zero, the result is undefined; you must avoid dividing by zero.

**Example**

```cpp
float angle = Math::Mod(370.0f, 360.0f);  // 10.0f
```

*This example wraps an angle into a 0–360 degree range using the floating-point remainder.*

---

#### `static float Lerp(float a, float b, float t) noexcept`

**Signature**

```cpp
static float Lerp(float a, float b, float t) noexcept;
```

**Synopsis**
Returns linear interpolation between a and b by t.

**Parameters**

* `a` – Start value.
* `b` – End value.
* `t` – Interpolation parameter, typically in `[0.0f, 1.0f]`.

**Return value**

* `(1 - t) * a + t * b` as a `float`.

**In Depth**

`Lerp()` is widely used in animations and smooth transitions:

* `t = 0` → returns `a`.
* `t = 1` → returns `b`.
* `t` between 0 and 1 → blends between `a` and `b`.

Values of `t` outside `[0, 1]` extrapolate beyond the endpoints.

**Example**

```cpp
float start = 0.0f;
float end = 10.0f;
float halfway = Math::Lerp(start, end, 0.5f);  // 5.0f
```

*This example computes the midpoint between two values.*

---

#### `static float Step(float edge, float x) noexcept`

**Signature**

```cpp
static float Step(float edge, float x) noexcept;
```

**Synopsis**
Returns 0 if x < edge, otherwise 1.

**Parameters**

* `edge` – Threshold value.
* `x` – Input value.

**Return value**

* `0.0f` if `x < edge`.
* `1.0f` if `x >= edge`.

**In Depth**

`Step()` implements a simple step function often used for thresholding:

* Anything below `edge` maps to zero.
* Anything at or above `edge` maps to one.

It is useful for quick gating logic in numeric computations.

**Example**

```cpp
float edge = 0.5f;
float x = 0.7f;
float result = Math::Step(edge, x);  // 1.0f
```

*This example converts a continuous value into a binary output based on a threshold.*

---

#### `static float DegToRad(float degrees) noexcept`

**Signature**

```cpp
static float DegToRad(float degrees) noexcept;
```

**Synopsis**
Converts degrees to radians.

**Parameters**

* `degrees` – Angle in degrees.

**Return value**

* Equivalent angle in radians.

**In Depth**

Use `DegToRad()` whenever you receive angles in degrees (for example, user input or file data) but need to call trigonometric functions that expect radians.

The conversion is `degrees * (π / 180)`.

**Example**

```cpp
float degrees = 180.0f;
float radians = Math::DegToRad(degrees);  // ≈ π
```

*This example converts 180 degrees to radians.*

---

#### `static float RadToDeg(float radians) noexcept`

**Signature**

```cpp
static float RadToDeg(float radians) noexcept;
```

**Synopsis**
Converts radians to degrees.

**Parameters**

* `radians` – Angle in radians.

**Return value**

* Equivalent angle in degrees.

**In Depth**

`RadToDeg()` is the inverse of `DegToRad()`, converting from radians back to degrees, which are often more intuitive for display.

**Example**

```cpp
float angle = Math::PI;
float degrees = Math::RadToDeg(angle);  // ≈ 180.0f
```

*This example converts π radians to degrees.*

---

#### `static bool IsFinite(float x) noexcept`

**Signature**

```cpp
static bool IsFinite(float x) noexcept;
```

**Synopsis**
Returns true if x is finite (not infinity or NaN).

**Parameters**

* `x` – Floating-point value.

**Return value**

* `true` if `x` is neither infinite nor NaN; `false` otherwise.

**In Depth**

`IsFinite()` is crucial when validating results from computations that may overflow or produce NaN. It distinguishes ordinary numbers from special cases.

**Example**

```cpp
float value = Math::INF;
bool ok = Math::IsFinite(value);  // false
```

*This example shows that positive infinity is not considered finite.*

---

#### `static bool IsNaN(float x) noexcept`

**Signature**

```cpp
static bool IsNaN(float x) noexcept;
```

**Synopsis**
Returns true if x is NaN.

**Parameters**

* `x` – Floating-point value.

**Return value**

* `true` if `x` is a NaN value; `false` otherwise.

**In Depth**

You must use `IsNaN()` to test for NaN, because comparisons like `x == Math::NAN` are always false.

Use it whenever you suspect that a computation might have produced an invalid result.

**Example**

```cpp
float value = Math::NAN;
bool bad = Math::IsNaN(value);  // true
```

*This example detects a NaN value using `IsNaN`.*

---

### 64-bit Integer Arithmetic

#### `static long long Add64(long long a, long long b) noexcept`

**Signature**

```cpp
static long long Add64(long long a, long long b) noexcept;
```

**Synopsis**
Returns the sum of a + b (64-bit integers).

**Parameters**

* `a` – First operand.
* `b` – Second operand.

**Return value**

* `a + b` as a 64-bit integer.

**In Depth**

`Add64()` adds two 64-bit integers. If the result overflows the range of `long long`, the behavior is implementation-defined, as with native integer overflow in C++.

**Example**

```cpp
long long a = 1000000000LL;
long long b = 2000000000LL;
long long sum = Math::Add64(a, b);
```

*This example adds two large 64-bit integers.*

---

#### `static long long Sub64(long long a, long long b) noexcept`

**Signature**

```cpp
static long long Sub64(long long a, long long b) noexcept;
```

**Synopsis**
Returns the difference of a - b (64-bit integers).

**Parameters**

* `a` – Minuend.
* `b` – Subtrahend.

**Return value**

* `a - b` as a 64-bit integer.

**In Depth**

`Sub64()` subtracts one 64-bit integer from another. As with addition, overflow is implementation-defined.

**Example**

```cpp
long long total = 5000LL;
long long used = 1200LL;
long long remaining = Math::Sub64(total, used);  // 3800
```

*This example subtracts one 64-bit value from another to compute a remaining amount.*

---

#### `static long long Div64(long long a, long long b) noexcept`

**Signature**

```cpp
static long long Div64(long long a, long long b) noexcept;
```

**Synopsis**
Returns the quotient of a / b (64-bit integers).

**Parameters**

* `a` – Dividend.
* `b` – Divisor (must not be zero).

**Return value**

* Quotient `a / b` as a 64-bit integer, using integer division (fractional part discarded).

**In Depth**

`Div64()` divides two 64-bit integers. Division by zero is undefined and must be avoided. The result is truncated toward zero.

**Example**

```cpp
long long bytes = 4096LL;
long long blockSize = 512LL;
long long blocks = Math::Div64(bytes, blockSize);  // 8
```

*This example computes how many whole blocks fit into a byte count.*

---

#### `static long long Mod64(long long a, long long b) noexcept`

**Signature**

```cpp
static long long Mod64(long long a, long long b) noexcept;
```

**Synopsis**
Returns the remainder of a / b (64-bit integers).

**Parameters**

* `a` – Dividend.
* `b` – Divisor (must not be zero).

**Return value**

* Remainder of integer division `a / b`.

**In Depth**

`Mod64()` complements `Div64()` by returning the remainder. As with any integer modulus operation, dividing by zero is undefined.

**Example**

```cpp
long long bytes = 4100LL;
long long blockSize = 512LL;
long long remainder = Math::Mod64(bytes, blockSize);  // 4
```

*This example computes the leftover bytes when dividing into fixed-size blocks.*
