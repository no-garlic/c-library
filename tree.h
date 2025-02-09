
#ifndef __h_tree
#define __h_tree

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus
  
#include "config.h"

// forward declarations
typedef struct tree;
typedef struct treenode;
typedef struct memorypool;
typedef int tree_enum_result;

// external values for enumerating the tree
extern const tree_enum_result tree_enum_ok;
extern const tree_enum_result tree_enum_stop;

// function prototype for enumerating the tree
typedef tree_enum_result(*tree_enum_func)(struct treenode *);


#define tree_alloc(a_valuesize, a_pool) _tree_alloc(a_valuesize, a_pool, __FILE__, __LINE__)
void tree_free(struct tree *a_tree);
int tree_count(struct tree *a_tree);
#define tree_insert(a_tree, a_key, a_value) _tree_insert(a_tree, a_key, a_value, __FILE__, __LINE__)
struct treenode *tree_set(struct tree *a_tree, int a_key, void *a_value);
struct treenode *tree_set_or_insert(struct tree *a_tree, int a_key, void *a_value);
void tree_remove_subtree(struct treenode *a_node);
void tree_remove(struct treenode *a_node);
int tree_find_remove_subtree(struct tree *a_tree, int a_key);
int tree_find_remove(struct tree *a_tree, int a_key);
struct treenode *tree_first(struct tree *a_tree);
struct treenode *tree_last(struct tree *a_tree);
struct treenode *treenode_left(struct treenode *a_node);
struct treenode *treenode_right(struct treenode *a_node);
struct treenode *treenode_parent(struct treenode *a_node);
int treenode_key(struct treenode *a_node);
void *treenode_value(struct treenode *a_node);
struct treenode *tree_find(struct tree *a_tree, int a_key);
tree_enum_result tree_enum(struct tree *a_tree, tree_enum_func a_funcptr);
tree_enum_result tree_enum_rev(struct tree *a_tree, tree_enum_func a_funcptr);



struct tree *_tree_alloc(int a_valuesize, struct memorypool *a_pool, const char *a_file, int a_line);
void tree_free(struct tree *a_tree);
int tree_count(struct tree *a_tree);
struct treenode *_tree_insert(struct tree *a_tree, int a_key, void *a_value, const char *a_file, int a_line);
struct treenode *tree_set(struct tree *a_tree, int a_key, void *a_value);
struct treenode *tree_set_or_insert(struct tree *a_tree, int a_key, void *a_value);
void tree_remove_subtree(struct treenode *a_node);
void tree_remove(struct treenode *a_node);
int tree_find_remove_subtree(struct tree *a_tree, int a_key);
int tree_find_remove(struct tree *a_tree, int a_key);
struct treenode *tree_first(struct tree *a_tree);
struct treenode *tree_last(struct tree *a_tree);
struct treenode *treenode_left(struct treenode *a_node);
struct treenode *treenode_right(struct treenode *a_node);
struct treenode *treenode_parent(struct treenode *a_node);
int treenode_key(struct treenode *a_node);
void *treenode_value(struct treenode *a_node);
struct treenode *tree_find(struct tree *a_tree, int a_key);
tree_enum_result tree_enum(struct tree *a_tree, tree_enum_func a_funcptr);
tree_enum_result tree_enum_rev(struct tree *a_tree, tree_enum_func a_funcptr);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __h_tree

// -- EOF

