#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <utility>

namespace aisdi
{

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
    class Node;
    using node = Node;

private:
    node *sentinel, *root;
    bool inOrderSuccessorRecentlyUsed = false; //variable used for choosing different variants of remove function

public:
    TreeMap(): sentinel(new node), root(sentinel)
    {}

    TreeMap(std::initializer_list<value_type> list): TreeMap()
    {
        for(value_type val : list)
            (*this)[val.first] = val.second;
    }

    TreeMap(const TreeMap& other): TreeMap()
    {
        if(other.isEmpty()) return;
        else copy_tree(other.root, other.sentinel);
    }

    TreeMap(TreeMap&& other): TreeMap()
    {
        move_tree(other);
    }

    ~TreeMap()
    {
        empty_tree(root);
        root = nullptr;
        delete sentinel;
        sentinel = nullptr;
    }

    TreeMap& operator=(const TreeMap& other)
    {
        if(&other != this)
        {
            empty_tree(root);
            sentinel->parent = nullptr;
            root = sentinel;
            copy_tree(other.root, other.sentinel);
        }
        return *this;
    }

    TreeMap& operator=(TreeMap&& other)
    {
        if(&other != this)
        {
            empty_tree(root);
            sentinel->parent = nullptr;
            move_tree(other);
        }
        return *this;
    }

    //Delete all nodes from the given subtree except the sentinel
    void empty_tree(node* nd)
    {
        if(nd == nullptr || nd == sentinel) return;
        empty_tree(nd->left);
        empty_tree(nd->right);
        delete nd;
    }

    //Copy all nodes from the given subtree except the sentinel
    void copy_tree(node* nd, node* sentinel)
    {
        if(nd == nullptr || nd == sentinel) return;
        (*this)[nd->val.first] = nd->val.second;
        copy_tree(nd->left, sentinel);
        copy_tree(nd->right, sentinel);
    }

    //Move tree nodes from *other* to *this* tree
    void move_tree(TreeMap &other)
    {
        //Remember the position of newly created sentinel node
        node* temp = sentinel;

        //Copy other's pointers
        root = other.root;
        sentinel = other.sentinel;

        //Make *other* an empty tree with one sentinel node
        other.root = other.sentinel = temp;
    }

    bool isEmpty() const
    {
        return (root == sentinel);
    }

    //Return a node with the given key or create a new one
    node* getNode(node *&nd, const key_type &key, node *parent, node *&requested)
    {
        //Node with given key doesn't exist, create it
        if(nd == nullptr)
        {
            nd = new node(key, parent);
            requested = nd;
        }
        /*Currently visited node is the sentinel, so the node we are looking
          for doesn't exist -> create it and insert before sentinel*/
        else if(nd == sentinel)
        {
            nd = new node(key, parent);
            nd->right = sentinel;
            sentinel->parent = nd;
            requested = nd;
        }
        //Requested node has smaller key than currently visited one
        else if(key < nd->val.first)
            nd->left = getNode(nd->left, key, nd, requested);
        //Requested node has greater key than currently visited one
        else if(key > nd->val.first)
            nd->right = getNode(nd->right, key, nd, requested);
        //Currently visited node has the right key
        else requested = nd;

        return nd;
    }

    mapped_type& operator[](const key_type &key)
    {
        bool wasEmpty = false;
        node *temp;
        if(isEmpty()) wasEmpty = true;
        getNode(root, key, nullptr, temp);
        if(wasEmpty) root = temp;
        return temp->val.second;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        node* temp = search(root, key);
        if(temp == nullptr) throw std::out_of_range("Node with given key doesn't exist");
        return temp->val.second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        node* temp = search(root, key);
        if(temp == nullptr) throw std::out_of_range("Node with given key doesn't exist");
        return temp->val.second;
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
        const_iterator it = find(key);
        remove(it);
    }

    void remove(const const_iterator& it)
    {
        if(it == end()) throw std::out_of_range("Node with given key doesn't exist or iterator is in end position");
        node* temp = it.getNode();
        bool isNotRoot = (temp != root);
        bool isRightChild = isNotRoot ? (temp->parent->right == temp) : false;
        if(temp->left == nullptr)
        {
            if(temp->right == nullptr) //no children (*temp* can't be root)
            {
                if(isRightChild) temp->parent->right = nullptr;
                else temp->parent->left = nullptr;
                delete temp;
            }
            else //one child on the right (can be sentinel node)
            {
                if(isNotRoot)
                {
                    if(isRightChild) temp->parent->right = temp->right;
                    else temp->parent->left = temp->right;
                }
                else root = temp->right;

                temp->right->parent = temp->parent; //this line is ok even if *temp* is root,
                                                    //because temp->parent is nullptr in that case
                delete temp;
            }
        }
        else //(temp->left != nullptr)
        {
            if(temp->right == nullptr) //one child on the left (*temp* can't be root)
            {
                temp->left->parent = temp->parent;
                if(isRightChild) temp->parent->right = temp->left;
                else temp->parent->left = temp->left;
                delete temp;
            }
            else if(temp->right == sentinel) //two children, sentinel on the right (*temp* is root)
            {
                node* nd = temp->left;
                while(nd->right != nullptr)
                    nd = nd->right;
                nd->right = sentinel;
                sentinel->parent = nd;

                temp->left->parent = nullptr;
                root = temp->left;

                delete temp;
            }
            else //two children (*temp* can't be root)
            {
                node* nd;
                if(inOrderSuccessorRecentlyUsed) //use in-order predecessor this time
                {
                    nd = temp->left;
                    while(nd->right != nullptr)
                        nd = nd->right;

                    nd->parent->right = nd->left;
                    if(nd->left != nullptr)
                        nd->left->parent = nd->parent;

                    inOrderSuccessorRecentlyUsed = false;
                }
                else //use in-order successor
                {
                    nd = temp->right;
                    while(nd->left != nullptr)
                        nd = nd->left;

                    nd->parent->left = nd->right;
                    if(nd->right != nullptr)
                        nd->right->parent = nd->parent;

                    inOrderSuccessorRecentlyUsed = true;
                }

                nd->left = temp->left;
                if(temp->left != nullptr)
                    temp->left->parent = nd;
                nd->right = temp->right;
                if(temp->right != nullptr)
                    temp->right->parent = nd;
                nd->parent = temp->parent;
                if(isNotRoot)
                {
                    if(isRightChild) temp->parent->right = nd;
                    else temp->parent->left = nd;
                }
                else root = nd;
                delete temp;
            }
        }
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
        if(getSize() != other.getSize()) return false;

        for(auto it1 = begin(), it2 = other.begin(); it1 != end(); ++it1, ++it2)
            if(*it1 != *it2) return false;
        return true;
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
class TreeMap<KeyType, ValueType>::Node
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

    ~Node()
    {
        parent = left = right = nullptr;
    }

};


template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    friend class TreeMap<KeyType, ValueType>;
    using reference = typename TreeMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename TreeMap::value_type;
    using pointer = const typename TreeMap::value_type*;
    using node = TreeMap<KeyType, ValueType>::Node;
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

private:
    node* getNode() const
    {
        return current;
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    friend class TreeMap<KeyType, ValueType>;
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
