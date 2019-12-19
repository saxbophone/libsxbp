from math import floor


problem_size = 2 ** 10
processors = 28

problem_shares = [
    problem_size // processors + (
        1 if i in range(problem_size % processors) else 0
    )
    for i in range(processors)
]

problem_ranges = [
    range(
        floor(problem_size / processors * i),
        floor(problem_size / processors * (i + 1))
    ) for i in range(processors)
]
