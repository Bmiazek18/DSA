// Niniejszy kod powstał w oparciu o materiały ze strony: [https://cp-algorithms.com/data_structures/treap.html]
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;

typedef struct Node* pnode;
struct Node {
    int id;
    int price;
    int prior;
    long long sum_price;
    int size;
    pnode l, r;
};


long long get_sum(pnode t) { return t ? t->sum_price : 0; }
int get_size(pnode t) { return t ? t->size : 0; }

void update(pnode t) {
    if (t) {
        t->size = 1 + get_size(t->l) + get_size(t->r);
        t->sum_price = (long long)t->price + get_sum(t->l) + get_sum(t->r);
    }
}


void split(pnode t, int key, pnode &l, pnode &r) {
    if (!t) {
        l = r = nullptr;
        return;
    }
    if (t->id <= key) {
        split(t->r, key, t->r, r);
        l = t;
    } else {
        split(t->l, key, l, t->l);
        r = t;
    }
    update(t);
}


void merge(pnode &t, pnode l, pnode r) {
    if (!l || !r) {
        t = l ? l : r;
    } else if (l->prior > r->prior) {
        merge(l->r, l->r, r);
        t = l;
    } else {
        merge(r->l, l, r->l);
        t = r;
    }
    update(t);
}

pnode create_node(int id, int price) {
    pnode res = new Node();
    res->id = id;
    res->price = price;
    res->prior = rand();
    res->sum_price = price;
    res->size = 1;
    res->l = res->r = nullptr;
    return res;
}

long long range_sum(pnode &t, int id_min, int id_max) {
    pnode l, m, r;
    split(t, id_min - 1, l, r);
    split(r, id_max, m, r);

    long long res = get_sum(m);

    merge(r, m, r);
    merge(t, l, r);
    return res;
}

void insert(pnode &t, int id, int price) {
    pnode l, m, r;
    split(t, id - 1, l, r);
    split(r, id, m, r);

    if (m) {
        m->price = price;
    } else {
        m = create_node(id, price);
    }

    merge(l, l, m);
    merge(t, l, r);
}

void erase(pnode &t, int id) {
    pnode l, m, r;
    split(t, id - 1, l, r);
    split(r, id, m, r);
    if (m) delete m;
    merge(t, l, r);
}
void clear_treap(pnode t) {
    if (!t) return;
    clear_treap(t->l);
    clear_treap(t->r);
    delete t;
}
void handle_commands(pnode &root) {
    char op;
    while (scanf(" %c", &op) != EOF) {
        switch (op) {
            case 'A': {
                int id, price;
                if (scanf("%d %d", &id, &price) == 2) {
                    insert(root, id, price);
                }
                break;
            }
            case 'D': {
                int id;
                if (scanf("%d", &id) == 1) {
                    erase(root, id);
                }
                break;
            }
            case 'S': {
                int i_min, i_max;
                if (scanf("%d %d", &i_min, &i_max) == 2) {
                    printf("%lld\n", range_sum(root, i_min, i_max));
                }
                break;
            }
            case 'C': {
                printf("%d\n", get_size(root));
                break;
            }
            default:
                break;
        }
    }
}
int main() {
    srand(time(nullptr));
    pnode root = nullptr;
    char op;

    handle_commands(root);
    clear_treap(root);
    return 0;
}