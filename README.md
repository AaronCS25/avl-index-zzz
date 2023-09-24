# avl-index

## Description

This is an implementation of an AVL tree index. It is a self-balancing binary search tree that stores a key and two pointers to the left(previous) and right(next) record.

## Complexity 

$n := Number \ of \ records \ in \ the \ index file$

$K := Average \ number \ of \ duplicates \ of \ a \ key$

If indexing over unique keys, $K = 1$.

| Operation           | Complexity             | Description       |
| ---------           | ----------             | -----------       |
| ```Search```        | $O(lg(n) + K)$       | Descends the tree until matched key. Retrieve duplicates from linked list.|
| ```Range_search```  | $O(n * K)$       | Descends recursively pruning out of range nodes.                   |
| ```Insert```        | $O(lg(n))$   | Search operation. Inserts as child of leaf node and makes rotations if necessary. Duplicates are linked with LIFO method.|
| ```Delete```        | $O(lg(n))$   | Search operation of key to be deleted. |
