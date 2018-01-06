# Public and Private API outlines
A broad overview of what the public and private APIs of the library need

## Public API

- data types/structures
    - `sxbp_buffer`, `sxbp_figure`, `sxbp_bitmap`
- functions for allocating and deallocating the library's data structures
    - `sxbp_create_buffer`, `sxbp_destroy_buffer`, `sxbp_create_figure`, etc... (naming convention to be confirmed, but keep it consistent)
- a function for converting an array of bytes into the directions of an SXBP figure, and populating an SXBP object with these directions
    - `sxbp_initialise_figure` (verb to be confirmed)
- a function for working out the lengths of all the lines of the SXBP figure, such that it makes the most efficient use of space without colliding with itself, also by working on an SXBP object and populating this object with the results
    - `sxbp_complete_figure` (verb to be confirmed)
- functions for saving and loading complete or partially-incomplete SXBP objects to and from arrays of bytes (serialisation and deseralisation)
    - `sxbp_load_figure`, `sxbp_save_figure`
- a function for rendering a complete SXBP object to a simple data object representing a bitmap image of the shape the SXBP figure makes
    - `sxbp_render_figure`
- function(s) for rendering a complete SXBP object to popular image formats (the current version of libsxbp supports PBM and PNG, the latter optionally)
    - `sxbp_render_figure_to_png` ??? (gosh that's an awful long name, to be confirmed)

## Private API

- a function for converting an array of bytes into an array of directions
- a function for setting the length of a given line of a spiral shape to a given length
- a function for evaluating if a spiral shape is currently colliding or not, with an optional index of the maximum line to check up to (or maybe instead of doing this, we treat lines with length 0 as 'not existing yet'?)
- a function for getting the current dimensions of the shape as it has been plotted so far
- a function for rasterising the spiral shape
