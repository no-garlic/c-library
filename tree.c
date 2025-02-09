
#include "tree.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include "memorypool.h"


struct treenode
{
  int m_key;
  struct treenode *m_parent;
  struct treenode *m_left;
  struct treenode *m_right;
  struct tree *m_tree;
};


struct tree
{
  struct memorypool *m_pool;
  struct treenode *m_root;
  int m_valuesize;
  int m_count;
};


// todo - call callbacks for mem

const tree_enum_result tree_enum_ok   = 0;
const tree_enum_result tree_enum_stop = -1;


struct treenode *_treenode_alloc(struct tree *a_tree, int a_key, void *a_value, const char *a_file, int a_line);
struct treenode *_tree_insert_root(struct tree *a_tree, int a_key, void *value, const char *a_file, int a_line);
struct treenode *_tree_insert_child(struct tree *a_tree, struct treenode *a_node, int a_key, void *value, const char *a_file, int a_line);
void _tree_insert_subtree(struct tree *a_tree, struct treenode *a_node, struct treenode *a_subtree);
void _treenode_set(struct tree *a_tree, struct treenode *node, void *a_value);
void _tree_remove(struct treenode *a_node);
void _tree_remove_subtree(struct treenode *a_node);
void _tree_remove_no_children(struct treenode *a_node);
void _tree_remove_one_child(struct treenode *a_node);
void _tree_remove_two_children(struct treenode *a_node);
tree_enum_result _tree_enum_node(struct treenode *a_node, tree_enum_func a_funcptr);
tree_enum_result _tree_enum_node_rev(struct treenode *a_node, tree_enum_func a_funcptr);


struct tree *_tree_alloc(int a_valuesize, struct memorypool *a_pool, const char *a_file, int a_line)
{
  struct tree *new_tree;

  // limit the value size to 32k pre value
  assert(a_valuesize >= 0);
  assert(a_valuesize <= 32 * 1024);

  new_tree = (struct tree *) _palloc(a_file, a_line, a_pool, sizeof(struct tree));
  assert(new_tree);

  memset(new_tree, 0, sizeof(struct tree));

  new_tree->m_valuesize = a_valuesize;
  new_tree->m_pool = a_pool;

  return new_tree;
}


void tree_free(struct tree *a_tree)
{
  assert(a_tree);

  if (a_tree->m_root)
  {
    tree_remove_subtree(a_tree->m_root);
  }

  pfree(a_tree->m_pool, a_tree);
}


int tree_count(struct tree *a_tree)
{
  assert(a_tree);
  return a_tree->m_count;
}


struct treenode *_tree_insert(struct tree *a_tree, int a_key, void *a_value, const char *a_file, int a_line)
{
  struct treenode *node, *ptr;

  assert(a_tree);

  ptr = a_tree->m_root;
  if (!ptr)
  {
    node = _tree_insert_root(a_tree, a_key, a_value, a_file, a_line);
    return node;
  }
 
  node = _tree_insert_child(a_tree, ptr, a_key, a_value, a_file, a_line);
  return node;
}


struct treenode *tree_set(struct tree *a_tree, int a_key, void *a_value)
{
  struct treenode *node;

  assert(a_tree);

  node = tree_find(a_tree, a_key);

  if (node)
  {
    _treenode_set(a_tree, node, a_value);
  }

  return node;
}


struct treenode *tree_set_or_insert(struct tree *a_tree, int a_key, void *a_value)
{
  struct treenode *node;

  assert(a_tree);

  node = tree_find(a_tree, a_key);

  if (node)
  {
    _treenode_set(a_tree, node, a_value);
  }
  else
  {
    node = tree_insert(a_tree, a_key, a_value);
  }

  return node;
}


int tree_find_remove_subtree(struct tree *a_tree, int a_key)
{
  struct treenode *node;

  assert(a_tree);

  node = tree_find(a_tree, a_key);

  if (node)
  {
    tree_remove_subtree(node);
    return 0;
  }

  return -1;
}


