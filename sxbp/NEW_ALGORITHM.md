# New Algorithm Idea

- Data is encoded in the length of different line segments instead, i.e. 0 = "short line", 1 = "long line" (probably, "short" will be line length 1 and "long" line length 2)
- The direction changes of the line are meaningless. The only requirement is that the path traced by the line makes a 90° turn between each line segment pair, in order to make sure individual lines can be distinguised.
- A brilliant side-effect of choosing this representation is that for a barcode that is to store n number of bits, the "solution" to it (i.e. the line direction turns, left or right) can be represented with n number of bits too. Therefore, for a problem of size n, there are 2^n potential solutions (but some, many or almost all of these might produce invalid, colliding results).

# Experiments to run before proper implementation
- Because the problem and candidate solution representations are finite this time, for all sizes of n from 1 to a reasonable number (maybe 24 or 32), all possible solutions can be computed and evaluated for validity. By doing this across all possible problems for each size of problem within a finite range, I hope to be able to compute the curve of the proportion of valid solutions of the search space across all problems of a given size. My expectation is that the proportion of valid solutions in the search space will decrease as the problem size increases, but it will be useful if the function of that graph can be found or estimated. This will be useful for making time estimates for how long it may take to solve an n-sized problem when working on much larger values of n.