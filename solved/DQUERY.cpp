#include <bits/stdc++.h>
using namespace std;

// first we process array s.t processed[i] contains j, the earliest index to the right
// s.t arr[i] == arr[j]
// then for [l, r] suppose we already have aa segment tree on the for the result on subarray[l, r] after processing
// we simply need to subtract from r - l + 1, the count of res with processed <= r
// we can do this in logn if segment tree stores for each value of res in [0, n + 2], the number of elem with that res
// when we have processed[i] <= r, means that arr[i] has a same element in the range [l, r] that has to be subtracted
// so the count of elems in segtree <= r == count of repeated elems
// we use the idea of persistant segment trees, each segment tree contains the seg tree for the prefix [0, i]
// to get segtree for [l, r], simply take segtree[r] - segtree[l - 1]

struct Node {
    int s;
    int e;
    int sum;
    Node* l;
    Node* r;

    Node(int s, int e, int sum): s(s), e(e), sum(sum), l(nullptr), r(nullptr) {};
};

int N; // here segtree needs to take values 0 to N + 1
vector<int> arr;
vector<int> processed;
vector<Node*> segTrees;

// arr is 1 indexed
void processArray() {
    unordered_map<int, int> store;
    for (int i = arr.size() - 1; i >= 0; i --) {
        if (store.find(arr[i]) == store.end()) processed[i] = N + 1;
        else processed[i] = store[arr[i]];
        
        store[arr[i]] = i;
    }
}

Node* buildEmpty(int s, int e) {
    Node* out = new Node(s, e, 0);
    if (s == e) return out;

    int m = (s + e) / 2;
    out->l = buildEmpty(s, m);
    out->r = buildEmpty(m + 1, e);
    return out;
}

// s <= val <= e
Node* insert(Node* p, int s, int e, int val) {
    Node* out = new Node(s, e, 0);
    int m = (s + e) / 2;
    if (s == e) {
        out->sum = p->sum + 1;
    } else if (val <= m) {
        out->l = insert(p->l, s, m, val);
        out->r = p->r;
        out->sum = out->l->sum + out->r->sum;
    } else {
        out->l = p->l;
        out->r = insert(p->r, m + 1, e, val);
        out->sum = out->l->sum + out->r->sum;
    }
    return out;
}

// s <= r <= e
int findRepeated(Node* left, Node* right, int s, int e, int r) {
    if (s == e) {
        return right->sum - left->sum;
    }

    int m = (s + e) / 2;
    int leftSum = right->l->sum - left->l->sum;
    if (r <= m) {
        return findRepeated(left->l, right->l, s, m, r);
    } else {
        return leftSum + findRepeated(left->r, right->r, m + 1, e, r);
    }
}

int query(int l, int r) {
    return (r - l + 1) - findRepeated(segTrees[l - 1], segTrees[r], 0, N + 1, r);
}

int main() {
    int n;
    cin >> n;
    N = n;
    arr = vector<int>(n + 1, 0);
    processed = vector<int>(n + 1, 0);
    segTrees = vector<Node*>(n + 1, nullptr);
    for (int i = 1; i <= n; i ++) {
        int x;
        cin >> x;
        arr[i] = x;
    } 
    
    processArray();
    segTrees[0] = buildEmpty(0, N + 1);
    for (int i = 1; i <= n; i ++) {
        segTrees[i] = insert(segTrees[i - 1], 0, N + 1, processed[i]);
    }

    int q;
    cin >> q;
    for (int i = 0; i < q; i ++) {
        int l, r;
        cin >> l >> r;
        cout << query(l, r) << '\n';
    }
    
}


