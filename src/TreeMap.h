#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{
template <typename KeyType, typename ValueType>
class Node;

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using node = Node<KeyType, ValueType>;

private:
    node *sentinel, *root;

public:
    TreeMap(): sentinel(new node), root(sentinel)
    {}

    TreeMap(std::initializer_list<value_type> list)
    {
        for(value_type val : list)
            getNode(val.first) = val.second;
    }

    void copy_tree(node* nd, node* sentinel)
    {
        if(nd == nullptr || nd == sentinel) return;
        getNode(nd->val.first) = nd->val.second;
        copy_tree(nd->left, sentinel);
        copy_tree(nd->right, sentinel);
    }

    TreeMap(const TreeMap& other): TreeMap()
    {
        if(other.isEmpty()) return;
        else copy_tree(other.root, other.sentinel);
    }

    TreeMap(TreeMap&& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    TreeMap& operator=(const TreeMap& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    TreeMap& operator=(TreeMap&& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    bool isEmpty() const
    {
        return root == sentinel;
    }

    node* insert(node *&nd, const key_type &key, node *parent, node *&inserted)
    {
        if(nd == nullptr)
        {
            nd = new node(key, parent);
            inserted = nd;
        }
        else if(nd == sentinel)
        {
            nd = new node(key, parent);
            nd->right = sentinel;
            sentinel->parent = nd;
            inserted = nd;
        }
        else if(key < nd->val.first)
            nd->left = insert(nd->left, key, nd, inserted);
        else if(key > nd->val.first)
            nd->right = insert(nd->right, key, nd, inserted);
        else inserted = nd;
        return nd;
    }

    mapped_type& getNode(const key_type &key)
    {
        bool wasEmpty = false;
        node *temp;
        if(isEmpty()) wasEmpty = true;
        insert(root, key, nullptr, temp);
        if(wasEmpty) root = temp;
        return temp->val.second;
    }

    mapped_type& operator[](const key_type &key)
    {
        return getNode(key);
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    mapped_type& valueOf(const key_type& key)
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    node* search(node *root, const key_type& key) const
    {
        if(root == nullptr || key == root->val.first)
            return root;
        else if(key < root->val.first)
            return search(root->left, key);
        else return search(root->right, key);
    }

    const_iterator find(const key_type& key) const
    {
        if(isEmpty()) return cend();
        node *temp = search(root, key);
        if(temp == nullptr) return cend();
        else return const_iterator(this, temp);
    }

    iterator find(const key_type& key)
    {
        if(isEmpty()) return end();
        node *temp = search(root, key);
        if(temp == nullptr) return end();
        else return iterator(const_iterator(this, temp));
    }

    void remove(const key_type& key)
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    void remove(const const_iterator& it)
    {
        (void)it;
        throw std::runtime_error("TODO");
    }

    void size(node* nd, size_type &s) const
    {
        if(nd == nullptr || nd == sentinel) return;
        size(nd->left, s);
        ++s;
        size(nd->right, s);
    }

    size_type getSize() const
    {
        size_type temp = 0;
        size(root, temp);
        return temp;
    }

    bool operator==(const TreeMap& other) const
    {
        (void)other;
        throw std::runtime_error("TODO==map");
    }

    bool operator!=(const TreeMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        return iterator(cbegin());
    }

    iterator end()
    {
        return iterator(cend());
    }

    const_iterator cbegin() const
    {
        node *temp = root;
        while(temp->left != nullptr)
            temp = temp->left;
        return const_iterator(this, temp);
    }

    const_iterator cend() const
    {

        return const_iterator(this, sentinel);
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }
};

template <typename KeyType, typename ValueType>
class Node
{
public:
    friend class TreeMap<KeyType, ValueType>;
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;

private:
    Node *parent, *left, *right;
    value_type val;

public:
    Node(Node *p=nullptr): parent(p), left(nullptr), right(nullptr)
    {}

    Node(key_type key, Node *p=nullptr): parent(p), left(nullptr), right(nullptr), val(value_type(key, mapped_type()))
    {}

    Node(value_type v, Node *p): parent(p), left(nullptr), right(nullptr), val(v)
    {}

//    Node(const Node &other): parent(other.parent), left(other.left), right(other.right)
//    {}
};


template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename TreeMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename TreeMap::value_type;
    using pointer = const typename TreeMap::value_type*;
    using node = Node<KeyType, ValueType>;
    using tree_map = TreeMap<KeyType, ValueType>;

private:
    node *current;
    const tree_map* parent_tree;

public:
    explicit ConstIterator(const tree_map* parent, node *n): current(n), parent_tree(parent)
    {}

    ConstIterator(const ConstIterator& other): current(other.current), parent_tree(other.parent_tree)
    {}

    ConstIterator& operator++()
    {
        if(*this==parent_tree->end())
        {
            throw std::out_of_range("Cannot increment iterator");
            return *this;
        }

        node *temp;
        if(current->right != nullptr)
        {
            temp = current->right;
            while(temp->left != nullptr)
                temp = temp->left;
            current = temp;
        }
        else if(current->parent != nullptr)
        {
            temp = current;
            while(temp->parent != nullptr && temp->parent->right == temp)
                temp = temp->parent;
            if(temp->parent != nullptr)
                current = temp->parent;
            else throw std::out_of_range("Cannot increment iterator");
        }
        else throw std::out_of_range("Cannot increment iterator");

        return *this;
    }

    ConstIterator operator++(int)
    {
        ConstIterator temp = *this;
        ++(*this);
        return temp;
    }

    ConstIterator& operator--()
    {
        if(*this==parent_tree->begin())
        {
            throw std::out_of_range("Cannot decrement iterator");
            return *this;
        }

        node *temp;
        if(current->left != nullptr)
        {
            temp = current->left;
            while(temp->right != nullptr)
                temp = temp->right;
            current = temp;
        }
        else if(current->parent != nullptr)
        {
            temp = current;
            while(temp->parent != nullptr && temp->parent->left == temp)
                temp = temp->parent;
            if(temp->parent != nullptr)
                current = temp->parent;
            else throw std::out_of_range("Cannot increment iterator");
        }
        else throw std::out_of_range("Cannot increment iterator");

        return *this;
    }

    ConstIterator operator--(int)
    {
        ConstIterator temp = *this;
        --(*this);
        return temp;
    }

    reference operator*() const
    {
        if(*this == parent_tree->end()) throw std::out_of_range("Iterator points at empty space after the last element");
        return current->val;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==(const ConstIterator& other) const
    {
        return current == other.current;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename TreeMap::reference;
    using pointer = typename TreeMap::value_type*;

    explicit Iterator()
    {}

    Iterator(const ConstIterator& other): ConstIterator(other)
    {}

    Iterator& operator++()
    {
        ConstIterator::operator++();
        return *this;
    }

    Iterator operator++(int)
    {
        auto result = *this;
        ConstIterator::operator++();
        return result;
    }

    Iterator& operator--()
    {
        ConstIterator::operator--();
        return *this;
    }

    Iterator operator--(int)
    {
        auto result = *this;
        ConstIterator::operator--();
        return result;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    reference operator*() const
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
    }
};

}

#endif /* AISDI_MAPS_MAP_H */
