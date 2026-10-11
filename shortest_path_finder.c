/*
 * Shortest Path Finder (City Map)
 * Algorithms: Dijkstra, Bellman-Ford, Floyd-Warshall, BFS, DFS
 * Compile: gcc shortest_path_finder.c -o spf
 * Run:     ./spf
 */
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX 20
#define INF 1000000000

int quiet = 0;   /* 1 = suppress output (used for timing) */
#define OUT(...) do { if (!quiet) printf(__VA_ARGS__); } while (0)

typedef struct { int u, v, w; } Edge;

int n = 0;                    /* number of cities */
int m = 0;                    /* number of directed edges */
char names[MAX][30];
int adj[MAX][MAX];            /* adjacency matrix (INF = no road) */
Edge edges[MAX * MAX];        /* edge list for Bellman-Ford */

/* ---------- Graph setup ---------- */
void initGraph(void) {
    n = m = 0;
    for (int i = 0; i < MAX; i++)
        for (int j = 0; j < MAX; j++)
            adj[i][j] = (i == j) ? 0 : INF;
}

int addCity(const char *name) {
    if (n >= MAX) { OUT("City limit reached.\n"); return -1; }
    strncpy(names[n], name, 29);
    names[n][29] = '\0';
    return n++;
}

void addRoad(int u, int v, int w) {
    if (u < 0 || v < 0 || u >= n || v >= n || u == v || w <= 0) {
        OUT("Invalid road.\n");
        return;
    }
    adj[u][v] = adj[v][u] = w;                 /* two-way road */
    edges[m++] = (Edge){u, v, w};
    edges[m++] = (Edge){v, u, w};
}

void loadSample(void) {
    initGraph();
    const char *c[] = {"Delhi", "Jaipur", "Agra", "Lucknow", "Bhopal", "Mumbai"};
    for (int i = 0; i < 6; i++) addCity(c[i]);
    addRoad(0, 1, 280);  addRoad(0, 2, 230);  addRoad(1, 2, 240);
    addRoad(2, 3, 330);  addRoad(2, 4, 600);  addRoad(1, 4, 600);
    addRoad(3, 4, 780);  addRoad(4, 5, 780);  addRoad(1, 5, 1150);
}

void showCities(void) {
    OUT("\nCities:\n");
    for (int i = 0; i < n; i++) OUT("  %d. %s\n", i, names[i]);
}

void showMap(void) {
    OUT("\nRoads (km):\n");
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (adj[i][j] != INF)
                OUT("  %s <-> %s : %d\n", names[i], names[j], adj[i][j]);
}

/* ---------- Helpers ---------- */
void printPath(int parent[], int s, int d) {
    if (d == s) { OUT("%s", names[s]); return; }
    if (parent[d] == -1) { OUT("(no path)"); return; }
    printPath(parent, s, parent[d]);
    OUT(" -> %s", names[d]);
}

int readCity(const char *prompt) {
    int x;
    showCities();
    OUT("%s", prompt);
    if (scanf("%d", &x) != 1 || x < 0 || x >= n) {
        OUT("Invalid city.\n");
        int ch; while ((ch = getchar()) != '\n' && ch != EOF);
        return -1;
    }
    return x;
}

/* ---------- Dijkstra: O(V^2), no negative weights ---------- */
void dijkstra(int s, int d) {
    int dist[MAX], parent[MAX], vis[MAX] = {0};
    for (int i = 0; i < n; i++) { dist[i] = INF; parent[i] = -1; }
    dist[s] = 0;

    for (int k = 0; k < n; k++) {
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!vis[i] && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1 || dist[u] == INF) break;
        vis[u] = 1;
        for (int v = 0; v < n; v++)
            if (adj[u][v] != INF && dist[u] + adj[u][v] < dist[v]) {
                dist[v] = dist[u] + adj[u][v];
                parent[v] = u;
            }
    }
    OUT("\n[Dijkstra] ");
    if (dist[d] == INF) { OUT("No route from %s to %s.\n", names[s], names[d]); return; }
    OUT("Distance: %d km\nPath: ", dist[d]);
    printPath(parent, s, d);
    OUT("\n");
}

/* ---------- Bellman-Ford: O(V*E), handles negative weights ---------- */
void bellmanFord(int s, int d) {
    int dist[MAX], parent[MAX];
    for (int i = 0; i < n; i++) { dist[i] = INF; parent[i] = -1; }
    dist[s] = 0;

    for (int i = 1; i < n; i++)
        for (int j = 0; j < m; j++) {
            int u = edges[j].u, v = edges[j].v, w = edges[j].w;
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
            }
        }
    for (int j = 0; j < m; j++)               /* negative cycle check */
        if (dist[edges[j].u] != INF &&
            dist[edges[j].u] + edges[j].w < dist[edges[j].v]) {
            OUT("\n[Bellman-Ford] Negative cycle detected!\n");
            return;
        }
    OUT("\n[Bellman-Ford] ");
    if (dist[d] == INF) { OUT("No route from %s to %s.\n", names[s], names[d]); return; }
    OUT("Distance: %d km\nPath: ", dist[d]);
    printPath(parent, s, d);
    OUT("\n");
}

