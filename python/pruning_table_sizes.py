import math
import numpy as np


# Complete pruning value: the exact number of moves needed to solve the whole block
print("--------------------------------------------------------------")
print("  ###  Table sizes (MB) for different pruning value ideas  ###  ")
print("       Number of corners in columns, number of edges in rows")
print("--------------------------------------------------------------\n")


print("Exact pruning value : exact number of moves needed to solve the block")

nc_max, ne_max = 5, 6
sizes = np.empty((nc_max, ne_max), dtype = np.int64)
for nc in range(nc_max):
    for ne in range(ne_max):
        exact_pruning_table_size = 1
        for c in range(nc):
            exact_pruning_table_size *= (8-c)*3
        for e in range(ne):
            exact_pruning_table_size *= (12-e)*2
        sizes[nc, ne] = exact_pruning_table_size

np.set_printoptions(precision=2, sign = " ")
print(sizes / 1.e6, '\n')

print("""Permutation pruning value : number of moves needed to restore the block pieces
to their home positions in the solved permutation state, but they can be misoriented""")

nc_max, ne_max = 5, 6
sizes = np.empty((nc_max, ne_max), dtype = np.int64)
for nc in range(nc_max):
    for ne in range(ne_max):
        exact_pruning_table_size = 1
        for c in range(nc):
            exact_pruning_table_size *= (8-c)
        for e in range(ne):
            exact_pruning_table_size *= (12-e)
        sizes[nc, ne] = exact_pruning_table_size

np.set_printoptions(precision=2, sign = " ")
print(sizes / 1.e6, '\n')

print("""Orientation pruning value : number of moves needed to restore the block pieces
to their home positions in the solved orientation, but they can be permuted""")
      
nc_max, ne_max = 5, 6
sizes = np.empty((nc_max, ne_max), dtype = np.int64)
for nc in range(nc_max):
    for ne in range(ne_max):
        exact_pruning_table_size = 1
        for c in range(nc):
            exact_pruning_table_size *= (8-c)*3
        for e in range(ne):
            exact_pruning_table_size *= (12-e)*2
        exact_pruning_table_size /= math.factorial(nc) * math.factorial(ne)
        sizes[nc, ne] = exact_pruning_table_size

np.set_printoptions(precision=2, sign = " ")
print(sizes / 1.e6, '\n')


print("""Layout pruning value : number of moves needed to restore the block pieces
to their home positions but they can be permuted and misoriented
(for 3 to 6 corners and 5 to 8 edges)""")
      
nc_max, ne_max = 6, 8
sizes = np.empty((nc_max, ne_max), dtype = np.int64)
for nc in range(nc_max):
    for ne in range(ne_max):
        exact_pruning_table_size = 1
        for c in range(nc):
            exact_pruning_table_size *= (8-c)
        for e in range(ne):
            exact_pruning_table_size *= (12-e)
        exact_pruning_table_size /= math.factorial(nc) * math.factorial(ne)
        sizes[nc, ne] = exact_pruning_table_size

np.set_printoptions(precision=2, sign = " ")
print(sizes[2:, 4:] / 1.e6)
