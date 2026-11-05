/*
 * Project : Campus Route Finder Using Graphs
 * Subject : Data Structures and Algorithms [R25-EC-PCC-202]
 * Unit V  : Trees and Graphs (CCE III Course Project)
 *
 * Graph representation : Adjacency list (manual singly linked lists)
 * Algorithms           : BFS, DFS, Dijkstra's shortest path, connectivity check,
 *                        selection sort, binary search, linear search
 * Helper structures    : IntQueue (for BFS), IntStack (for path printing)
 *                        -- all implemented manually, NO STL containers used.
 *
 * Compile : g++ campus_route_finder.cpp -o campus_route_finder
 * Run     : ./campus_route_finder
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <climits>
using namespace std;

const int MAX_LOC   = 30;       // maximum number of campus locations
const int NAME_LEN  = 40;       // maximum length of a location name
const int INF       = INT_MAX;  // "infinite" distance
const int WALK_SPEED = 80;      // average walking speed in metres per minute

/* ------------------------------------------------------------------ */
/*  Small string helpers (no std::string needed)                       */
/* ------------------------------------------------------------------ */

char lowerChar(char c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    return c;
}

// Case-insensitive compare: <0 if a<b, 0 if equal, >0 if a>b
int compareIgnoreCase(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        char x = lowerChar(a[i]);
        char y = lowerChar(b[i]);
        if (x != y) return x - y;
        i++;
    }
    return lowerChar(a[i]) - lowerChar(b[i]);
}

bool isAllDigits(const char* s) {
    if (s[0] == '\0') return false;
    for (int i = 0; s[i] != '\0'; i++)
        if (s[i] < '0' || s[i] > '9') return false;
    return true;
}

void readLine(const char* prompt, char* buf, int size) {
    cout << prompt;
    cin.getline(buf, size);
    if (cin.eof()) { cout << "\nInput ended. Exiting.\n"; exit(0); }
    if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); buf[0] = '\0'; }
}

int readInt(const char* prompt) {
    char buf[32];
    readLine(prompt, buf, 32);
    return atoi(buf);
}

/* ------------------------------------------------------------------ */
/*  Distance class  (demonstrates constructors + operator overloading) */
/* ------------------------------------------------------------------ */

class Distance {
    int meters;
public:
    Distance(int m = 0) : meters(m) {}                       // constructor
    int getMeters() const { return meters; }
    int walkingMinutes() const {                             // rounded up
        return (meters + WALK_SPEED - 1) / WALK_SPEED;
    }
    Distance operator+(const Distance& other) const {        // d1 + d2
        return Distance(meters + other.meters);
    }
    bool operator<(const Distance& other) const {            // d1 < d2
        return meters < other.meters;
    }
    friend ostream& operator<<(ostream& os, const Distance& d) {  // cout << d
        os << d.meters << " m";
        return os;
    }
};

/* ------------------------------------------------------------------ */
/*  Manual Queue (circular array) -- used by BFS                       */
/* ------------------------------------------------------------------ */

class IntQueue {
    int data[MAX_LOC];
    int frontIdx, rearIdx, size;
public:
    IntQueue() : frontIdx(0), rearIdx(0), size(0) {}
    bool isEmpty() const { return size == 0; }
    void enqueue(int x) {
        if (size == MAX_LOC) return;
        data[rearIdx] = x;
        rearIdx = (rearIdx + 1) % MAX_LOC;
        size++;
    }
    int dequeue() {
        int x = data[frontIdx];
        frontIdx = (frontIdx + 1) % MAX_LOC;
        size--;
        return x;
    }
};

/* ------------------------------------------------------------------ */
/*  Manual Stack (array) -- used to print the shortest path in order   */
/* ------------------------------------------------------------------ */

class IntStack {
    int data[MAX_LOC];
    int topIdx;
public:
    IntStack() : topIdx(-1) {}
    bool isEmpty() const { return topIdx == -1; }
    void push(int x) { if (topIdx < MAX_LOC - 1) data[++topIdx] = x; }
    int pop() { return data[topIdx--]; }
};

/* ------------------------------------------------------------------ */
/*  Adjacency list node                                                */
/* ------------------------------------------------------------------ */

