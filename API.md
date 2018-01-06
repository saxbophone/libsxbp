# Public and Private API outlines
A broad overview of what the public and private APIs of the library need

## Public API

- functions for allocating and deallocating the library's data structures
- a function for converting an array of bytes into the directions of an SXBP figure, and populating an SXBP object with these directions
- a function for working out the lengths of all the lines of the SXBP figure, such that it makes the most efficient use of space without colliding with itself, also by working on an SXBP object and populating this object with the results
- functions for saving and loading complete or partially-incomplete SXBP objects to and from arrays of bytes (serialisation and deseralisation)
- a function for rendering a complete SXBP object to a simple data object representing a bitmap image of the shape the SXBP figure makes
- function(s) for rendering a complete SXBP object to popular image formats (the current version of libsxbp supports PBM and PNG, the latter optionally)
