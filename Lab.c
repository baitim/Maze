#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024
#define PPOS(x, y) (lab + W * (y) + (x))
#define LPOS(x, y) *(lab + W * (y) + (x))
#define VALID(dx, dy) ((dx) != 0 || (dy) != 0)
#define LENGTH(dx, dy) ((dx) == 0 ? 10 : (dy) == 0 ? 10 : 14)
#define INBOUNDS(x, y) ((x) > 1 && (x) < W - 1 && (y) > 0 && (y) < H - 1)

typedef struct _queue {
    int x;
    int y;
    struct _queue *next;
} Queue;

int *lab;
int W, H;
int ix, iy;
int ox, oy;

int enqueue(Queue **q, int x, int y);
int dequeue(Queue **q, int *x, int *y);
int step(int x, int y, Queue **q);
int print_lab();
int read_lab(FILE *f);
int find_shortest_path(Queue **q);
void is_step_par(int *x, int *y);
int is_par();
void print_par();
int dtor_queue(Queue *q, Queue *head);

int enqueue(Queue **q, int x, int y)
{
    Queue *n = malloc(sizeof(Queue));
    if (!*q) {
        *n = (Queue) {x ,y, n};
        *q = n;
    }
    else {
        *n = (Queue) {x, y, (*q)->next};
        (*q)->next = n;
        *q = (*q)->next;
    }
    return 0;
}

int dequeue(Queue **q, int *x, int *y)
{
    Queue *n;
    if (!*q) return 1;
    n = (*q)->next;
    *x = n->x;
    *y = n->y;
    if (*q == (n))
        return (free(*q), *q = NULL, 0);
    else    
        return ((*q)->next = n->next, free(n), 0);
}

int step(int x, int y, Queue **q)
{
    int dx, dy;
    int l = LPOS(x, y);
    int *n, dl;
    for (dx = -1; dx <= 1; dx++) {
        for (dy = -1; dy <= 1; dy++) {
            if (VALID(dx, dy) && INBOUNDS(x + dx, y + dy)) {
                n = PPOS(x + dx, y + dy);
                dl = LENGTH(dx, dy);
                if (*n == -1) continue;
                if (*n == 0 || *n > l + dl) {
                    *n = l + dl;
                    enqueue(q, x + dx, y + dy);
                }
            }
        }
    }
}

int find_shortest_path(Queue **q)
{
    LPOS(ix, iy) = 1;
    enqueue(q, ix, iy);
    while (*q) {
        int x, y;
        if (!dequeue(q, &x, &y)) {
            if (x == ox && y == oy) { return LPOS(x, y) - 1; }
            step(x, y, q);
        } else {
            return -1;
        }
    }
}

int read_lab(FILE *f) {
    char buf[MAX_LINE];
    int y = 0;
    W = -1;
    H = 0;
    lab = NULL;
    while (fgets(buf, MAX_LINE, f)) {
        int x = 0;
        int end = strchr(buf, '\n') - buf;
        if (W < 0 && end > 0) W = end;
        lab = (int *)realloc(lab, W * (H + 1) * sizeof(int));
        char *s;
        for (x = 0, s = buf; *s != 0 && *s != '\n'; x++, s++) {
            switch (*s) {
                case '#': LPOS(x, H) = -1; break;
                case 'o': LPOS(x, H) = 0; ox = x; oy = H; break;
                case 'i': LPOS(x, H) = 0; ix = x; iy = H; break;
                default:  LPOS(x, H) = 0; break;
            }
        }
        while (x < W) LPOS(x++, H) = -1;
        H++;
    }
    return 0;
}

int print_lab()
{
    int x, y;
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            if (LPOS(x, y) == -1) printf("#");
            else if (x == ix && y == iy) printf("I");
            else if (x == ox && y == oy) printf("O");
            else if (LPOS(x, y) == 0) printf(" ");
            else printf("\033[3%dm0\033[0m", ((LPOS(x, y) / 10) % 5 + 1));
        }
        putchar('\n');
    }
    putchar('\n');
}

void is_step_par(int *x, int *y)
{
    int dx, dy;
    for (dx = -1; dx <= 1; dx++) {
        for (dy = -1; dy <= 1; dy++) {
            if (LPOS(*x + dx, *y + dy) != -1 && VALID(dx, dy) && INBOUNDS(*x + dx, *y + dy)) {
                if (LPOS(*x + dx, *y + dy) + LENGTH(dx, dy) - LPOS(*x, *y) == 0) {
                    *x += dx;
                    *y += dy;
                    return;
                }
            }
        }
    }
}

int is_par()
{
    int x = ox, y = oy;
    while (x != ix || y != iy) {
        int old_x = x;
        int old_y = y;
        is_step_par(&x, &y);
        *PPOS(old_x, old_y) = -777;
        if (old_x == x && old_y == y) return 0;
    }
    return 1;
}

void print_par() 
{
    if (!is_par()) return;
    int x, y;
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            if (LPOS(x, y) == -1) printf("#");
            else if (x == ix && y == iy) printf("I");
            else if (x == ox && y == oy) printf("O");
            else if (LPOS(x, y) == -777) printf(".");
            else printf(" ");
        }
        putchar('\n');
    }
    putchar('\n');
}

int dtor_queue(Queue *q, Queue *head)
{
    if (!q || q->next == head) return 1;
    dtor_queue(q->next, head);
    free(q);
    return 1;
}

int main()
{
    Queue *q = NULL;
    FILE *f = fopen("lab.txt", "r");
    if (!f) return -fprintf(stderr, "Cannot open file\n");
    if (read_lab(f)) { fclose(f); return -fprintf(stderr, "Cannot read labirynth\n"); }
    print_lab();
    int sp = find_shortest_path(&q);
    print_lab();
    print_par();
    if (sp == -1)   fprintf(stderr, "Cannot find path\n");
    else            printf("short %d\n", sp);
    dtor_queue(q, q);
    free(lab);
    fclose(f);
}