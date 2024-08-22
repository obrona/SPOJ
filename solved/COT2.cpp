



#include <bits/stdc++.h>
#define LEN 40005
#define QUERY_LENGTH 100005
#define BLK 400
using namespace std;

// MO algorithm and cleverly flattening an tree by dfs so that path queries can be processed as an contigous range
// for each node, start time is when 1st entering this node, end time is when leaving this node
// eg   1
//     2 3
//    4   5
// becomes 1 2 4 4 2 3 5 5 3 1, the indices where v appears is the start and end time respectively
// for each query u, v, where startTime[u] < startTime[v] if lca(u, v) != u, v
// then consider number of distinct elems in range [endTime[u], startTime[v]] and startTime[lca], just have to add weight of lca into account
// else range is [startTime[u], startTime[v]]
// elems not in the path will be counted twice, so as we iterate through the flattened dfs traversal, if we encounter a node twice, 
// we remove its weight from the count;
// time limit quite tight need to try a few times
// by grouping according to sqrt blocks, let S be size;
// each query currL can be moved S times so QS
// currR is moved N times for each block, so (N/S) * N
// take S to be sqrt(N) to get the time complexity

struct Store {
    int l, r, idx, p;

    Store(int l, int r, int idx, int p) : l(l), r(r), idx(idx), p(p) {};

    bool operator <(const Store& b) {
        int blk = l / BLK;
        int bblk = b.l / BLK;
        return (blk == bblk) ? (r < b.r) : (blk < bblk);
    } 
};

class LCA {
public:
    int MAX = 16; 
    vector<vector<int>> binLift;
    vector<int> depth;

    LCA() {};

    // n == number of vertices 
    LCA(int n, vector<vector<int>>& adj): binLift(n, vector<int>(MAX + 1, -1)), depth(n, 0) {
        processBinLift(adj, 1, 1, 0);
    }

    void processBinLift(vector<vector<int>>& adj, int v, int p, int d) {
        depth[v] = d;
        if (v != p) {
            binLift[v][0] = p;
            for (int i = 1; i <= MAX; i ++) {
                int curr = binLift[binLift[v][i - 1]][i - 1]; // no need to check for binLift[v][i - 1], as we are in ith iteration if 2**(i-1) parent of v is valid
                if (curr == -1) break;
                binLift[v][i] = curr;
            }
        }
        
        for (int c : adj[v]) {
            if (c != p) processBinLift(adj, c, v, d + 1);
        }
    }

    int getIthParent(int v, int jumps) {
        int i = 0;
        while (jumps > 0) {
            if (jumps & 1 && binLift[v][i] != -1) v = binLift[v][i];
            jumps = jumps >> 1;
            i ++;
        }
        return v;
    }

    int getLca(int u, int v) {
        if (depth[u] < depth[v]) swap(u, v);
        
        u = getIthParent(u, depth[u] - depth[v]);
        if (u == v) return u;
        
        for (int i = MAX; i >= 0; i --) {
            if (binLift[u][i] != binLift[v][i]) {
                u = binLift[u][i];
                v = binLift[v][i];
            } 
        }
        return binLift[u][0];
    }
};

int flat[2 * LEN];
int startT[LEN];
int endT[LEN];
int weights[LEN];
int results[QUERY_LENGTH];
vector<int> visited; // storses how manyu times this node has been included, 0, 1, or 2 times
vector<Store> queries;
vector<vector<int>> adj;
unordered_map<int, int> hashmap;
LCA lca;



int dfs(int v, int p, int t) {
    flat[t] = v;
    startT[v] = t;
    t ++;
    int last = t;
    for (int c : adj[v]) {
        if (c == p) continue;
        last = dfs(c, v, last);
    }
    
    flat[last] = v;
    endT[v] = last;
    last ++;
    return last;
}

// returns -1, 0, 1 if adding ptr p decreases, maintains or increases num of distinct elems
int insert(int p) {
    int node = flat[p];
    int w = weights[node];
    int cnt = hashmap[w];
    
    // visited[node] is either 1 or 0
    if (visited[node] == 1) {
        hashmap[w] --;
        visited[node] ++;
        if (cnt == 1) return -1;
        else return 0;
    } else  {
        hashmap[w] ++;
        visited[node] ++;
        if (cnt == 0) return 1;
        else return 0;
    }
    
}

int remove(int p) {
    int node = flat[p];
    int w = weights[node];
    int cnt = hashmap[w];

    // visited[node] is either 1 or 2
    if (visited[node] == 2) {
        hashmap[w] ++;
        visited[node] --;
        if (cnt == 0) return 1;
        else return 0;
    } else {
        hashmap[w] --;
        visited[node] --;
        if (cnt == 1) return -1;
        else return 0;
    }
}

void MO() {
    // invariant, we always have the result for [l, r)
    int currL = 0;
    int currR = 0;
    int currBlk = 0;
    int ans = 0;
    for (Store& q : queries) {
        
        // new block, reset everything
        if (q.l / BLK != currBlk) {
            currBlk = q.l / BLK;
            currL = q.l;
            currR = q.l;
            hashmap.clear();
            fill(visited.begin(), visited.end(), 0);
            ans = 0;
        } 

        while (q.l < currL) {
            currL --;
            ans += insert(currL);
        } 

        while (q.l > currL) {
            ans += remove(currL);
            currL ++;
        }

        while (currR <= q.r) {
            ans += insert(currR);
            currR ++;
        }

        int weightP = weights[q.p];
        results[q.idx] = (hashmap[weightP] == 0) ? ans + 1 : ans;
        //cout << q.l << " " << q.r << " " << ans << endl;
    }
}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    int N, M;
    cin >> N >> M;
    visited = vector<int>(N + 1, 0);
    adj = vector<vector<int>>(N + 1, vector<int>());

    for (int i = 1; i <= N; i ++) {
        int w;
        cin >> w;
        weights[i] = w;
    }

    for (int i = 0; i < N - 1; i ++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    int lim = dfs(1, 1, 0);
    /*for (int i = 0; i < lim; i ++) {
        cout << flat[i] << " ";
    }*/
    cout << endl;
    
    lca = LCA(N + 1, adj);
    
    for (int i = 0; i < M; i ++) {
        int u, v;
        cin >> u >> v;
        if (startT[u] > startT[v]) swap(u, v);
        int p = lca.getLca(u, v);

        if (p == u) {
            queries.push_back(Store(startT[u], startT[v], i, p));
        } else {
            queries.push_back(Store(endT[u], startT[v], i, p));
        }
    }

    sort(queries.begin(), queries.end());
    MO();
    for (int i = 0; i < M; i ++) {
        cout << results[i] << '\n';
    }
}



