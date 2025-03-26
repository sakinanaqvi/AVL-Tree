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

    // Add helper functions here
    void rLeft(AVLNode<Key, Value>* node);
    void rRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* par, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* n, int8_t difference);
    void balanceRotation(AVLNode<Key,Value>* n, AVLNode<Key,Value>* cd, AVLNode<Key,Value>* gcd); 

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    //check if tree is empty and creates a new node which becomes root
    if(this->root_ == nullptr) {
      AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      this->root_ = node;
      node->setBalance(0);
      return;
    }
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* par = nullptr;

    //travserse through the tree
    while(curr != nullptr) {
      par = curr;
      
      //check if the key matches the node and update it
      if(new_item.first == curr->getKey()) {
        curr->setValue(new_item.second); 
        return;
      }

      curr = (new_item.first < curr->getKey()) ? curr->getLeft() : curr->getRight();
    }
    //make a new node with the key and value and attach it 
    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, par);
    if(par->getKey() > new_item.first) par->setLeft(node);
    else par->setRight(node);

    node->setBalance(0);

    //update balance factors if parent node was balanced
    if(par->getBalance() == 0) {
      par->updateBalance((node == par->getLeft()) ? -1 : 1);
      insertFix(par, node);
    } else par->setBalance(0);
}   
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> * par, AVLNode<Key, Value> * node) {
  //check if parent does not have a parent
  if(par->getParent() == nullptr) return;

  AVLNode<Key, Value> * gp = par->getParent();
  //if parent is right child of grandparent
  if(gp->getRight() == par) {
      gp->updateBalance(1);
      //if grandparent is balanced, no rotations needed
      if(gp->getBalance() == 0) return;
      //if balance factor: 1, close to imbalance so check further up
      else if(gp->getBalance() == 1) insertFix(gp, par);
       //if balance factor: 2, imbalance on right
      else if(gp->getBalance() == 2) {
        if(gp->getRight() == par && par->getRight() == node) {
          //RR Case
          rLeft(gp);
          gp->setBalance(0);
          par->setBalance(0);
        } else if(par->getLeft() == node && gp->getBalance() > 0 && par->getBalance() < 0) {
            //RL Case
            rRight(par);
            rLeft(gp);

            if(node->getBalance() <= 0) {
              par->setBalance((node->getBalance() == -1) ? 1 : 0);
              gp->setBalance(0);
            } else {
                par->setBalance(0);
                gp->setBalance(-1);
            }
            node->setBalance(0);
        } 
      }
    //if parent is left child of grandparent
  } else if(gp->getLeft() == par) {
      gp->updateBalance(-1);
      //if grandparent is balanced, no rotations needed
      if(gp->getBalance() == 0) return;
      //if balance factor: -1, close to imbalance so check further up
      else if(gp->getBalance() == -1) insertFix(gp, par);
      //if balance factor: -2, imbalance on left
      else if(gp->getBalance() == -2) {
        if(gp->getLeft() == par && par->getLeft() == node) {
          //LL Case
          rRight(gp);
          gp->setBalance(0);
          par->setBalance(0);
        } else if(par->getRight() == node  && gp->getBalance() < 0 && par->getBalance() > 0) {
            //LR Case
            rLeft(par);
            rRight(gp);

            if(node->getBalance() <= 0) {
              par->setBalance(0);
              gp->setBalance((node->getBalance() == -1) ? 1 : 0);
            } else {
                par->setBalance(-1);
                gp->setBalance(0);
            }
            node->setBalance(0);
        }
      }
  }
}
template<class Key, class Value>
void AVLTree<Key, Value>:: rLeft(AVLNode<Key, Value> * node) {
  //if node is null
  if(!node) return;

  AVLNode<Key, Value>* par = node->getParent();
  AVLNode<Key, Value>* right = node->getRight();
  
  //if right child does not exist
  if(right == nullptr) return;

  node->setRight(right->getLeft());
  //if right has left child, change parent to current 
  if(right->getLeft()) right->getLeft()->setParent(node);
  right->setParent(par);

  //updating parents' right or left child depending on parent child
  if(par == nullptr) this->root_ = right;
  else if(par->getRight() == node) par->setRight(right);
  else  par->setLeft(right);

  //set left child of right child to current
  right->setLeft(node);
  node->setParent(right);

}
template<class Key, class Value>
void AVLTree<Key, Value>:: rRight(AVLNode<Key, Value> * node) { 
  //if node is null
  if(!node) return;

  AVLNode<Key, Value>* par = node->getParent();
  AVLNode<Key, Value>* left = node->getLeft();
  
  //if left child does not exist
  if(left == nullptr) return;

  node->setLeft(left->getRight());
  //if left has right child, change parent to current 
  if(left->getRight()) left->getRight()->setParent(node);
  left->setParent(par);

  //updating parents' right or left child depending on parent child
  if(par == nullptr) this->root_ = left;
  else if(par->getRight() == node) par->setRight(left);
  else  par->setLeft(left);

  //set right child of left child to current
  left->setRight(node);
  node->setParent(left);
}
/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
  //check if the tree is empty
  if(this->root_ == nullptr) return;

  AVLNode<Key, Value>* removed = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
  if(removed == nullptr) return;

  //case of two children 
  if(removed->getLeft() && removed->getRight()) {
    AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(removed));
    nodeSwap(removed, pred);
  }

  AVLNode<Key, Value>* par = removed->getParent();
  int8_t balDifference = 0;
  
  if(par != nullptr) {
    if(par->getRight() == removed) balDifference = -1;
    else if(par->getLeft() == removed) balDifference = 1;
  }
  
  AVLNode<Key, Value>* cd = nullptr;
  //case of no children 
  if(removed->getLeft() == nullptr && removed->getRight() == nullptr) {
    cd = nullptr;
  } 
  
  //case of only right child 
  if(removed->getLeft() == nullptr && removed->getRight() != nullptr) cd = removed->getRight();
  else if(removed->getLeft() != nullptr && removed->getRight() == nullptr) cd = removed->getLeft();
  //case of only left child 


  //if the node to remove is the root, change the root
  if(par == nullptr) this->root_ = cd;
  else {
    if(par->getRight() == removed) {
      par->setRight(cd);
    } else if(par->getLeft() == removed) {
        par->setLeft(cd);
    }
  }

  if(cd != nullptr) cd->setParent(par);

  delete removed;
  removeFix(par, balDifference);
}
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* node, int8_t difference)
{
  if(node == nullptr) return;
  AVLNode<Key, Value>* par = node->getParent();
  //find balance difference for parent
  int8_t nDifference = (par && par->getLeft() == node) ? 1 : -1;

   if(difference == 1) {
      if(node->getBalance() + difference == 2) {
        AVLNode<Key, Value>* cd = node->getRight();
        //RR case
        if(cd->getBalance() == 1) {
          rLeft(node);
          node->setBalance(0);
          cd->setBalance(0);
          removeFix(par, nDifference); 
        } else if(cd->getBalance() == -1) { //RL case
            AVLNode<Key, Value>* gcd = cd->getLeft();
            rRight(cd);
            rLeft(node);

            node->setBalance(gcd->getBalance() == 1 ? -1 : 0);
            cd->setBalance(gcd->getBalance() == -1 ? 1 : 0);
            gcd->setBalance(0);
            
            removeFix(par, nDifference);
        }
        else if(cd->getBalance() == 0) { 
            rLeft(node);
            node->setBalance(1);
            cd->setBalance(-1);
        }
      } else if(node->getBalance() + difference == 0) { //perfectly balanced
          node->setBalance(0);
          removeFix(par, nDifference);
        //a little imbalanced
      } else if(node->getBalance() + difference == 1) node->setBalance(1);
  } else if(difference == -1) {
      if(node->getBalance() + difference == -2) {
        AVLNode<Key, Value>* cd = node->getLeft();

        //LL Case
        if(cd->getBalance() == -1) {
            rRight(node);
            node->setBalance(0);
            cd->setBalance(0);
            removeFix(par, nDifference);
        } else if(cd->getBalance() == 1) { //LR Case
            AVLNode<Key, Value>* gcd = cd->getRight();
            rLeft(cd);
            rRight(node);

            node->setBalance(gcd->getBalance() == -1 ? 1 : 0);
            cd->setBalance(gcd->getBalance() == 1 ? -1 : 0);
            gcd->setBalance(0);
            
            removeFix(par, nDifference);
          } else if(cd->getBalance() == 0) { 
              rRight(node);
              node->setBalance(-1);
              cd->setBalance(1);
          }
      } else if(node->getBalance() + difference == 0) { //perfectly balanced
          node->setBalance(0);
          removeFix(par, nDifference);
        //a little imbalanced
      } else if(node->getBalance() + difference == -1) node->setBalance(-1);
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
