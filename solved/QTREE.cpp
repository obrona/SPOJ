#include <bits/stdc++.h>
using namespace std;

// hld, except this time is more annoying, as it is on edges

class LCA {
public:
    int MAX = 14;

    // -1 if the 2 ** i parent is invalid
    vector<vector<int>> parents;
    vector<int> depths;

    LCA() {}

    LCA(int n, vector<vector<int>> adj): parents(n, vector<int>(MAX + 1, -1)), depths(n, 0) {
        build(0, 0, 0, adj);
    }

    void build(int v, int p, int d, vector<vector<int>>& adj) {
        depths[v] = d;
        parents[v][0] = p;
        for (int i = 1; i <= MAX; i ++) {
            parents[v][i] = parents[parents[v][i - 1]][i - 1];
            if (parents[v][i] == -1) break;
        }

        for (int c : adj[v]) {
            if (c == p) continue;
            build(c, v, d + 1, adj);
        }
    }

    int getKthParent(int v, int k) {
        int pow = 0;
        while (k > 0 && v != -1) {
            if (k & 1) v = parents[v][pow];
            pow ++;
            k >>= 1;
        }
        return v;
    }

    int getLCA(int v, int u) {
        if (depths[v] < depths[u]) return getLCA(u, v);  

        v = getKthParent(v, depths[v] - depths[u]);
        if (u == v) return v;

        for (int i = MAX; i >= 0; i --) {
            if (parents[v][i] != parents[u][i]) {
                v = parents[v][i];
                u = parents[u][i];
            }
        }
        return parents[v][0];
    }
};


class SegTree {
public:
    int size;
    vector<int> tree;
    
    SegTree(vector<int>* arr): size(arr->size()), tree(4 * arr->size(), 0) {
        if (arr->size() == 0) return;

        build(0, 0, size - 1, arr);
    }

    void build(int p, int s, int e, vector<int>* arr) {
        if (s == e) {
            tree[p] = (*arr)[s];
        } else {
            int m = (s + e) >> 1;
            build((p << 1) + 1, s, m, arr);
            build((p << 1) + 2, m + 1, e, arr);
            tree[p] = max(tree[(p << 1) + 1], tree[(p << 1) + 2]);
        }
    }

    int rangeMax(int p, int s, int e, int l, int r) {
        if (s == e || (s == l && e == r)) return tree[p];
        
        int m = (s + e) >> 1;
        if (r <= m) return rangeMax((p << 1) + 1, s, m, l, r);
        else if (l > m) return rangeMax((p << 1) + 2, m + 1, e, l, r);
        else return max(rangeMax((p << 1) + 1, s, m, l, m), rangeMax((p << 1) + 2, m + 1, e, m + 1, r));
    }

    void update(int p, int s, int e, int idx, int val) {
        if (s == e) tree[p] = val;
        else {
            int m = (s + e) >> 1;
            if (idx <= m) update((p << 1) + 1, s, m, idx, val);
            else update((p << 1) + 2, m + 1, e, idx, val);
            tree[p] = max(tree[(p << 1) + 1], tree[(p << 1) + 2]);
        }
    }
    
};

struct Edge {
    int u, v, w;

    Edge(int u, int v, int w): u(u), v(v), w(w) {}
};

struct Pair2 {
    int p0, p1;

    Pair2(int p0, int p1): p0(p0), p1(p1) {}
};

struct Pair3 {
    int p0, p1, p2;

    Pair3(int p0, int p1, int p2): p0(p0), p1(p1), p2(p2) {}
};

class Solution {
public:
    vector<Edge> edgeInfo; // {u, v, weight}
    vector<vector<Pair2>> adjList; // p0: dest node, p1: idx of edgeInfo
    
    vector<SegTree*> segtrees;
    vector<Pair3> segTreeInfo; // p0: segtree connected to, p1: index of segtree node , p2: edge index
    
    vector<int> weights;
    vector<int> depths;
    vector<int> heaviestNodes;
    vector<Pair2> segTreeNodeInfo; // p0: which segtree this node is inside, p1: which idx is the node in the segtree
    
    LCA Lca;

    Solution(int n): adjList(n, vector<Pair2>()), weights(n, 0), depths(n, 0), heaviestNodes(n, 0), 
        segTreeNodeInfo(n, Pair2(0, 0)) {
        
    }

    void process() {
        dfs(0, 0, 0);
        hld(0, 0, 0, new vector<int>(), -1, -1, -1);
    }