int tree_find_remove(struct tree *a_tree, int a_key)
{
  struct treenode *node;

  assert(a_tree);

  node = tree_find(a_tree, a_key);

  if (node)
  {
    tree_remove(node);
    return 0;
  }

  return -1;
}


void tree_remove_subtree(struct treenode *a_node)
{
  assert(a_node);
  assert(a_node->m_tree);

  _tree_remove_subtree(a_node);
}


void tree_remove(struct treenode *a_node)
{
  assert(a_node);

  if (!a_node->m_left && !a_node->m_right)
  {
    _tree_remove_no_children(a_node);
    return;
  }
  else if (a_node->m_left && a_node->m_right)
  {
    _tree_remove_two_children(a_node);
    return;
  }
  else
  {
    _tree_remove_one_child(a_node);
    return;
  }
}


struct treenode *tree_first(struct tree *a_tree)
{
  struct treenode *node;

  assert(a_tree);

  node = a_tree->m_root;

  while (node)
  {
    if (node->m_left)
    {
      node = node->m_left;
    }
    else
    {
      return node;
    }
  }

  return 0;
}


struct treenode *tree_last(struct tree *a_tree)
{
  struct treenode *node;

  assert(a_tree);

  node = a_tree->m_root;

  while (node)
  {
    if (node->m_right)
    {
      node = node->m_right;
    }
    else
    {
      return node;
    }
  }

  return 0;
}


struct treenode *tree_next(struct treenode *a_node)
{
  struct treenode *node;
  
  assert(a_node);

  if (a_node->m_left)
  {
    return a_node->m_left;
  }

  node = a_node->m_parent;
  
  while (node)
  {
    if (a_node->m_right)
    {
      return a_node->m_right;
    }
  }

  return 0;
}


struct treenode *tree_find(struct tree *a_tree, int a_key)
{
  struct treenode *node;

  assert(a_tree);

  node = a_tree->m_root;

  while (node)
  {
    if (node->m_key == a_key)
    {
      return node;
    }
    else if (a_key < node->m_key)
    {
      node = node->m_left;
    }
    else
    {
      node = node->m_right;
    }
  }

  return 0;
}


struct treenode *treenode_left(struct treenode *a_node)
{
  assert(a_node);
  return a_node->m_left;
}


struct treenode *treenode_right(struct treenode *a_node)
{
  assert(a_node);
  return a_node->m_right;
}


struct treenode *treenode_parent(struct treenode *a_node)
{
  assert(a_node);
  return a_node->m_parent;
}


int treenode_key(struct treenode *a_node)
{
  assert(a_node);
  return a_node->m_key;
}


void *treenode_value(struct treenode *a_node)
{
  struct tree *nodetree;
  char *value;

  assert(a_node);

  nodetree = a_node->m_tree;
  assert(nodetree);

  if (nodetree->m_valuesize)
  {
    value = (char *) a_node;
    value += sizeof(struct treenode);
    return (void *) value;
  }
  
  return 0;
}


tree_enum_result tree_enum(struct tree *a_tree, tree_enum_func a_funcptr)
{
  assert(a_tree);

  if (a_tree->m_root)
  {
    return _tree_enum_node(a_tree->m_root, a_funcptr);
  }

  return tree_enum_ok;
}


tree_enum_result tree_enum_rev(struct tree *a_tree, tree_enum_func a_funcptr)
{
  assert(a_tree);

  if (a_tree->m_root)
  {
    return _tree_enum_node_rev(a_tree->m_root, a_funcptr);
  }

  return tree_enum_ok;
}




// ------------------------------------------------------------------------------
// Internal
// ------------------------------------------------------------------------------




void _tree_remove_subtree(struct treenode *a_node)
{
  if (a_node->m_left)
  {
    _tree_remove_subtree(a_node->m_left);
    a_node->m_left = 0;
  }

  if (a_node->m_right)
  {
    _tree_remove_subtree(a_node->m_right);
    a_node->m_right = 0;
  }

  _tree_remove(a_node);
}


