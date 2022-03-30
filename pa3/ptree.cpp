/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  // add your implementation below
  Clear(root);
}





/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) 
{
  // add your implementation below
  this->root = Copy(other.root);
}

Node* PTree::Copy(Node* subroot)
{
  if(subroot == nullptr)
  {
    return nullptr;
  }
  else
  {
    Node* nd = new Node();
    nd->upperleft = subroot->upperleft;
    nd->width = subroot->width;
    nd->height = subroot->height;
    nd->avg = subroot->avg;

    if(subroot->A != nullptr)
    {
      nd->A = Copy(subroot->A);
    }
    else
    {
      nd->A = nullptr;
    }

    if(subroot->B != nullptr)
    {
    nd->B = Copy(subroot->B);
    }
    else
    {
      nd->B = nullptr;
    }

    return nd;
  }

  
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  // replace the line below with your implementation
  
  
  Node* newNode = new Node();
  newNode->upperleft = ul;
  newNode->width = w;
  newNode->height = h;

  if(w == 1 && h == 1)
  {
    newNode->avg = *(im.getPixel(ul.first, ul.second));
    return newNode;
  }
  else
  {
    unsigned int count = 0;
    double totalX = 0;
    double totalY = 0;
    double totalS = 0;
    double totalL = 0;

    for(unsigned int x = ul.first; x < ul.first + w; x++)
    {
      for(unsigned int y = ul.second; y < ul.second + h; y++)
      {
        HSLAPixel* pix = im.getPixel(x, y);
        count++;
        totalX += Deg2X(pix->h);
        totalY += Deg2Y(pix->h);
        totalS += pix->s;
        totalL += pix->l;
      }
    }

    totalX = totalX / count;
    totalY = totalY / count;
    totalS = totalS / count;
    totalL = totalL / count;

    double finalAngle = XY2Deg(totalX, totalY);
    newNode->avg.h = finalAngle;
    newNode->avg.s = totalS;
    newNode->avg.l = totalL;
  }

  if(w >= h)
  {
    newNode->A = BuildNode(im, ul, w/2, h);
    pair<unsigned int, unsigned int> newUL;
    newUL.first = ul.first + w/2;
    newUL.second = ul.second;
    newNode->B = BuildNode(im, newUL, w % 2 == 0 ? w/2 : w/2 + 1, h);
  }
  else
  {
    newNode->A = BuildNode(im, ul, w, h/2);
    pair<unsigned int, unsigned int> newUL;
    newUL.first = ul.first;
    newUL.second = ul.second + h/2;
    newNode->B = BuildNode(im, newUL, w, h % 2 == 0 ? h/2 : h/2 + 1);
  }

  return newNode;
}

  
      
    
      


    


  
  


////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  // add your implementation below
  root = BuildNode(im, make_pair(0,0), im.width(), im.height());
  
  


}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  // add your implementation below
  Copy(other);

  
  
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  // add your implementation below

  if(this->root == other.root)
  {
    return *this;
  }
  else
  {
    Clear();
    Copy(other);
  }

  return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  // add your implementation below
  Clear();
  
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  PNG* retPNG = new PNG(root->width, root->height);

  FillCN(root, retPNG);

  return *retPNG;

  


  
}

void PTree:: FillCN(Node* subroot, PNG* png) const
{
  if(subroot == nullptr)
  {
    return;
  }

  bool isLeaf = true;

  if(subroot->A != nullptr)
  {
    FillCN(subroot->A, png);
    isLeaf = false;
  }
  if(subroot->B != nullptr)
  {
    FillCN(subroot->B, png);
    isLeaf = false;
  }

  if(!isLeaf)
  {
    return;
  }
  else
  {
    for(unsigned int x = subroot->upperleft.first; x < (subroot->width + subroot->upperleft.first); x++)
    {
      for(unsigned int y = subroot->upperleft.second; y < (subroot->height + subroot->upperleft.second); y++)
      {
        HSLAPixel* pix = png->getPixel(x, y);
        pix->h = subroot->avg.h;
        pix->s = subroot->avg.s;
        pix->l = subroot->avg.l;
        pix->a = subroot->avg.a;
      }
    }
  }
  
}


/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  // add your implementation below

  Prune(root, tolerance);

}



void PTree::Prune(Node* subRoot, double tolerance) {
  if(subRoot == NULL || root->A == NULL){
    return;
  }

  
  if (inTol(subRoot, tolerance, subRoot->avg)) {
    Clear(subRoot->A);
    Clear(subRoot->B);
    subRoot->A = NULL;
    subRoot->B = NULL;


  } else {
    Prune(subRoot->A, tolerance);
    Prune(subRoot->B, tolerance);
  }
  

}