struct AdjNode {
    int dest;          // destination location id
    int weight;        // distance in metres
    AdjNode* next;
    AdjNode(int d, int w) : dest(d), weight(w), next(NULL) {}
};

/* ------------------------------------------------------------------ */
/*  Campus graph (undirected, weighted)                                */
/* ------------------------------------------------------------------ */

class CampusGraph {
    char names[MAX_LOC][NAME_LEN];   // location names
    AdjNode* adj[MAX_LOC];           // adjacency lists
    bool active[MAX_LOC];            // false if the location was removed
    int count;                       // number of ids used so far

    /* ---- one-directional edge helpers ---- */
    void addEdgeOneWay(int u, int v, int w) {
        for (AdjNode* p = adj[u]; p != NULL; p = p->next) {
            if (p->dest == v) { p->weight = w; return; }   // update existing
        }
        AdjNode* node = new AdjNode(v, w);                  // insert at head
        node->next = adj[u];
        adj[u] = node;
    }

    bool removeEdgeOneWay(int u, int v) {
        AdjNode* prev = NULL;
        AdjNode* cur = adj[u];
        while (cur != NULL) {
            if (cur->dest == v) {
                if (prev == NULL) adj[u] = cur->next;
                else prev->next = cur->next;
                delete cur;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    void clearList(int u) {
        AdjNode* cur = adj[u];
        while (cur != NULL) {
            AdjNode* temp = cur;
            cur = cur->next;
            delete temp;
        }
        adj[u] = NULL;
    }

    /* ---- recursive helper for DFS ---- */
    void dfsVisit(int u, bool visited[]) const {
        visited[u] = true;
        cout << names[u] << "  ";
        for (AdjNode* p = adj[u]; p != NULL; p = p->next)
            if (!visited[p->dest]) dfsVisit(p->dest, visited);
    }

    /* ---- Dijkstra's algorithm (array based, O(V^2 + E)) ---- */
    void dijkstra(int src, int dist[], int parent[]) const {
        bool done[MAX_LOC];
        for (int i = 0; i < MAX_LOC; i++) {
            dist[i] = INF;
            parent[i] = -1;
            done[i] = false;
        }
        dist[src] = 0;

        for (int iter = 0; iter < count; iter++) {
            int u = -1;                          // pick unvisited node with min dist
            for (int i = 0; i < count; i++) {
                if (active[i] && !done[i] && dist[i] != INF &&
                    (u == -1 || dist[i] < dist[u]))
                    u = i;
            }
            if (u == -1) break;                  // remaining nodes unreachable
            done[u] = true;

            for (AdjNode* p = adj[u]; p != NULL; p = p->next) {   // relax edges
                if (!done[p->dest] && dist[u] + p->weight < dist[p->dest]) {
                    dist[p->dest] = dist[u] + p->weight;
                    parent[p->dest] = u;
                }
            }
        }
    }

    int edgeWeight(int u, int v) const {
        for (AdjNode* p = adj[u]; p != NULL; p = p->next)
            if (p->dest == v) return p->weight;
        return 0;
    }

public:
    CampusGraph() : count(0) {
        for (int i = 0; i < MAX_LOC; i++) {
            adj[i] = NULL;
            active[i] = false;
            names[i][0] = '\0';
        }
    }

    ~CampusGraph() {
        for (int i = 0; i < count; i++) clearList(i);
    }

    bool isValid(int id) const { return id >= 0 && id < count && active[id]; }
    int  totalIds() const { return count; }
    const char* getName(int id) const { return names[id]; }

    /* ---------------- location operations ---------------- */

    // Linear search by name (case-insensitive). Returns id or -1.
    int findLocation(const char* name) const {
        for (int i = 0; i < count; i++)
            if (active[i] && compareIgnoreCase(names[i], name) == 0) return i;
        return -1;
    }

    int addLocation(const char* name) {
        if (name[0] == '\0') { cout << "Name cannot be empty.\n"; return -1; }
        if (findLocation(name) != -1) { cout << "Location already exists.\n"; return -1; }
        if (count == MAX_LOC) { cout << "Campus map is full.\n"; return -1; }
        strncpy(names[count], name, NAME_LEN - 1);
        names[count][NAME_LEN - 1] = '\0';
        active[count] = true;
        adj[count] = NULL;
        return count++;
    }

    bool removeLocation(int id) {
        if (!isValid(id)) return false;
        for (int i = 0; i < count; i++)
            if (active[i] && i != id) removeEdgeOneWay(i, id);
        clearList(id);
        active[id] = false;
        return true;
    }

    /* ---------------- path (edge) operations ---------------- */

    bool addPath(int u, int v, int w) {
        if (!isValid(u) || !isValid(v) || u == v || w <= 0) return false;
        addEdgeOneWay(u, v, w);     // undirected: add both directions
        addEdgeOneWay(v, u, w);
        return true;
    }

    bool removePath(int u, int v) {
        if (!isValid(u) || !isValid(v)) return false;
        bool a = removeEdgeOneWay(u, v);
        bool b = removeEdgeOneWay(v, u);
        return a || b;
    }

    /* ---------------- display ---------------- */

    void displayMap() const {
        int total = 0;
        cout << "\n----- CAMPUS MAP (Adjacency List) -----\n";
        for (int i = 0; i < count; i++) {
            if (!active[i]) continue;
            total++;
            cout << "[" << i << "] " << names[i] << " -> ";
            if (adj[i] == NULL) cout << "(no paths)";
            for (AdjNode* p = adj[i]; p != NULL; p = p->next) {
                cout << names[p->dest] << " (" << Distance(p->weight) << ")";
                if (p->next != NULL) cout << ", ";
            }
            cout << "\n";
        }
        if (total == 0) cout << "Map is empty.\n";
    }

    // Selection sort of active location ids by name. Returns how many.
    int getSortedIds(int sorted[]) const {
        int n = 0;
        for (int i = 0; i < count; i++)
            if (active[i]) sorted[n++] = i;
        for (int i = 0; i < n - 1; i++) {
            int minPos = i;
            for (int j = i + 1; j < n; j++)
                if (compareIgnoreCase(names[sorted[j]], names[sorted[minPos]]) < 0)
                    minPos = j;
            int temp = sorted[i];
            sorted[i] = sorted[minPos];
            sorted[minPos] = temp;
        }
        return n;
    }

    void listSorted() const {
        int sorted[MAX_LOC];
        int n = getSortedIds(sorted);
        cout << "\n----- LOCATIONS (A-Z) -----\n";
        if (n == 0) cout << "No locations.\n";
        for (int i = 0; i < n; i++)
            cout << (i + 1) << ". " << names[sorted[i]] << "  (ID " << sorted[i] << ")\n";
    }

    // Binary search on the name-sorted list
    void binarySearchLocation(const char* name) const {
        int sorted[MAX_LOC];
        int n = getSortedIds(sorted);
        int low = 0, high = n - 1;
        while (low <= high) {
            int mid = (low + high) / 2;
            int cmp = compareIgnoreCase(names[sorted[mid]], name);
            if (cmp == 0) {
                int id = sorted[mid];
                int degree = 0;
                for (AdjNode* p = adj[id]; p != NULL; p = p->next) degree++;
                cout << "Found: " << names[id] << " (ID " << id << "), directly connected to "
                     << degree << " location(s).\n";
                return;
            }
            if (cmp < 0) low = mid + 1;
            else high = mid - 1;
        }
        cout << "Location not found.\n";
    }

    /* ---------------- traversals ---------------- */

    // BFS: prints locations level by level (hops from the start point)
    void bfs(int start) const {
        bool visited[MAX_LOC] = {false};
        int level[MAX_LOC] = {0};
        IntQueue q;
        visited[start] = true;
        q.enqueue(start);
        cout << "\nBFS from " << names[start] << " (location : hops):\n";
        while (!q.isEmpty()) {
            int u = q.dequeue();
            cout << "  " << names[u] << " : " << level[u] << "\n";
            for (AdjNode* p = adj[u]; p != NULL; p = p->next) {
                if (!visited[p->dest]) {
                    visited[p->dest] = true;
                    level[p->dest] = level[u] + 1;
                    q.enqueue(p->dest);
                }
            }
        }
    }

    void dfs(int start) const {
        bool visited[MAX_LOC] = {false};
        cout << "\nDFS from " << names[start] << ":\n  ";
        dfsVisit(start, visited);
        cout << "\n";
    }

    /* ---------------- shortest route ---------------- */

    void shortestRoute(int src, int dest) const {
        int dist[MAX_LOC], parent[MAX_LOC];
        dijkstra(src, dist, parent);

        if (dist[dest] == INF) {
            cout << "No route exists between " << names[src] << " and " << names[dest] << ".\n";
            return;
        }

        // Walk back from dest to src using parent[], push on a stack,
        // then pop to get the path in correct (src -> dest) order.
        IntStack path;
        for (int v = dest; v != -1; v = parent[v]) path.push(v);

        cout << "\nShortest route from " << names[src] << " to " << names[dest] << ":\n";
        int prev = path.pop();
        cout << "  Start : " << names[prev] << "\n";
        while (!path.isEmpty()) {
            int cur = path.pop();
            cout << "  Go to : " << names[cur] << "  (" << Distance(edgeWeight(prev, cur)) << ")\n";
            prev = cur;
        }
        Distance total(dist[dest]);
        cout << "Total distance : " << total << "\n";
        cout << "Walking time   : about " << total.walkingMinutes() << " min\n";
    }

    // All locations within 'limit' metres of src, sorted nearest-first
    void nearbyLocations(int src, int limit) const {
        int dist[MAX_LOC], parent[MAX_LOC];
        dijkstra(src, dist, parent);

        int ids[MAX_LOC];
        int n = 0;
        for (int i = 0; i < count; i++)
            if (active[i] && i != src && dist[i] != INF && dist[i] <= limit)
                ids[n++] = i;

        // selection sort by distance (uses overloaded operator<)
        for (int i = 0; i < n - 1; i++) {
            int minPos = i;
            for (int j = i + 1; j < n; j++)
                if (Distance(dist[ids[j]]) < Distance(dist[ids[minPos]])) minPos = j;
            int temp = ids[i];
            ids[i] = ids[minPos];
            ids[minPos] = temp;
        }

        cout << "\nLocations within " << limit << " m of " << names[src] << ":\n";
        if (n == 0) cout << "  None.\n";
        for (int i = 0; i < n; i++)
            cout << "  " << (i + 1) << ". " << names[ids[i]] << " - " << Distance(dist[ids[i]])
                 << " (~" << Distance(dist[ids[i]]).walkingMinutes() << " min)\n";
    }

    /* ---------------- connectivity ---------------- */

    void checkConnectivity() const {
        int first = -1, totalActive = 0;
        for (int i = 0; i < count; i++)
            if (active[i]) { totalActive++; if (first == -1) first = i; }
        if (first == -1) { cout << "Map is empty.\n"; return; }

        bool visited[MAX_LOC] = {false};
        IntQueue q;
        visited[first] = true;
        q.enqueue(first);
        int reached = 1;
        while (!q.isEmpty()) {
            int u = q.dequeue();
            for (AdjNode* p = adj[u]; p != NULL; p = p->next) {
                if (!visited[p->dest]) {
                    visited[p->dest] = true;
                    reached++;
                    q.enqueue(p->dest);
                }
            }
        }
        if (reached == totalActive) {
            cout << "The campus map is fully connected. Every location can be reached.\n";
        } else {
            cout << "The campus map is NOT fully connected. Unreachable from "
                 << names[first] << ":\n";
            for (int i = 0; i < count; i++)
                if (active[i] && !visited[i]) cout << "  - " << names[i] << "\n";
        }
    }
};

/* ------------------------------------------------------------------ */
/*  Input helper: ask for a location by name OR by ID                  */
/* ------------------------------------------------------------------ */

int askLocation(const CampusGraph& g, const char* prompt) {
    char buf[NAME_LEN];
    readLine(prompt, buf, NAME_LEN);
    int id;
    if (isAllDigits(buf)) id = atoi(buf);
    else id = g.findLocation(buf);
    if (!g.isValid(id)) {
        cout << "Location not found.\n";
        return -1;
    }
    return id;
}

/* ------------------------------------------------------------------ */
/*  Sample data                                                        */
/* ------------------------------------------------------------------ */

void loadSampleCampus(CampusGraph& g) {
    int gate     = g.addLocation("Main Gate");
    int admin    = g.addLocation("Admin Building");
    int library  = g.addLocation("Library");
    int canteen  = g.addLocation("Canteen");
    int dept     = g.addLocation("ECE Department");
    int lab      = g.addLocation("Computer Lab");
    int audi     = g.addLocation("Auditorium");
    int ground   = g.addLocation("Sports Ground");
    int hostel   = g.addLocation("Hostel");
    int parking  = g.addLocation("Parking");

    g.addPath(gate, admin, 150);
    g.addPath(gate, parking, 80);
    g.addPath(parking, admin, 100);
    g.addPath(admin, library, 120);
    g.addPath(admin, audi, 200);
    g.addPath(library, dept, 90);
    g.addPath(library, canteen, 140);
    g.addPath(dept, lab, 40);
    g.addPath(dept, canteen, 110);
    g.addPath(canteen, ground, 160);
    g.addPath(canteen, hostel, 250);
    g.addPath(ground, hostel, 130);
    g.addPath(audi, library, 170);
}

/* ------------------------------------------------------------------ */
/*  Menu                                                               */
/* ------------------------------------------------------------------ */

void showMenu() {
    cout << "\n=========== CAMPUS ROUTE FINDER ===========\n"
         << " 1. Display campus map\n"
         << " 2. Add a location\n"
         << " 3. Add a path between two locations\n"
         << " 4. Remove a path\n"
         << " 5. Remove a location\n"
         << " 6. Search a location (binary search)\n"
         << " 7. List all locations (sorted A-Z)\n"
         << " 8. BFS traversal from a location\n"
         << " 9. DFS traversal from a location\n"
         << "10. Find shortest route (Dijkstra)\n"
         << "11. Find nearby locations within a distance\n"
         << "12. Check campus connectivity\n"
         << " 0. Exit\n"
         << "===========================================\n";
}

int main() {
    CampusGraph campus;
    loadSampleCampus(campus);
    cout << "Welcome to the Campus Route Finder!\n"
         << "(A sample campus map has been loaded. You can modify it from the menu.)\n";

    int choice;
    do {
        showMenu();
        choice = readInt("Enter your choice: ");
        char name[NAME_LEN];

        switch (choice) {
        case 1:
            campus.displayMap();
            break;

        case 2: {
            readLine("Enter new location name: ", name, NAME_LEN);
            int id = campus.addLocation(name);
            if (id != -1) cout << "Added \"" << name << "\" with ID " << id << ".\n";
            break;
        }

        case 3: {
            int u = askLocation(campus, "Enter first location (name or ID): ");
            if (u == -1) break;
            int v = askLocation(campus, "Enter second location (name or ID): ");
            if (v == -1) break;
            int w = readInt("Enter distance in metres: ");
            if (campus.addPath(u, v, w)) cout << "Path added/updated.\n";
            else cout << "Invalid path (locations must differ and distance must be > 0).\n";
            break;
        }

        case 4: {
            int u = askLocation(campus, "Enter first location (name or ID): ");
            if (u == -1) break;
            int v = askLocation(campus, "Enter second location (name or ID): ");
            if (v == -1) break;
            if (campus.removePath(u, v)) cout << "Path removed.\n";
            else cout << "No such path exists.\n";
            break;
        }

        case 5: {
            int u = askLocation(campus, "Enter location to remove (name or ID): ");
            if (u == -1) break;
            campus.removeLocation(u);
            cout << "Location and all its paths removed.\n";
            break;
        }

        case 6:
            readLine("Enter location name to search: ", name, NAME_LEN);
            campus.binarySearchLocation(name);
            break;

        case 7:
            campus.listSorted();
            break;

        case 8: {
            int s = askLocation(campus, "Enter starting location (name or ID): ");
            if (s != -1) campus.bfs(s);
            break;
        }

        case 9: {
            int s = askLocation(campus, "Enter starting location (name or ID): ");
            if (s != -1) campus.dfs(s);
            break;
        }

        case 10: {
            int s = askLocation(campus, "Enter source location (name or ID): ");
            if (s == -1) break;
            int d = askLocation(campus, "Enter destination location (name or ID): ");
            if (d == -1) break;
            campus.shortestRoute(s, d);
            break;
        }

        case 11: {
            int s = askLocation(campus, "Enter your current location (name or ID): ");
            if (s == -1) break;
            int limit = readInt("Enter maximum distance in metres: ");
            campus.nearbyLocations(s, limit);
            break;
        }

        case 12:
            campus.checkConnectivity();
            break;

        case 0:
            cout << "Thank you for using Campus Route Finder. Goodbye!\n";
            break;

        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
/*PS C:\Users\LENOVO> cd 'c:\Users\LENOVO\Downloads\output'
PS C:\Users\LENOVO\Downloads\output> & .\'routefinder.exe'
Welcome to the Campus Route Finder!
(A sample campus map has been loaded. You can modify it from the menu.)

=========== CAMPUS ROUTE FINDER ===========
 1. Display campus map
 2. Add a location
 3. Add a path between two locations
 4. Remove a path
 5. Remove a location
 6. Search a location (binary search)
 7. List all locations (sorted A-Z)
 8. BFS traversal from a location
 9. DFS traversal from a location
10. Find shortest route (Dijkstra)
11. Find nearby locations within a distance
12. Check campus connectivity
 0. Exit
===========================================
Enter your choice: 1

----- CAMPUS MAP (Adjacency List) -----
[0] Main Gate -> Parking (80 m), Admin Building (150 m)
[1] Admin Building -> Auditorium (200 m), Library (120 m), Parking (100 m), Main Gate (150 m)
[2] Library -> Auditorium (170 m), Canteen (140 m), ECE Department (90 m), Admin Building (120 m)
[3] Canteen -> Hostel (250 m), Sports Ground (160 m), ECE Department (110 m), Library (140 m)
[4] ECE Department -> Canteen (110 m), Computer Lab (40 m), Library (90 m)
[5] Computer Lab -> ECE Department (40 m)
[6] Auditorium -> Library (170 m), Admin Building (200 m)
[7] Sports Ground -> Hostel (130 m), Canteen (160 m)
[8] Hostel -> Sports Ground (130 m), Canteen (250 m)
[9] Parking -> Admin Building (100 m), Main Gate (80 m)

=========== CAMPUS ROUTE FINDER ===========
 1. Display campus map
 2. Add a location
 3. Add a path between two locations
 4. Remove a path
 5. Remove a location
 6. Search a location (binary search)
 7. List all locations (sorted A-Z)
 8. BFS traversal from a location
 9. DFS traversal from a location
10. Find shortest route (Dijkstra)
11. Find nearby locations within a distance
12. Check campus connectivity
 0. Exit
===========================================
Enter your choice: 12
The campus map is fully connected. Every location can be reached.

=========== CAMPUS ROUTE FINDER ===========
 1. Display campus map
 2. Add a location
 3. Add a path between two locations
 4. Remove a path
 5. Remove a location
 6. Search a location (binary search)
 7. List all locations (sorted A-Z)
 8. BFS traversal from a location
 9. DFS traversal from a location
10. Find shortest route (Dijkstra)
11. Find nearby locations within a distance
12. Check campus connectivity
 0. Exit
===========================================
Enter your choice: 8
Enter starting location (name or ID): Hostel

BFS from Hostel (location : hops):
  Hostel : 0
  Sports Ground : 1
  Canteen : 1
  ECE Department : 2
  Library : 2
  Computer Lab : 3
  Auditorium : 3
  Admin Building : 3
  Parking : 4
  Main Gate : 4

=========== CAMPUS ROUTE FINDER ===========
 1. Display campus map
 2. Add a location
 3. Add a path between two locations
 4. Remove a path
 5. Remove a location
 6. Search a location (binary search)
 7. List all locations (sorted A-Z)
 8. BFS traversal from a location
 9. DFS traversal from a location
10. Find shortest route (Dijkstra)
11. Find nearby locations within a distance
12. Check campus connectivity
 0. Exit
===========================================
Enter your choice: */