void _tree_remove(struct treenode *a_node)
{
  assert(a_node);
  assert(a_node->m_tree);
  assert(a_node->m_tree->m_count > 0);

  // the node must not have children
  assert(!a_node->m_left);
  assert(!a_node->m_right);

  // unlink this node from the parent
  if (a_node->m_parent)
  {
    if (a_node == a_node->m_parent->m_left)
    {
      a_node->m_parent->m_left = 0;
    }
    else if (a_node == a_node->m_parent->m_right)
    {
      a_node->m_parent->m_right = 0;
    }
  }

  // if this is the root node then clear it
  if (a_node == a_node->m_tree->m_root)
  {
    a_node->m_tree->m_root = 0;
  }

  // reduce the count
  a_node->m_tree->m_count--;

  // free the node
  pfree(a_node->m_tree->m_pool, a_node);
}


void _tree_remove_no_children(struct treenode *a_node)
{
  assert(a_node);

  _tree_remove(a_node);
}


void _tree_remove_one_child(struct treenode *a_node)
{
  struct treenode **ptrnode;

  assert(a_node);
  assert((a_node->m_left == 0) ^ (a_node->m_right == 0));

  ptrnode = 0;

  if (a_node == a_node->m_tree->m_root)
  {
    ptrnode = &(a_node->m_tree->m_root);
  }
  else if (a_node->m_parent && a_node->m_parent->m_left == a_node)
  {
    ptrnode = &(a_node->m_parent->m_left);
  }
  else if (a_node->m_parent && a_node->m_parent->m_right == a_node)
  {
    ptrnode = &(a_node->m_parent->m_right);
  }

  assert(ptrnode);

  if (a_node->m_left)
  {
    *ptrnode = a_node->m_left;
    a_node->m_left->m_parent = a_node->m_parent;
    a_node->m_left = 0;
  }
  else
  {
    assert(a_node->m_right);
    *ptrnode = a_node->m_right;
    a_node->m_right->m_parent = a_node->m_parent;
    a_node->m_right= 0;
  }

  _tree_remove(a_node);
}


void _tree_remove_two_children(struct treenode *a_node)
{
  struct treenode *left, *right;
  struct tree *nodetree;

  assert(a_node);

  left = a_node->m_left;
  right = a_node->m_right;
  nodetree = a_node->m_tree;

  assert(left);
  assert(right);
  assert(nodetree);

  left->m_parent = 0;
  right->m_parent = 0;
  a_node->m_left = 0;
  a_node->m_right = 0;

  _tree_remove(a_node);

  if (nodetree->m_root)
  {
    _tree_insert_subtree(nodetree, nodetree->m_root, left);
    _tree_insert_subtree(nodetree, nodetree->m_root, right);
  }
  else
  {
    nodetree->m_root = left;
    _tree_insert_subtree(nodetree, left, right);    
  }
}


struct treenode *_treenode_alloc(struct tree *a_tree, int a_key, void *a_value, const char *a_file, int a_line)
{
  struct treenode *node;
  char *memory;
  int allocsize;

  assert(a_tree);

  allocsize = sizeof(struct treenode) + a_tree->m_valuesize;

  memory = (char *) _palloc(a_file, a_line, a_tree->m_pool, allocsize);
  assert(memory);

  memset(memory , 0, allocsize);

  node = (struct treenode *) memory;
  assert(node);

  node->m_key = a_key;
  node->m_tree = a_tree;

  a_tree->m_count++;

  if (a_tree->m_valuesize)
  {
    _treenode_set(a_tree, node, a_value);
  }

  return node;
}