void PTree:: Clear(Node* subRoot) {
  if (subRoot == NULL) {
    return;
  }
  if (subRoot != NULL) {
    Clear(subRoot->A);
    Clear(subRoot->B);
  }
  delete subRoot;
}

bool PTree:: inTol(Node* subRoot, double tolerance, HSLAPixel& avg) {
  if (subRoot->A == NULL) {
    bool valid = false;
    valid = (subRoot->avg.dist(avg) <= tolerance);
    return valid;   
  } else {
    return inTol(subRoot->A, tolerance, avg) && inTol(subRoot->B, tolerance, avg);
  }
}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  // replace the line below with your implementation
  return Size(root);
}

int PTree::Size(Node* subRoot) const {
  if (subRoot == NULL) {
    return 0;
  }

  return 1 + Size(subRoot->A) + Size(subRoot->B);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  // replace the line below with your implementation
  
  return NumLeaves(root);
}

int PTree::NumLeaves(Node* subRoot) const {
  if (subRoot == NULL) {
    return 0;
  }
  if (subRoot->A == NULL && subRoot->B == NULL){
    return 1;
  } else {
    return NumLeaves(subRoot->A) + NumLeaves(subRoot->B);
  }

}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  // add your implementation below
  FlipHorizontal(root);
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  // add your implementation below'
  FlipVertical(root);
}

void PTree::FlipVertical(Node* subRoot) {
  if (subRoot == nullptr)
  {
    return;
  }
  if(subRoot->A == nullptr && subRoot->B == nullptr)
  {
    return;
  }

  if(subRoot->height > subRoot->width)
  {
    Node* topNode = subRoot->A;
    Node* botNode = subRoot->B;
    
    pair<unsigned int, unsigned int> temp = topNode->upperleft;
    botNode->upperleft = temp;
    pair<unsigned int, unsigned int> tempb;
    tempb.first = temp.first;
    tempb.second = temp.second + botNode->height;
    topNode->upperleft = tempb;

    subRoot->A = botNode;
    UpdateCNULh(subRoot->A, topNode->height, false);
    subRoot->B = topNode;
    UpdateCNULh(subRoot->B, botNode->height, true);
  }

  FlipVertical(subRoot->A);
  FlipVertical(subRoot->B);
}

void PTree::FlipHorizontal(Node* subRoot) {
  if (subRoot == nullptr)
  {
    return;
  }
  if(subRoot->A == nullptr && subRoot->B == nullptr)
  {
    return;
  }

  if(subRoot->width >= subRoot->height)
  {
    Node* leftNode = subRoot->A;
    Node* rightNode = subRoot->B;
    
    pair<unsigned int, unsigned int> temp = leftNode->upperleft;
    rightNode->upperleft = temp;
    pair<unsigned int, unsigned int> tempb;
    tempb.first = temp.first + rightNode->width;
    tempb.second = temp.second;
    leftNode->upperleft = tempb;

    subRoot->A = rightNode;
    UpdateCNULw(subRoot->A, leftNode->width, false);
    subRoot->B = leftNode;
    UpdateCNULw(subRoot->B, rightNode->width, true);
  }

  FlipHorizontal(subRoot->A);
  FlipHorizontal(subRoot->B);
 
  
}

void PTree::UpdateCNULw(Node* subroot, unsigned int wid, bool pos)
{
  if(subroot->A == nullptr && subroot->B == nullptr)
  {
    return;
  }

  Node* a = subroot->A;
  Node* b = subroot->B;

  if(pos)
  {
    a->upperleft.first += wid;
    b->upperleft.first += wid;
  }
  else
  {
    a->upperleft.first -= wid;
    b->upperleft.first -= wid;
  }

  UpdateCNULw(a, wid, pos);
  UpdateCNULw(b, wid, pos);
}

void PTree::UpdateCNULh(Node* subroot, unsigned int hgt, bool pos)
{
  if(subroot->A == nullptr && subroot->B == nullptr)
  {
    return;
  }

  Node* a = subroot->A;
  Node* b = subroot->B;

  if(pos)
  {
    a->upperleft.second += hgt;
    b->upperleft.second += hgt;
  }
  else
  {
    a->upperleft.second -= hgt;
    b->upperleft.second -= hgt;
  }

  UpdateCNULh(a, hgt, pos);
  UpdateCNULh(b, hgt, pos);
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

