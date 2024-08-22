#include <bits/stdc++.h>
using namespace std;

// idea of persistant segment trees
// we have m insertions, but we still want the segment tree before the kth insertions
// each insertion change logn nodes, so for each insertions, we reuse nodes from previous version as much as possible

// all indices are 1 indexed
// use persistant segment trees for O(logn) query
// for each i, eahc segment tree stores the number of nums of each 'type' after compression
// i.e i = 5, so segment tree 5 stores the processed result of prefix [0, 5]
// to query kth number [l, r] notice that segment tree r - segment tree l contains the segment tree of elems in [l, r]
// can easily find the kth number.
class Node {
public:
    // stores number of elems whose compressed result is inside [s. e]
    Node* left;
    Node* right;
    int s;
    int e;
    int sum;

    Node(int s, int e, int sum) : s(s), e(e), sum(sum), left(nullptr), right(nullptr) {};
};


int N; // number of distinct integers
unordered_map<int, int> valToIdx;
vector<Node*> segTrees;
vector<int> nums;
vector<int> sorted;



int process(vector<int>& nums) {
    sorted = nums;
    sort(sorted.begin(), sorted.end());
    for (int i = 0; i < sorted.size(); i ++) {
        if (valToIdx.find(sorted[i]) != valToIdx.end()) continue;
        valToIdx[sorted[i]] = i;
    }
    return valToIdx.size();
}

Node* build(int s, int e) {
    int m = (s + e) / 2;
    Node* out = new Node(s, e, 0);
    if (s == e) return out;
    
    out->left = build(s, m);
    out->right = build(m + 1, e);
    return out;
}


// everytime we do point insert, we create a 'new' segment tree
Node* insert(Node* node, int s, int e, int idx) {
    int m = (s + e) / 2;
    Node* out = new Node(s, e, 0);
    
    if (s == e) {
        out->sum = node->sum + 1;
    } else if (idx <= m) {
        out->left = insert(node->left, s, m, idx);
        out->right = node->right;
        out->sum = out->left->sum + out->right->sum;
    } else {
        out->left = node->left;
        out->right = insert(node->right, m + 1, e, idx);
        out->sum = out->left->sum + out->right->sum;
    }
    return out;
}

// k <= r - l + 1;
// n1->sum - n0->sum contains number of elems with compressed indices in [s, e] for subarray [l, r]
// hence no need for l, r
int query(Node* n0, Node* n1, int s, int e, int k) {
    if (s == e) return s;

    int m = (s + e) / 2;
    int leftLimit = n1->left->sum - n0->left->sum;
    if (k <= leftLimit) {
        return query(n0->left, n1->left, s, m, k);
    } else {
        return query(n0->right, n1->right, m + 1, e, k - leftLimit);
    }
}

int main() {
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; i ++) {
        int elem;
        cin >> elem;
        nums.push_back(elem);
    }

    N = process(nums);
    //for_each(sorted.begin(), sorted.end(), [] (int x) -> void {cout << x << " ";});
    segTrees.push_back(build(0, N - 1));
    for (int i = 0; i < nums.size(); i ++) {
        segTrees.push_back(insert(segTrees[i], 0, N - 1, valToIdx[nums[i]]));
    }

    for (int i = 0; i < m; i ++) {
        int l, r, k;
        cin >> l >> r >> k;
        int ans = query(segTrees[l - 1], segTrees[r], 0, N - 1, k);
        
        cout << sorted[ans] << endl;
    }

    
}