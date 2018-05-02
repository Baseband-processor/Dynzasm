#include "db.h"

struct db_node *db_node_init(struct db_node *parent)
{
	struct db_node *node = malloc(sizeof(struct db_node));

	node->keys = NULL, node->child = NULL;
	node->num_keys = 0, node->num_child = 0;
	node->leaf = 1;
	node->parent = parent;

	return node;
}

void db_node_destroy(struct db_node *node)
{
	if (!node) return;

	for (int i = 0; i < node->num_child; i++) {
		db_node_destroy(node->child[i]);
	}
	free(node->child);
	free(node->keys);
	free(node);
}

void db_node_print(struct db_node *root, int indent)
{
	if (!root) return;
	for (int i = 0; i < indent; i++)
		printf("\t");
	for (int i = 0; i < root->num_keys; i++) {
		printf("(");
		db_key_print(root->keys[i]);
		printf("), ");
	}
	printf("\n");
	for (int i = 0; i < root->num_child; i++) {
		db_node_print(root->child[i], indent+1);
		printf("\n");
	}
}

void db_key_print(struct db_key k)
{
	int ks = k.ksize;
	for (int j = 0; j < 1; j++)
		printf("%02x", ((char*)k.key)[j]);
}

struct db_node *db_lookup(struct db_node *node, struct db_key key)
{
	if (node->leaf) return node;
	for (int i = 0; i < node->num_keys; i++) {
		if (CMP(key, node->keys[i]) < 0)
			return db_lookup(node->child[i], key);
	}
	return db_lookup(node->child[node->num_child-1], key);
}

/*
struct db_node *db_lookup(struct db_node *root, struct db_key key)
{
	if (!root) return NULL;
	struct db_node *node = root;
	int found = 0;
	while (node && !node->leaf) {
		found = 0;
		for (int i = 0; i < node->num_keys; i++) {
			if (CMP(key, node->keys[i]) < 0) {
				node = node->child[i];
				found = 1;
				break;
			} else if (!CMP(key, node->keys[i])) {
				node = node->child[i+1];
				found = 1;
				break;
			}
			if ((i+1)==node->num_keys && CMP(key, node->keys[i]) > 0) {
				node = node->child[i+1];
				found = 1;
				break;
			}
		}
		if (!found) {
			break;
			node = node->child[node->num_child-1];
		}
	}
	return node;
}
*/

struct db_node *db_insert(struct db_node *root, struct db_key key)
{
	struct db_node * closest = db_lookup(root, key);
	struct db_node *nroot = root;
	if (closest)
		nroot = db_insert_internal(closest, CPY(key));
	return !nroot->parent ? nroot : root;
}

struct db_node *db_insert_internal(struct db_node *closest, struct db_key key)
{
	if (closest->num_keys < MAX_KEYS) {
		db_addkey(closest, key);
		return closest;
	} else if (!closest->parent) {
		db_addkey(closest, key);
		struct db_node *left = db_node_init(NULL);
		struct db_node *right = db_node_init(NULL);
		key = CPY(db_node_split(closest, left, right));
		struct db_node *newroot = db_node_init(NULL);
		db_addkey(newroot, key);
		db_addchild(newroot, left);
		db_addchild(newroot, right);
		return newroot;
	} /*else if (closest->parent->num_keys < MAX_KEYS) {
		db_addkey(closest, key);
		struct db_node *left = db_node_init(NULL);
		struct db_node *right = db_node_init(NULL);
		struct db_node *parent = closest->parent;
		db_remchild(parent, closest);
		key = CPY(db_node_split(closest, left, right));
		db_addchild(parent, left);
		db_addchild(parent, right);
		return db_insert_internal(parent, key);
	} */else {
		db_addkey(closest, key);
		struct db_node *left = db_node_init(NULL);
		struct db_node *right = db_node_init(NULL);
		struct db_node *parent = closest->parent;
		db_remchild(parent, closest);
		key = CPY(db_node_split(closest, left, right));
		db_addchild(parent, left);
		db_addchild(parent, right);
		return db_insert_internal(parent, key);
	}
	return closest;
}

