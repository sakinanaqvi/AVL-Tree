#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#endif
#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsHelp(Node * root, int& height) {
  if(root == nullptr) {
    height = 0;
    return true;
  }
  int rh = 0, lh = 0;

  //check each of the right and left subtree
  bool rightEqual = equalPathsHelp(root->right, rh);
  bool leftEqual = equalPathsHelp(root->left, lh);
  height = max(rh, lh) + 1; //keep updating each height for the node

  //check if right subtree is missing 
  if(root->right == nullptr && root->left != nullptr) return rh == 0 && leftEqual;
  //check if the left subtree is missingg
  if(root->left == nullptr && root->right != nullptr) return lh == 0 && rightEqual;
  
  return rightEqual && leftEqual && rh == lh;

}
bool equalPaths(Node * root)
{
  int height = 0;
  return equalPathsHelp(root, height);
}