/* ---------- Floyd-Warshall: O(V^3), all pairs ---------- */
void floydWarshall(int s, int d) {
    int dist[MAX][MAX], next[MAX][MAX];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            dist[i][j] = adj[i][j];
            next[i][j] = (adj[i][j] != INF && i != j) ? j : -1;
        }
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }

    OUT("\n[Floyd-Warshall] All-pairs shortest distances (km):\n%-9s", "");
    for (int j = 0; j < n; j++) OUT("%-9.8s", names[j]);
    OUT("\n");
    for (int i = 0; i < n; i++) {
        OUT("%-9.8s", names[i]);
        for (int j = 0; j < n; j++) {
            if (dist[i][j] == INF) OUT("%-9s", "INF");
            else OUT("%-9d", dist[i][j]);
        }
        OUT("\n");
    }
    if (dist[s][d] == INF) { OUT("\nNo route from %s to %s.\n", names[s], names[d]); return; }
    OUT("\n%s to %s: %d km\nPath: %s", names[s], names[d], dist[s][d], names[s]);
    for (int cur = s; cur != d; ) { cur = next[cur][d]; OUT(" -> %s", names[cur]); }
    OUT("\n");
}

/* ---------- BFS: fewest roads (hops) + reachability ---------- */
void bfs(int s) {
    int q[MAX], front = 0, rear = 0, vis[MAX] = {0}, hops[MAX] = {0};
    q[rear++] = s; vis[s] = 1;
    OUT("\n[BFS] Cities reachable from %s (with number of roads):\n", names[s]);
    while (front < rear) {
        int u = q[front++];
        OUT("  %s (%d)\n", names[u], hops[u]);
        for (int v = 0; v < n; v++)
            if (adj[u][v] != INF && u != v && !vis[v]) {
                vis[v] = 1; hops[v] = hops[u] + 1; q[rear++] = v;
            }
    }
    for (int i = 0; i < n; i++)
        if (!vis[i]) OUT("  %s is NOT reachable\n", names[i]);
}

/* ---------- DFS: traversal order ---------- */
void dfsRec(int u, int vis[]) {
    vis[u] = 1;
    OUT("%s ", names[u]);
    for (int v = 0; v < n; v++)
        if (adj[u][v] != INF && u != v && !vis[v]) dfsRec(v, vis);
}
void dfs(int s) {
    int vis[MAX] = {0};
    OUT("\n[DFS] Visit order from %s: ", names[s]);
    dfsRec(s, vis);
    OUT("\n");
}

/* ---------- Timing comparison ---------- */
void compare(int s, int d) {
    clock_t t;
    int runs = 20000;
    OUT("\nRunning each algorithm %d times (output hidden)...\n", runs);
    quiet = 1;
    t = clock();
    for (int i = 0; i < runs; i++) dijkstra(s, d);
    double a = (double)(clock() - t) / CLOCKS_PER_SEC;
    t = clock();
    for (int i = 0; i < runs; i++) bellmanFord(s, d);
    double b = (double)(clock() - t) / CLOCKS_PER_SEC;
    t = clock();
    for (int i = 0; i < runs; i++) floydWarshall(s, d);
    double c = (double)(clock() - t) / CLOCKS_PER_SEC;
    quiet = 0;
    OUT("Dijkstra       : %.4f s\nBellman-Ford   : %.4f s\nFloyd-Warshall : %.4f s\n", a, b, c);
}

/* ---------- Main menu ---------- */
int main(void) {
    loadSample();
    int ch;
    do {
        OUT("\n===== CITY SHORTEST PATH FINDER =====\n"
               "1. Show cities and roads\n"
               "2. Add city\n"
               "3. Add road\n"
               "4. Dijkstra\n"
               "5. Bellman-Ford\n"
               "6. Floyd-Warshall (all pairs)\n"
               "7. BFS (reachability)\n"
               "8. DFS (traversal)\n"
               "9. Compare running times\n"
               "0. Exit\nChoice: ");
        if (scanf("%d", &ch) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            ch = -1;
        }
        int s, d, w;
        char name[30];
        switch (ch) {
            case 1: showCities(); showMap(); break;
            case 2:
                OUT("City name: ");
                if (scanf("%29s", name) == 1 && addCity(name) >= 0) OUT("Added.\n");
                break;
            case 3:
                s = readCity("From: "); if (s < 0) break;
                d = readCity("To: ");   if (d < 0) break;
                OUT("Distance (km): ");
                if (scanf("%d", &w) != 1) { OUT("Invalid.\n"); break; }
                addRoad(s, d, w);
                break;
            case 4: case 5: case 6: case 9:
                s = readCity("Source: ");      if (s < 0) break;
                d = readCity("Destination: "); if (d < 0) break;
                if (ch == 4) dijkstra(s, d);
                else if (ch == 5) bellmanFord(s, d);
                else if (ch == 6) floydWarshall(s, d);
                else compare(s, d);
                break;
            case 7: s = readCity("Start city: "); if (s >= 0) bfs(s); break;
            case 8: s = readCity("Start city: "); if (s >= 0) dfs(s); break;
            case 0: OUT("Goodbye!\n"); break;
            default: OUT("Invalid choice.\n");
        }
    } while (ch != 0);
    return 0;
}
