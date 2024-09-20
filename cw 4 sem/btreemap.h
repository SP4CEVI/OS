#include <iostream>
#include <vector>

#include "interface.h"
#include "customer.h"
#include "logger.h"

using namespace std;

const int ORDER_MAX = 128;

template <typename T, typename V>
struct TreeNodePair {
    T key;
    V* value;
};

template <typename T, typename V, int ORDER=ORDER_MAX>
class BTreeNode {
public:
    TreeNodePair<T, V> keys[ORDER - 1];
    BTreeNode* children[ORDER];
    int num; // number of keys
    bool leaf; // True - leaf node

    BTreeNode(bool isLeaf = true) : num(0), leaf(isLeaf) {
        for (int i = 0; i < ORDER; i++)
            children[i] = nullptr;
    }

};

template <typename T, typename V, int ORDER=ORDER_MAX>
class BTreeMap : public ICollection<T, V> {


private:
    BTreeNode<T, V, ORDER>* root;

    void split(BTreeNode<T, V, ORDER>* root, int i) {
        BTreeNode<T, V, ORDER>* node = root->children[i];
        BTreeNode<T, V, ORDER>* new_node = new BTreeNode<T, V, ORDER>(node->leaf);
        new_node->num = ORDER / 2 - 1;

        for (int j = 0; j < ORDER / 2 - 1; j++) {
            new_node->keys[j] = node->keys[j + ORDER / 2];
        }

        if (!node->leaf) {
            for (int j = 0; j < ORDER / 2; j++)
                new_node->children[j] = node->children[j + ORDER / 2];
        }

        node->num = ORDER / 2 - 1;

        for (int j = root->num; j >= i + 1; j--)
            root->children[j + 1] = root->children[j];

        root->children[i + 1] = new_node;

        for (int j = root->num - 1; j >= i; j--)
            root->keys[j + 1] = root->keys[j];

        root->keys[i] = node->keys[ORDER / 2 - 1];
        root->num = root->num + 1;
    }

    void insertNonFull(BTreeNode<T, V, ORDER>* node, T key, V* value) {
        int i = node->num - 1;

        if (node->leaf) {
            while (i >= 0 && key < node->keys[i].key) {
                node->keys[i + 1] = node->keys[i];
                i -= 1;
            }

            node->keys[i + 1].key = key;
            node->keys[i + 1].value = value;
            node->num = node->num + 1;
        }
        else {
            while (i >= 0 && key < node->keys[i].key) i -= 1;

            i += 1;
            if (node->children[i]->num == ORDER - 1) {
                split(node, i);
                if (key > node->keys[i].key)
                    i += 1;
            }
            insertNonFull(node->children[i], key, value);
        }
    }

    void in_order(BTreeNode<T, V, ORDER>* node) {
        int i;
        for (i = 0; i < node->num; i++) {
            if (!node->leaf)
                in_order(node->children[i]);
            cout << " " << node->keys[i];
        }

        if (!node->leaf)
            in_order(node->children[i]);
    }

    BTreeNode<T, V, ORDER>* searchRecursive(BTreeNode<T, V, ORDER>* node, T key) {
        int i = 0;
        while (i < node->num && key > node->keys[i].key)
            i += 1;
        if (i < node->num && key == node->keys[i].key)
            return node;
        if (node->leaf)
            return nullptr;
        return searchRecursive(node->children[i], key);
    }

    T getPredecessor(BTreeNode<T, V, ORDER>* node, int idx) {
        BTreeNode<T, V, ORDER>* current = node->children[idx];
        while (!current->leaf)
            current = current->children[current->num];
        return current->keys[current->num - 1].key;
    }

    T getSuccessor(BTreeNode<T, V, ORDER>* node, int idx) {
        BTreeNode<T, V, ORDER>* current = node->children[idx + 1];
        while (!current->leaf)
            current = current->children[0];
        return current->keys[0].key;
    }
    
    void removeFromNonLeaf(BTreeNode<T, V, ORDER>* node, int idx) {
        T key = node->keys[idx].key;

        if (node->children[idx]->num >= ORDER / 2) {
            T pred = getPredecessor(node, idx);
            node->keys[idx].key = pred;
            doRemove(node->children[idx], pred);
        }
        else if (node->children[idx + 1]->num >= ORDER / 2) {
            T succ = getSuccessor(node, idx);
            node->keys[idx].key = succ;
            doRemove(node->children[idx + 1], succ);
        }
        else {
            merge(node, idx);
            doRemove(node->children[idx], key);
        }
    }

    void removeFromLeaf(BTreeNode<T, V, ORDER>* node, int idx) {
        for (int i = idx + 1; i < node->num; ++i)
            node->keys[i - 1] = node->keys[i];

        node->num -= 1;
    }

    void fix(BTreeNode<T, V, ORDER>* node, int idx) {
        if (idx != 0 && node->children[idx - 1]->num >= ORDER / 2)
            borrowFromPrev(node, idx);
        else if (idx != node->num && node->children[idx + 1]->num >= ORDER / 2)
            borrowFromNext(node, idx);
        else{
            if (idx != node->num) merge(node, idx);
            else merge(node, idx - 1);
        }
    }

    void borrowFromPrev(BTreeNode<T, V, ORDER>* node, int idx) {
        BTreeNode<T, V, ORDER>* child = node->children[idx];
        BTreeNode<T, V, ORDER>* prevChild = node->children[idx - 1];

        for (int i = child->num - 1; i >= 0; --i)
            child->keys[i + 1] = child->keys[i];

        if (!child->leaf) {
            for (int i = child->num; i >= 0; --i)
                child->children[i + 1] = child->children[i];
        }

        child->keys[0] = node->keys[idx - 1];

        if (!child->leaf) child->children[0] = prevChild->children[prevChild->num];

        node->keys[idx - 1] = prevChild->keys[prevChild->num - 1];

        child->num += 1;
        prevChild->num -= 1;
    }

