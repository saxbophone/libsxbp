from math import ceil, floor


arrays = [
    [3, 5],
    [3, 6, 9, 7],
    [6, 9, 2, 7, 5, 2, 1, 2],
    [1, 4, 3],
    [4, 2, 5, 1],
]

M = 0
K = len(arrays)
V = [None] * K
for i in range(K):
    M += len(arrays[i])
    V[i] = len(arrays[i])

def get_size(i, k, m):
    return floor(m / k * (i + 1)) - floor(m / k * i)

for i in range(K):
    i_required_size = get_size(i, K, M)
    if len(arrays[i]) > i_required_size:
        for j in range(K):
            j_required_size = get_size(j, K, M)
            if len(arrays[j]) < j_required_size:
                overflow = len(arrays[i]) - i_required_size
                underflow = j_required_size - len(arrays[j])
                transfer = min(overflow, underflow)
                arrays[j] += arrays[i][len(arrays[i]) - transfer:]
                arrays[i] = arrays[i][:len(arrays[i]) - transfer]
