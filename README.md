# Shortest Path Finder (DAA Project)

A city-map shortest path project that implements and compares classic graph algorithms.

## Algorithms
| Algorithm | Paradigm | Time | Space | Use |
|---|---|---|---|---|
| Dijkstra | Greedy | O(V²) | O(V) | Single source, non-negative weights |
| Bellman-Ford | Dynamic Programming | O(V·E) | O(V) | Negative weights, cycle detection |
| Floyd-Warshall | Dynamic Programming | O(V³) | O(V²) | All pairs shortest paths |
| BFS | Queue traversal | O(V²) | O(V) | Fewest roads, reachability |
| DFS | Recursion / stack | O(V²) | O(V) | Traversal, reachability |

## Files
- `index.html`: interactive front end (open in any browser). Animated step-by-step run, live explanation, comparison table, add/remove cities and roads.
- `shortest_path_finder.c`: console version in C (menu driven).

## Run
- Front end: double-click `index.html`.
- C version:
  ```
  gcc shortest_path_finder.c -o spf
  ./spf
  ```

## Sample map
Delhi, Jaipur, Agra, Lucknow, Bhopal, Mumbai with road distances in km.