    void borrowFromNext(BTreeNode<T, V, ORDER>* node, int idx) {
        BTreeNode<T, V, ORDER>* child = node->children[idx];
        BTreeNode<T, V, ORDER>* nextChild = node->children[idx + 1];

        child->keys[child->num] = node->keys[idx];

        if (!child->leaf) child->children[child->num + 1] = nextChild->children[0];

        node->keys[idx] = nextChild->keys[0];

        for (int i = 1; i < nextChild->num; ++i) nextChild->keys[i - 1] = nextChild->keys[i];

        if (!nextChild->leaf) {
            for (int i = 1; i <= nextChild->num; ++i) nextChild->children[i - 1] = nextChild->children[i];
        }

        child->num += 1;
        nextChild->num -= 1;
    }

    void merge(BTreeNode<T, V, ORDER>* node, int idx) {
        BTreeNode<T, V, ORDER>* child = node->children[idx];
        BTreeNode<T, V, ORDER>* nextChild = node->children[idx + 1];

        child->keys[ORDER / 2 - 1] = node->keys[idx];

        for (int i = 0; i < nextChild->num; ++i) child->keys[i + ORDER / 2] = nextChild->keys[i];

        if (!child->leaf) {
            for (int i = 0; i <= nextChild->num; ++i) child->children[i + ORDER / 2] = nextChild->children[i];
        }

        for (int i = idx + 1; i < node->num; ++i) node->keys[i - 1] = node->keys[i];

        for (int i = idx + 2; i <= node->num; ++i) node->children[i - 1] = node->children[i];

        child->num += nextChild->num + 1;
        node->num -= 1;

        if (nextChild != nullptr)
            delete nextChild;
    }
    
    bool doRemove(BTreeNode<T, V, ORDER>* node, T key) {
        int idx = 0;
        while (idx < node->num && node->keys[idx].key < key)
            ++idx;

        if (idx < node->num && node->keys[idx].key == key) {
            if (node->leaf)
                removeFromLeaf(node, idx);
            else
                removeFromNonLeaf(node, idx);
        } else {
            if (node->leaf) {
                return false;
            }

            bool flag = idx == node->num;

            if (node->children[idx]->num < ORDER / 2)
                fix(node, idx);

            if (flag && idx > node->num)
                doRemove(node->children[idx - 1], key);
            else
                doRemove(node->children[idx], key);
        }
        return true;
    }

    BTreeNode<T, V, ORDER>* search(T key) {
        if (root == nullptr)
            return nullptr;
        else
            return searchRecursive(root, key);
    }

    void rangeSearchRecursive(BTreeNode<T, V, ORDER>* node, T minbound, T maxbound, vector<V*>& result) {
        int i = 0;
        while (i < node->num && node->keys[i].key < minbound) {
            i += 1;
        }

        if (node->leaf) {
            while (i < node->num && node->keys[i].key <= maxbound) {
                result.push_back(node->keys[i].value);
                i++;
            }
        }
        else {
            while (i < node->num && node->keys[i].key <= maxbound) {
                if (node->keys[i].key >= minbound) {
                    result.push_back(node->keys[i].value);
                }

                rangeSearchRecursive(node->children[i], minbound, maxbound, result);
                i += 1;
            }
            rangeSearchRecursive(node->children[i], minbound, maxbound, result);
        }
    }

    void in_order() {
        if (root != nullptr) in_order(root);
    }


    void GetKeys(BTreeNode<T, V, ORDER>* x, vector<T>& r) {
        int i;
        for (i = 0; i < x->num; i++) {
            if (!x->leaf)
                GetKeys(x->children[i], r);
            r.push_back(x->keys[i].key);
        }

        if (!x->leaf)
            GetKeys(x->children[i], r);
    }

public:
    BTreeMap() {
        root = new BTreeNode<T, V, ORDER>(true);
    }

    ~BTreeMap() {
        if (root != nullptr)
            delete root;
    }

    void Add(T key, V* value) {
        if (root->num == ORDER - 1) {
            BTreeNode<T, V, ORDER>* s = new BTreeNode<T, V, ORDER>(false);
            s->children[0] = root;
            root = s;
            split(s, 0);
            insertNonFull(s, key, value);
        }
        else
            insertNonFull(root, key, value);
    }

    bool Get(T key, V& value) {
        BTreeNode<T, V, ORDER> *node = searchRecursive(root, key);

        if (node == nullptr) {
            return false;
        }
        int i = 0;
        while (i < node->num && key > node->keys[i].key)
            i += 1;
        value = *node->keys[i].value;
        return true;
    }

    void Get(T minbound, T maxbound, vector<V*>& result) {
        rangeSearchRecursive(root, minbound, maxbound, result);
    }

    bool Update(T key, V* value) {
        BTreeNode<T, V, ORDER> *node = searchRecursive(root, key);
        if (node == nullptr) {
            return false;
        }
        int i = 0;
        while (i < node->num && key > node->keys[i].key)
            i += 1;
        node->keys[i].value = value;
        return true;
    }

    bool Remove(T key) {
        if (!root) {
            return false;
        }

        bool res = doRemove(root, key);

        if (root->num == 0) {
            BTreeNode<T, V, ORDER> *tmp = root;
            if (root->leaf)
                root = nullptr;
            else
                root = root->children[0];

            if (tmp != nullptr)
                delete tmp;
        }

        return res;
    }


    vector<T> Keys() {
        vector<T> res;
        if (root != nullptr)
            GetKeys(root, res);
        return res;
    }
};
