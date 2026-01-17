void rb_insert(rb_node **root, process *p)
{
    rb_node *z = malloc(sizeof(rb_node));
    z->vruntime = p->vruntime;
    z->task = p;
    z->left = z->right = z->parent = NULL;
    z->color = RED;

    rb_node *y = NULL;
    rb_node *x = *root;

    while (x) {
        y = x;
        if (z->vruntime < x->vruntime)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;
    if (!y)
        *root = z;
    else if (z->vruntime < y->vruntime)
        y->left = z;
    else
        y->right = z;

    rb_insert_fixup(root, z);
}
