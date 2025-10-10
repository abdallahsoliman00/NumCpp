# TODO List
---

This contains, as the name suggests, a todo list for new features / bug fixes / optimizations / etc...

## TODO:
- Check for types. For example, a zeros array of type char cannot be created. [NumCpp/Core/array_creation.hpp](NumCpp/Core/array_creation.hpp)

- Add complex number compatibility.

- Add random: NumCpp/Random

- Add a cache memory function where the results of functions that are reused many times can be cached for reuse.

- Make NArray operations (multiplications, divisions, etc...) return the common type or a type that allows them to show full resolution. See this [Hadamard multiplication Implementation](NumCpp/Core/VecOps.hpp) (lines 83-98).