void _treenode_set(struct tree *a_tree, struct treenode *a_node, void *a_value)
{
  char *value;

  assert(a_tree);
  assert(a_tree->m_valuesize);
  assert(a_node);

  value = (char *) a_node;
  value += sizeof(struct treenode);

  if (a_value)
  {
    memcpy(value, a_value, a_tree->m_valuesize);
  }
  else
  {
    memset(value, 0, a_tree->m_valuesize);
  }
}


struct treenode *_tree_insert_root(struct tree *a_tree, int a_key, void *a_value, const char *a_file, int a_line)
{
  assert(a_tree);

  a_tree->m_root = _treenode_alloc(a_tree, a_key, a_value, a_file, a_line);
  return a_tree->m_root;
}


struct treenode *_tree_insert_child(struct tree *a_tree, struct treenode *a_node, int a_key, void *a_value, const char *a_file, int a_line)
{
  assert(a_tree);
  assert(a_node);

  // is the key equal to a_node
  if (a_key == a_node->m_key)
  {
    return 0;
  }

  // is the key left of a_node
  if (a_key < a_node->m_key)
  {
    if (!a_node->m_left)
    {
      a_node->m_left = _treenode_alloc(a_tree, a_key, a_value, a_file, a_line);
      a_node->m_left->m_parent = a_node;
      return a_node->m_left;
    }
    else
    {
      return _tree_insert_child(a_tree, a_node->m_left, a_key, a_value, a_file, a_line);
    }
  }

  // the key must be right of a_node
  if (!a_node->m_right)
  {
    a_node->m_right = _treenode_alloc(a_tree, a_key, a_value, a_file, a_line);
    a_node->m_right->m_parent = a_node;
    return a_node->m_right;
  }
  else
  {
    return _tree_insert_child(a_tree, a_node->m_right, a_key, a_value, a_file, a_line);
  }
}


void _tree_insert_subtree(struct tree *a_tree, struct treenode *a_node, struct treenode *a_subtree)
{
  assert(a_tree);
  assert(a_node);
  assert(a_subtree->m_key != a_node->m_key);

  // is the key left of a_node
  if (a_subtree->m_key < a_node->m_key)
  {
    if (!a_node->m_left)
    {
      a_node->m_left = a_subtree;
      a_subtree->m_parent = a_node;
      return;
    }
    else
    {
      _tree_insert_subtree(a_tree, a_node->m_left, a_subtree);
      return;
    }
  }

  // the key must be right of a_node
  if (!a_node->m_right)
  {
    a_node->m_right = a_subtree;
    a_subtree->m_parent = a_node;
    return;
  }
  else
  {
    _tree_insert_subtree(a_tree, a_node->m_right, a_subtree);
    return;
  }
}


tree_enum_result _tree_enum_node(struct treenode *a_node, tree_enum_func a_funcptr)
{
  tree_enum_result result;

  assert(a_node);

  if (a_node->m_left)
  {
    result = _tree_enum_node(a_node->m_left, a_funcptr);
    if (result == tree_enum_stop)
    {
      return result;
    }
  }

  result = a_funcptr(a_node);
  if (result == tree_enum_stop)
  {
    return result;
  }

  if (a_node->m_right)
  {
    result = _tree_enum_node(a_node->m_right, a_funcptr);
    if (result == tree_enum_stop)
    {
      return result;
    }
  }

  return tree_enum_ok;
}


tree_enum_result _tree_enum_node_rev(struct treenode *a_node, tree_enum_func a_funcptr)
{
  tree_enum_result result;

  assert(a_node);

  if (a_node->m_right)
  {
    result = _tree_enum_node_rev(a_node->m_right, a_funcptr);
    if (result == tree_enum_stop)
    {
      return result;
    }
  }

  result = a_funcptr(a_node);
  if (result == tree_enum_stop)
  {
    return result;
  }

  if (a_node->m_left)
  {
    result = _tree_enum_node_rev(a_node->m_left, a_funcptr);
    if (result == tree_enum_stop)
    {
      return result;
    }
  }

  return tree_enum_ok;
}


// -- EOF

