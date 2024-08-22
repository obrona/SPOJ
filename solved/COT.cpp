#include <bits/stdc++.h>
using namespace std;

// persistant segement tree and coordinate compression
// segmentTrees[node] return segment tree containing the compressed indices of values of nodes
// so segtree[s, e] contains num of elems with compressed indices in [s, e], can easily see
// how such a segtree can find kth min num in O(logn)
// from root to node
// to get Kth weight, we need the segment tree of the path from u to v
// which is simply segmentTrees[u] + segmentTrees[v] - segmentTree(lca(u, v)) - segmentTree(parent(lca(u, v)))

class Node {
public:
    int s;
    int e;
    int sum;
    Node* left;
    Node* right;

    Node(int s, int e, int sum): s(s), e(e), sum(sum), left(nullptr), right(nullptr) {};
};

class LCA {
public:
    int MAX = 17; 
    vector<vector<int>> binLift;
    vector<int> depth;

    LCA() {};

    LCA(int n, vector<vector<int>> adj): binLift(n, vector<int>(MAX, -1)), depth(n, 0) {
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
        for (int i = 0; i <= MAX; i ++) {
            if (jumps & 1 && binLift[v][i] != -1) v = binLift[v][i];
            jumps = jumps >> 1;
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

int N;
unordered_map<int, int> valToIdx;
vector<int> parents;
vector<int> weights;
vector<int> sorted;
vector<vector<int>> adj;
LCA lca;
vector<Node*> segTrees; 



void processNums() {
    set<int> temp(weights.begin(), weights.end());
    sorted = vector<int>(temp.begin(), temp.end());
    int idx = 0;
    for (int i = 0; i < sorted.size(); i ++) {
       valToIdx[sorted[i]] = i;
    }
    N = valToIdx.size();
}

Node* build(int s, int e) {
    int m = (s + e) / 2;
    Node* out = new Node(s, e, 0);
    if (s == e) return out;

    out->left = build(s, m);
    out->right = build(m + 1, e);
    return out;   
}

Node* insert(Node* parent, int s, int e, int idx) {
    int m = (s + e) / 2;
    Node* out = new Node(s, e, 0);
    if (s == e) {
        out->sum = parent->sum + 1;
    } else if (idx <= m) {
        out->left = insert(parent->left, s, m, idx);
        out->right = parent->right;
        out->sum = out->left->sum + out->right->sum;
    } else {
        out->left = parent->left;
        out->right = insert(parent->right, m + 1, e, idx);
        out->sum = out->left->sum + out->right->sum;
    }
    return out;
}

void dfs(int v, int p) {
    parents[v] = p;
    
    if (v == p) {
        segTrees[v] = build(0, N - 1);
    } else {
        segTrees[v] = insert(segTrees[p], 0, N - 1, valToIdx[weights[v]]);
    }
    
    for (int c : adj[v]) {
        if (c != p) dfs(c, v);
    }
}

int findKth(Node* u, Node* v, Node* lca, Node* plca, int s, int e, int k) {
    int m = (s + e) / 2;
    if (s == e) return s;

    int leftLim = u->left->sum + v->left->sum - lca->left->sum - plca->left->sum;
    if (k <= leftLim) return findKth(u->left, v->left, lca->left, plca->left, s, m, k);
    else return findKth(u->right, v->right, lca->right, plca->right, m + 1, e, k - leftLim);
}

int query(int u, int v, int k) {
    int lcca = lca.getLca(u, v);
    int plcca = parents[lcca];
    int idx = findKth(segTrees[u], segTrees[v], segTrees[lcca], segTrees[plcca], 0, N - 1, k);
    //cout << lcca << " " << plcca << " " << idx << endl;
    return sorted[idx];
}





int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    adj = vector<vector<int>>(n + 1, vector<int>());
    parents = vector<int>(n + 1, 0);
    weights = vector<int>(n + 1, (int) -2e9);
    segTrees = vector<Node*>(n + 1, nullptr);
    
    for (int i = 0; i < n; i ++) {
        int w;
        cin >> w;
        weights[i + 1] = w;
    }
    processNums();
    //for_each(sorted.begin(), sorted.end(), [] (int x) -> void {cout << x << " ";}); cout << endl;

    // add 0 as dummy root
    adj[0].push_back(1);
    adj[1].push_back(0);
    for (int i = 0; i < n - 1; i ++) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    lca = LCA(n + 1, adj);
    dfs(0, 0);

    for (int i = 0; i < m; i ++) {
        int u, v, k;
        cin >> u >> v >> k;
        cout << query(u, v, k) << '\n';
    }
}
