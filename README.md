
# Dynamic Array

This is a header implementing a basic dynamic array that can be used to keep
track of objects with very low runtime costs.


# Tests

1. Basic testing of adding/freeing
2. Test of filling array, then moving it
3. Fuzz testing of adding and removing lots of elements
4. Fuzz testing like 3, but relies on asan to ensure nothing untoward happened.
5. More fuzzing
6. Test of trying to shrink the array when late indices are in use.
7. Test of shrinking the array when the late indices are not in use (success)
8. Test moving an array with many free elements
9. Test resizing an array to size 0
10. Repeatedly filling and emptying the array
11. Simple case for shrinking an array (when the difference in size is unused).
12. Repeatedly fill and resize the array with random insertions and deletions

