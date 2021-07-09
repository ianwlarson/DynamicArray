
# Dynamic Array

This is a header implementing a basic dynamic array that can be used to keep
track of objects with very low runtime costs.


# Tests

1. Basic testing of adding/freeing
2. Test of filling array, then moving it
3. Fuzz testing of adding and removing lots of elements
4. Fuzz testing like 3, but relies on asan to ensure nothing untoward happened.
5. More fuzzing
