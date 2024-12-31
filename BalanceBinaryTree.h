#pragma once
#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"
#include <stdexcept>

// Узел дерева
template <typename Key, typename Value>
struct TreeNode {
    Pair<Key, Value> pair;
    TreeNode* left;
    TreeNode* right;
    int height;

    TreeNode(const Key& k, const Value& v)
        : pair(k, v), left(nullptr), right(nullptr), height(1) {}
};

template <typename Key, typename Value>
class BalanceBinaryTree : public IDictionary<Key, Value> {
private:
    TreeNode<Key, Value>* root;

    // Вспомогательные функции для работы с AVL-деревом
    int getHeight(TreeNode<Key, Value>* node) const {
        return node ? node->height : 0;
    }

    int getBalanceFactor(TreeNode<Key, Value>* node) const {
        return getHeight(node->left) - getHeight(node->right);
    }

    void updateHeight(TreeNode<Key, Value>* node) {
        int hl = getHeight(node->left);
        int hr = getHeight(node->right);
        node->height = (hl > hr ? hl : hr) + 1;
    }

    TreeNode<Key, Value>* rotateRight(TreeNode<Key, Value>* y) {
        TreeNode<Key, Value>* x = y->left;
        TreeNode<Key, Value>* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    TreeNode<Key, Value>* rotateLeft(TreeNode<Key, Value>* x) {
        TreeNode<Key, Value>* y = x->right;
        TreeNode<Key, Value>* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    TreeNode<Key, Value>* insertNode(TreeNode<Key, Value>* node, const Key& key, const Value& value) {
        if (!node) return new TreeNode<Key, Value>(key, value);

        if (key < node->pair.key) {
            node->left = insertNode(node->left, key, value);
        }
        else if (key > node->pair.key) {
            node->right = insertNode(node->right, key, value);
        }
        else {
            // Ключ уже существует — обновляем значение
            node->pair.value = value;
            return node;
        }

        updateHeight(node);

        int balance = getBalanceFactor(node);

        // LL
        if (balance > 1 && key < node->left->pair.key)
            return rotateRight(node);

        // RR
        if (balance < -1 && key > node->right->pair.key)
            return rotateLeft(node);

        // LR
        if (balance > 1 && key > node->left->pair.key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // RL
        if (balance < -1 && key < node->right->pair.key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    bool existNode(TreeNode<Key, Value>* node, const Key& key) const {
        if (!node) return false;
        if (key < node->pair.key) return existNode(node->left, key);
        else if (key > node->pair.key) return existNode(node->right, key);
        else return true; // key == node->pair.key
    }

    Value getNode(TreeNode<Key, Value>* node, const Key& key) const {
        if (!node) throw std::runtime_error("Key not found");
        if (key < node->pair.key) return getNode(node->left, key);
        else if (key > node->pair.key) return getNode(node->right, key);
        else return node->pair.value; // key == node->pair.key
    }

    TreeNode<Key, Value>* findMin(TreeNode<Key, Value>* node) const {
        return node->left ? findMin(node->left) : node;
    }

    TreeNode<Key, Value>* removeMin(TreeNode<Key, Value>* node) {
        if (!node->left) return node->right;
        node->left = removeMin(node->left);
        updateHeight(node);
        return node;
    }

    TreeNode<Key, Value>* removeNode(TreeNode<Key, Value>* node, const Key& key, bool& success) {
        if (!node) {
            success = false;
            return nullptr;
        }

        if (key < node->pair.key) {
            node->left = removeNode(node->left, key, success);
        }
        else if (key > node->pair.key) {
            node->right = removeNode(node->right, key, success);
        }
        else {
            // Ключ найден
            success = true;
            TreeNode<Key, Value>* left = node->left;
            TreeNode<Key, Value>* right = node->right;
            delete node;

            if (!right) return left;

            TreeNode<Key, Value>* minNode = findMin(right);
            minNode->right = removeMin(right);
            minNode->left = left;
            updateHeight(minNode);

            // Балансируем
            int balance = getBalanceFactor(minNode);
            if (balance > 1 && getBalanceFactor(minNode->left) >= 0)
                return rotateRight(minNode);
            if (balance > 1 && getBalanceFactor(minNode->left) < 0) {
                minNode->left = rotateLeft(minNode->left);
                return rotateRight(minNode);
            }
            if (balance < -1 && getBalanceFactor(minNode->right) <= 0)
                return rotateLeft(minNode);
            if (balance < -1 && getBalanceFactor(minNode->right) > 0) {
                minNode->right = rotateRight(minNode->right);
                return rotateLeft(minNode);
            }

            return minNode;
        }

        updateHeight(node);

        // Балансируем
        int balance = getBalanceFactor(node);
        if (balance > 1 && getBalanceFactor(node->left) >= 0)
            return rotateRight(node);
        if (balance > 1 && getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && getBalanceFactor(node->right) <= 0)
            return rotateLeft(node);
        if (balance < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void fillArray(TreeNode<Key, Value>* node, DynamicArray<Pair<Key, Value>>& arr) const {
        if (!node) return;
        fillArray(node->left, arr);
        arr.Append(node->pair);
        fillArray(node->right, arr);
    }

void clear(TreeNode<Key, Value>* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

public:
    BalanceBinaryTree() : root(nullptr) {}

    ~BalanceBinaryTree() {
        clear(root);
    }

    void insert(const Key& key, const Value& value) override {
        root = insertNode(root, key, value);
    }

    bool exist(const Key& key) const override {
        return existNode(root, key);
    }

    Value get(const Key& key) const override {
        return getNode(root, key);
    }

    bool remove(const Key& key) override {
        bool success = false;
        root = removeNode(root, key, success);
        return success;
    }

    void getAllPairs(DynamicArray<Pair<Key, Value>>& arr) const override {
        fillArray(root, arr);
    }
};