    int dfs(int v, int p, int d) {
        depths[v] = d;
        int w = 1;
        int heaviestNode = -1;
        int heaviestWeight = 0;
        for (Pair2& e : adjList[v]) {
            if (e.p0 == p) continue;
            
            int wc = dfs(e.p0, v, d + 1);
            if (wc > heaviestWeight) {
                heaviestNode = e.p0;
                heaviestWeight = wc;
            }
            w += wc;

        }
        heaviestNodes[v] = heaviestNode;
        weights[v] = w;
        return w;
    }

    void hld(int v, int p, int idx, vector<int>* ew, int segId, int segNodeIdx, int segEdgeIdx) {
        segTreeNodeInfo[v] = Pair2(segtrees.size(), idx);
        
        // is a leaf, has node has no children
        if (heaviestNodes[v] == -1) {
            segTreeInfo.emplace_back(segId, segNodeIdx, segEdgeIdx);
            segtrees.push_back(new SegTree(ew));
            return;
        }

        int thisSegId = segtrees.size();

        for (Pair2& e : adjList[v]) {
            if (e.p0 == heaviestNodes[v]) {
                ew->push_back(edgeInfo[e.p1].w);
                hld(e.p0, v, idx + 1, ew, segId, segNodeIdx, segEdgeIdx);
            }
        }

        for (Pair2& e : adjList[v]) {
            if (e.p0 == p || e.p0 == heaviestNodes[v]) continue;

            hld(e.p0, v, 0, new vector<int>(), thisSegId, idx, e.p1);
        }
    }

    void update(int idx, int val) {
        Edge& eInfo = edgeInfo[idx];
        eInfo.w = val;

        int uid = segTreeNodeInfo[eInfo.u].p0;
        int vid = segTreeNodeInfo[eInfo.v].p0;
        int uidx = segTreeNodeInfo[eInfo.u].p1;
        int vidx = segTreeNodeInfo[eInfo.v].p1;
        
        // case 1: edge lies 'inside' a segtree
        if (uid == vid) {
            segtrees[uid]->update(0, 0, segtrees[uid]->size - 1, min(uidx, vidx), val);
        } 
        // case 2: edge connects 2 segtrees, then no need to do anything
    }

    int getMax(int lca, int u) {
        if (lca == u) return 0;

        int maxWeight = 0;
        int lcaId = segTreeNodeInfo[lca].p0;
        int lcaIdx = segTreeNodeInfo[lca].p1;

        int currSegTreeId = segTreeNodeInfo[u].p0;
        int currIdx = segTreeNodeInfo[u].p1;

        while (1) {
            if (currSegTreeId != lcaId) {
                int res = (currIdx == 0) ? 0 : segtrees[currSegTreeId]->rangeMax(0, 0, segtrees[currSegTreeId]->size - 1, 0, currIdx - 1);
                int connect = edgeInfo[segTreeInfo[currSegTreeId].p2].w; // get weight of connecting edge

                maxWeight = max(maxWeight, max(res, connect));
                
                currIdx = segTreeInfo[currSegTreeId].p1;
                currSegTreeId = segTreeInfo[currSegTreeId].p0;
            } else {
                int res = (currIdx == lcaIdx) ? 0 : segtrees[lcaId]->rangeMax(0, 0, segtrees[lcaId]->size - 1, lcaIdx, currIdx - 1);
                maxWeight = max(maxWeight, res);
                break;
            }
        }
        return maxWeight;
    }

    int query(int a, int b) {
        if (a == b) return 0;

        int lca = Lca.getLCA(a, b);
        return max(getMax(lca, a), getMax(lca, b));
    }

    

};

// input indices are 1 indexed

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);

    int t;
    cin >> t;

    for (int i = 0; i < t; i ++) {
        int n;
        cin >> n;

        vector<vector<int>> adj(n, vector<int>());
        Solution sol(n);
        for (int i = 0; i < n - 1; i ++) {
            int a, b, w;
            cin >> a >> b >> w;
            a--; b--;
            
            adj[a].push_back(b);
            adj[b].push_back(a);

            sol.adjList[a].push_back(Pair2(b, i));
            sol.adjList[b].push_back(Pair2(a, i));
            sol.edgeInfo.emplace_back(a, b, w);
        }
        sol.Lca = LCA(n, adj);
        sol.process();
        

        string type;
        while (1) {
            cin >> type;
           
            if (type == "DONE") break;
            else if (type == "CHANGE") {
                int i, ti;
                cin >> i >> ti;
                sol.update(i - 1, ti);
                
            } else {
                int a, b;
                cin >> a >> b;
                cout << sol.query(a - 1, b - 1) << '\n';
               
            }
        }
        
    }
}