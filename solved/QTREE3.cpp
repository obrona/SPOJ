#include <bits/stdc++.h>

using namespace std;

// we want the 1st black node from node 1 to node v
// we use binary search and Euler tour
// for black node, we put 1 for start and -1 for end, 0 for white node
// to find 1st black node from 1 to node i, we binary search. we find the node in the middle of 1 and n through binary lifting, 
// then use range sum to determine whether there is a black node from node 1 to node middle (if sum >= 1 then yes, otherwise no why?)
// the range sum is [start[1], start[middle node]]

class LCA {
public:
    int MAX = 17; 
    vector<vector<int>> binLift;
    vector<int> depth;

    LCA() {};

    LCA(int n, vector<vector<int>> adj): binLift(n, vector<int>(MAX + 1, -1)), depth(n, 0) {
        processBinLift(adj, 0, 0, 0);
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
        int pow = 0;
        while (jumps > 0) {
            if (v == -1) break;
            if (jumps & 1) v = binLift[v][pow];
            pow ++;
            jumps >>= 1;
        }  
        return v;
    }

    int getLca(int u, int v) {
        if (depth[u] < depth[v]) return getLca(v, u);
        
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

struct SegTree {
    int n;
    vector<int> tree;

    SegTree() {}

    SegTree(int n): n(n), tree(4 * n, 0) {}

    void build(int p, int s, int e, vector<int>& arr) {
        if (s == e) {
            tree[p] = arr[s];
            return;
        }

        int m = (s + e) / 2;
        build(2 * p + 1, s, m, arr);
        build(2 * p + 2, m + 1, e, arr);
        tree[p] = tree[2 * p + 1] + tree[2 * p + 2];
    }

    void update(int p, int s, int e, int idx, int val) {
        if (s == e) {
            tree[p] = val;
            return;
        }

        int m = (s + e) / 2;
        if (idx <= m) update(2 * p + 1, s, m, idx, val);
        else if (idx > m) update(2 * p + 2, m + 1, e, idx, val);
        tree[p] = tree[2 * p + 1] + tree[2 * p + 2];
    }

    int query(int p, int s, int e, int l, int r) {
        if (s == e || (s == l && e == r)) return tree[p];
        
        int m = (s + e) / 2;
        if (r <= m) return query(2 * p + 1, s, m, l, r);
        else if (l > m) return query(2 * p + 2, m + 1, e, l, r);
        else return query(2 * p + 1, s, m, l, m) + query(2 * p + 2, m + 1, e, m + 1, r);
    }
};

LCA lca;
SegTree st;
vector<int> colors;
//vector<int> ett;
vector<vector<int>> startEndTime;
vector<vector<int>> adj;

int dfs(int v, int p, int t) {
    startEndTime[v][0] = t;
    t ++;

    for (int c : adj[v]) {
        if (c == p) continue;
        t = dfs(c, v, t);
    }

    startEndTime[v][1] = t;
    t ++;
    return t;
}

void update(int v) {
    colors[v] = 1 - colors[v];
    st.update(0, 0, st.n - 1, startEndTime[v][0], colors[v]);
    st.update(0, 0, st.n - 1, startEndTime[v][1], -colors[v]);
}

// return -2 if no solution
int query(int v) {
    int s = 0;
    int e = lca.depth[v];
    //cout << v << " " << e << endl;
    
    while (s < e) {
        int m = (s + e) / 2 + 1 - (e - s + 1) % 2;
        int node = lca.getIthParent(v, m);
        int sum = st.query(0, 0, st.n - 1, 0, startEndTime[node][0]);
        
        if (sum >= 1) {
            s = m;
        } else {
            e = m - 1;
        }
    }

    if (s == 0) {
        return (colors[v] == 1) ? v : -2;
    }
    return lca.getIthParent(v, s);

}

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    int N, Q;
    cin >> N >> Q;
    adj = vector<vector<int>>(N, vector<int>());
    colors = vector<int>(N, 0);
    startEndTime = vector<vector<int>>(N, {0, 0});
    st = SegTree(2 * N); // the size of the euler tour is 2 * N

    for (int i = 0; i < N - 1; i ++) {
        int a, b;
        cin >> a >> b;
        a --; b --;
        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    lca = LCA(N, adj);
    dfs(0, 0, 0);
    
    //update(7);
    //cout << st.query(0, 0, st.n - 1, 0, 9) << endl;

    for (int i = 0; i < Q; i ++) {
        int t, v;
        cin >> t >> v;
        v --;
        if (t == 0) {
            update(v);
        } else {
            cout << (query(v) + 1) << "\n";
        }
    }

}