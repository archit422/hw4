#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here
bool equalPaths_helper(Node * node, int depth, int& reference_depth);

bool equalPaths(Node * root)
{
  if(root == nullptr)
  {
    return true; 
  }
  int depth = 0;
  int reference_depth = -1; 

  return equalPaths_helper(root,depth,reference_depth);
}


bool equalPaths_helper(Node * node, int depth, int& reference_depth)
{
  bool left_check = true;
  bool right_check = true;

  if(node->left == nullptr && node->right ==nullptr)
  {
    if (reference_depth == -1)
    {
      reference_depth = depth;
      return true;
    }
    else{
      return depth == reference_depth;
    }
  }
  else
  {
    if(node->left != nullptr)
    {
      left_check = equalPaths_helper(node->left, depth+1 , reference_depth);
    }
    if(node->right != nullptr)
    {
      right_check = equalPaths_helper(node->right, depth+1 ,reference_depth);
    }
    return (left_check && right_check);
  }
}


  //if node is leaf
    //if targetdepth is unset, set to depth 
    //else return depth == target depth
  //if node is internal recursively check all children





