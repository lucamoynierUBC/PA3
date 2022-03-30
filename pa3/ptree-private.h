/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////
void Clear(Node* subRoot);

int NumLeaves(Node* subroot) const;

int Size(Node* subRoot) const;

void FlipHorizontal(Node* subRoot);

void FlipVertical(Node* subRoot);

void Prune(Node* subRoot, double tolerance);

bool inTol(Node* subRoot, double tolerance, HSLAPixel& avg);

void FillCN(Node* subroot, PNG* png) const;

Node* Copy(Node* subroot);

void UpdateCNULw(Node* subroot, unsigned int wid, bool pos);

void UpdateCNULh(Node* subroot, unsigned int hgt, bool pos);







#endif