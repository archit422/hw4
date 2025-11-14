#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    using BST = BinarySearchTree<Key, Value>;

     
    // Add helper functions here

protected:  
    
    void rebalance_insert(AVLNode<Key, Value>* prev_node,AVLNode<Key, Value>* next_node);
    void rebalance_remove(AVLNode<Key, Value>* prev_node,bool removed);

    void rotateLeft(AVLNode<Key, Value>* l);
    void rotateRight(AVLNode<Key, Value>* r);

    AVLNode<Key, Value>* rebalance_helper(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rebalance_insert_helper(AVLNode<Key, Value>* node);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    if(this->root_ == nullptr )
    {
        this -> root_ = new AVLNode<Key, Value>(new_item.first,new_item.second,nullptr);
        return;
    }

    AVLNode<Key, Value>* current_node = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* prev_node = nullptr;

    while(current_node)
    {
      prev_node = current_node;

      if(new_item.first< current_node->getKey())
      {
        current_node = current_node->getLeft();
      }
      else if(new_item.first > current_node->getKey())
      {
        current_node = current_node->getRight();
      }
      else 
      {
        current_node->setValue(new_item.second);
        return;
      }
    }

    AVLNode<Key, Value>* new_node = new AVLNode<Key, Value>(new_item.first,new_item.second,static_cast<AVLNode<Key, Value>*>(prev_node));

    if(new_item.first < prev_node->getKey())
    {
      prev_node->setLeft(new_node);
    }
    else
    {
      prev_node->setRight(new_node);
    }

    rebalance_insert(prev_node,new_node);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* to_delete = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(to_delete == nullptr)
    {
      return; 
    }

    if(to_delete->getLeft() != nullptr && to_delete->getRight() != nullptr)
    {
      AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(BST::predecessor(to_delete));
      nodeSwap(to_delete,predecessor);
    }

    AVLNode<Key, Value>* prev_node = to_delete->getParent();

    AVLNode<Key, Value>* next_node = nullptr;

    bool removed_from_right = (prev_node != nullptr && prev_node->getRight() == to_delete);

    if(to_delete->getLeft() != nullptr)
    {
      next_node =  to_delete->getLeft();
    }
    else
    {
      next_node = to_delete->getRight();
    }

    if(next_node != nullptr)
    {
      next_node->setParent(prev_node);
    }

    if(prev_node == nullptr)
    {
      this->root_ = next_node;
    }
    else if(prev_node != nullptr && prev_node->getLeft() == to_delete)
    {
      prev_node->setLeft(next_node);
    }
    else
    {
      prev_node->setRight(next_node);
    }


    delete to_delete;

    if(prev_node != nullptr)
    {
      rebalance_remove(prev_node, removed_from_right);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance_insert(AVLNode<Key, Value>* prev_node,AVLNode<Key, Value>* next_node)
{
  AVLNode<Key, Value>* prev = prev_node;
  AVLNode<Key, Value>* next = next_node;

  while(prev!= nullptr)
  {
    if(next == prev->getRight())
    {
      prev->updateBalance(-1);
    }
    else
    {
      prev->updateBalance(1);
    }

    int new_balance = prev->getBalance();

    if(new_balance == +1 || new_balance == -1 )
    {
      next = prev;
      prev = prev->getParent();
    }
    else if(new_balance == 0)
    {
      break;
    }
    else
    {
      rebalance_insert_helper(prev);
      break;
    }
  }
}


template<class Key, class Value>
void AVLTree<Key, Value>::rebalance_remove(AVLNode<Key, Value>* prev_node,bool removed_from_right)
{
  while(prev_node != nullptr)
  {
    if(removed_from_right)
    {
      prev_node->updateBalance(1);
    }
    else
    {
      prev_node->updateBalance(-1);
    }
    
    int current_balance = prev_node->getBalance();

    if(current_balance == +1 || current_balance == -1)
    {
      break;
    }
    else if(current_balance == 0)
    {
      AVLNode<Key, Value>* parent_node = prev_node->getParent();
      if(parent_node == nullptr)
      {
        return;
      }
      removed_from_right = (parent_node->getRight() == prev_node);
      prev_node = parent_node;
    }
    else
    {
      AVLNode<Key, Value>* new_subtree_root = rebalance_helper(prev_node);

      AVLNode<Key, Value>* parent_node = new_subtree_root->getParent();

      if(parent_node == nullptr)
      {
        return;
      }

      if(new_subtree_root->getBalance() == 0)
      {
        removed_from_right = (parent_node->getRight() == new_subtree_root);
        prev_node = parent_node;
      }
      else
      {
        break;
      }
    }

  }
}

template<class Key,class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance_helper(AVLNode<Key, Value>* node)
{
  int8_t balance_value = node->getBalance();

  if(balance_value == +2)
  {
    AVLNode<Key, Value>* left_node = node->getLeft();

    if(!left_node)
    {
      return node;
    }

    int8_t left_node_balance =0;

    if(left_node != nullptr)
    {
      left_node_balance = left_node->getBalance();
    }
    else
    {
      left_node_balance = 0;
    }


    if(left_node_balance >=0)
    {
      this->rotateRight(node);

      if(left_node_balance == 0)
      {
        left_node->setBalance(-1);
        node->setBalance(+1);
        return left_node;
      }
      else
      {
        left_node->setBalance(0);
        node->setBalance(0);
        return left_node;
      }
    } 
    else
    {
      AVLNode<Key, Value>* right_node = left_node->getRight();

      int8_t right_node_balance = 0;

      if(right_node != nullptr)
      {
        right_node_balance = right_node->getBalance();
      }
      else
      {
        right_node_balance = 0;
      }

      this->rotateLeft(left_node);
      this->rotateRight(node);


      if(right_node_balance == 0)
      {
        node->setBalance(0);
        right_node->setBalance(0);
        left_node->setBalance(0);
      }
      else if(right_node_balance == +1)
      {
        node->setBalance(-1);
        right_node->setBalance(0);
        left_node->setBalance(0);
      }
      else
      {
        node->setBalance(0);
        right_node->setBalance(0);
        left_node->setBalance(+1);
      }

      return right_node;
    }

  }
  else if(balance_value == -2)
  {
    AVLNode<Key, Value>* right_node = node->getRight();

    if(!right_node) 
    {
      return node;
    }

    int8_t right_node_balance = 0;

    if(right_node != nullptr)
    {
      right_node_balance = right_node->getBalance();
    }
    else
    {
      right_node_balance = 0;
    }

    if(right_node_balance <=0)
    {
      this->rotateLeft(node);

      if(right_node_balance == 0)
      {
        right_node->setBalance(1);
        node->setBalance(-1);
        return right_node;
      }
      else
      {
        right_node->setBalance(0);
        node->setBalance(0);
        return right_node;
      }
    } 
    else
    {
      AVLNode<Key, Value>* left_node = right_node->getLeft();

      int8_t left_node_balance = 0;

      if(left_node != nullptr)
      {
        left_node_balance = left_node->getBalance();
      }
      else
      {
        left_node_balance = 0;
      }

      this->rotateRight(right_node);
      this->rotateLeft(node);


      if(left_node_balance == 0)
      {
        node->setBalance(0);
        right_node->setBalance(0);
        left_node->setBalance(0);
      }
      else if(left_node_balance == -1)
      {
        node->setBalance(1);
        right_node->setBalance(0);
        left_node->setBalance(0);
      }
      else
      {
        node->setBalance(0);
        right_node->setBalance(-1);
        left_node->setBalance(0);
      }

      return left_node;
    }
  }

  return node;
}


template<class Key,class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance_insert_helper(AVLNode<Key, Value>* node)
{
  int8_t balance_value = node->getBalance();

  if(balance_value == +2)
  {
    AVLNode<Key, Value>* left_node = node->getLeft();

    if(!left_node)
    {
      return node;
    }

    int8_t left_node_balance =0;

    if(left_node != nullptr)
    {
      left_node_balance = left_node->getBalance();
    }
    else
    {
      left_node_balance = 0;
    }


    if(left_node_balance >=0)
    {
      this->rotateRight(node);

      left_node->setBalance(0);
      node->setBalance(0);
      return left_node;
    } 
    else
    {
      AVLNode<Key, Value>* right_node = left_node->getRight();

      int8_t right_node_balance = 0;

      if(right_node != nullptr)
      {
        right_node_balance = right_node->getBalance();
      }
      else
      {
        right_node_balance = 0;
      }

      this->rotateLeft(left_node);
      this->rotateRight(node);

      if(right_node_balance == +1)
      {
        node->setBalance(-1);
        left_node->setBalance(0);
      }
      else if(right_node_balance == 0)
      {
        node->setBalance(0);
        left_node->setBalance(0);
      }
      else
      {
        node->setBalance(0);
        left_node->setBalance(+1);
      }
      right_node->setBalance(0);

      return right_node;
    }

  }
  else if(balance_value == -2)
  {
    AVLNode<Key, Value>* right_node = node->getRight();

    if(!right_node) 
    {
      return node;
    }

    int8_t right_node_balance = 0;

    if(right_node != nullptr)
    {
      right_node_balance = right_node->getBalance();
    }
    else
    {
      right_node_balance = 0;
    }

    if(right_node_balance <=0)
    {
      this->rotateLeft(node);

      right_node->setBalance(0);
      node->setBalance(0);
      return right_node;  
    } 
    else
    {
      AVLNode<Key, Value>* left_node = right_node->getLeft();

      int8_t left_node_balance = 0;

      if(left_node != nullptr)
      {
        left_node_balance = left_node->getBalance();
      }
      else
      {
        left_node_balance = 0;
      }

      this->rotateRight(right_node);
      this->rotateLeft(node);


      if(left_node_balance == 0)
      {
        node->setBalance(0);
        right_node->setBalance(0);
      }
      else if(left_node_balance == -1)
      {
        node->setBalance(1);
        right_node->setBalance(0);
      }
      else
      {
        node->setBalance(0);
        right_node->setBalance(-1);
      }

      left_node->setBalance(0);
      return left_node;
    }
  }

  return node;
}


//other helper for balancing 


template<class Key,class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* l)
{

  if(l->getRight() == nullptr)
  {
    return;
  }

  AVLNode<Key, Value>* right_child = l->getRight();
  AVLNode<Key, Value>* subtree = right_child->getLeft();
  AVLNode<Key, Value>* current_parent = l->getParent();

  right_child->setParent(current_parent);

  if(current_parent == nullptr)
  {
    this->root_ = right_child;
  }
  else if(current_parent->getLeft() == l)
  {
    current_parent->setLeft(right_child);
  }
  else
  {
    current_parent->setRight(right_child);
  }

  right_child->setLeft(l);
  l->setParent(right_child);

  l->setRight(subtree);

  if(subtree!=nullptr)
  {
    subtree->setParent(l);
  }
}

template<class Key,class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* r)
{

  if(r->getLeft() == nullptr)
  {
    return;
  }

  AVLNode<Key, Value>* left_child = r->getLeft();
  AVLNode<Key, Value>* subtree = left_child->getRight();
  AVLNode<Key, Value>* current_parent = r->getParent();

  
  
  left_child->setParent(current_parent);

  if(current_parent == nullptr)
  {
    this->root_ = left_child;
  }
  else if(current_parent->getRight() == r)
  {
    current_parent->setRight(left_child);
  }
  else
  {
    current_parent->setLeft(left_child);
  }

  left_child->setRight(r);
  r->setParent(left_child);

  r->setLeft(subtree);

  if(subtree!=nullptr)
  {
    subtree->setParent(r);
  }

}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
