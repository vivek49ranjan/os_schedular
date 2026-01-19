void left_rotate(rb_node **root, rb_node *x) {
    rb_node *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void right_rotate(rb_node **root, rb_node *y) {
    rb_node *x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    if (!y->parent) *root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

void fix_insert(rb_node **root, rb_node *z) {
    while (z != *root && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            rb_node *uncle = z->parent->parent->right;
            if (uncle && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(root, z->parent->parent);
            }
        } else {
            rb_node *uncle = z->parent->parent->left;
            if (uncle && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;
}

void rb_insert(rb_node **root, process *p) {
    rb_node *z = (rb_node*)malloc(sizeof(rb_node));
    z->vruntime = p->vruntime;
    z->task = p;
    z->left = z->right = z->parent = NULL;
    z->color = RED;

    rb_node *y = NULL;
    rb_node *x = *root;

    while (x) {
        y = x;
        if (z->vruntime < x->vruntime) x = x->left;
        else x = x->right;
    }

    z->parent = y;
    if (!y) *root = z;
    else if (z->vruntime < y->vruntime) y->left = z;
    else y->right = z;

    fix_insert(root, z);
}