struct db_key db_node_split(struct db_node *node, struct db_node *left, struct db_node *right)
{
	int median = node->num_keys/2;
	int high = node->num_keys;
	struct db_key * okeys = node->keys;
	struct db_key mkey = okeys[median];
	struct db_node ** ochild = node->child;
	int nochild = node->num_child;

	int wasleaf = node->leaf;
	node->child = NULL;
	node->num_child = 0;
	node->keys = NULL;
	node->num_keys = 0;
	db_node_destroy(node);

	for (int i = 0; i < high; i++) {
		if (i > median)
			db_addkey(right, okeys[i]);
		else if (i < median)
			db_addkey(left, okeys[i]);
	}
	if (wasleaf)
		db_addkey(right, okeys[median]);
	for (int i = 0; i < nochild; i++) {
		if (i > median)
			db_addchild(right, ochild[i]);
		else if (i <= median)
			db_addchild(left, ochild[i]);
	}

	free(okeys);
	free(ochild);

	return mkey;
}

void db_addchild(struct db_node *node, struct db_node *child)
{
	int idx = 0;
	while ((idx < node->num_child) && (CMP(child->keys[0], node->child[idx]->keys[0]) > 0)) idx++;
	node->leaf = 0;
	child->parent = node;
	node->num_child++;
	if (!node->child)
		node->child = malloc(sizeof(struct db_node*));
	else
		node->child = realloc(node->child, sizeof(struct db_node*)*node->num_child);
	int size = (node->num_child - (idx+1))*sizeof(struct db_node*);
	if (idx < (node->num_child-1))
		memmove(node->child+idx+1, node->child+idx, size);
	node->child[idx] = child;
}

void db_remchild(struct db_node *node, struct db_node *child)
{
	if (!node) return;
	int idx = -1;
	for (int i = 0; i < node->num_child; i++) {
		if (node->child[i] == child) {
			idx = i;
			break;
		}
	}
	if (idx < 0) return;
	int size = ((node->num_child-1)-idx) * sizeof(struct db_node*);
	if (idx < node->num_child)
		memmove(node->child+idx,node->child+idx+1, size);
	node->num_child--;
	if (!node->num_child) {
		node->leaf = 1;
		free(node->child);
		node->child = NULL;
	} else {
		node->child = realloc(node->child, sizeof(struct db_node*)*node->num_child);
	}
}

void db_addkey(struct db_node *node, struct db_key key)
{
	int idx = 0;
	while ((idx < node->num_keys) && (CMP(key, node->keys[idx]) >= 0)) idx++;
	node->num_keys++;
	if (!node->keys)
		node->keys=malloc(sizeof(struct db_key));
	else
		node->keys=realloc(node->keys, sizeof(struct db_key)*node->num_keys);
	int size = (node->num_keys - (idx+1))*sizeof(struct db_key);
	if (idx < (node->num_keys-1))
		memmove(node->keys+idx+1, node->keys+idx, size);
	node->keys[idx] = key;
}

struct db_key db_key_copy(struct db_key key)
{
	struct db_key cpy;
	cpy.key = malloc(key.ksize);
	cpy.ksize = key.ksize;
	memcpy(cpy.key, key.key, key.ksize);
	return cpy;
}

int db_key_compare(struct db_key k1, struct db_key k2)
{
	int result = 0;
	int len = k1.ksize < k2.ksize ? k1.ksize : k2.ksize;
	int diff = 0;
	for (int i = 0; i < len; i++) {
		diff = ((unsigned char*)k1.key)[i] - ((unsigned char*)k2.key)[i];
		if (diff != 0) {
			result = diff;
			break;
		}
	}
	if (result == 0)
		result = k1.ksize > k2.ksize ? 1 : k1.ksize == k2.ksize ? 0 : -1;
	return result;
}
