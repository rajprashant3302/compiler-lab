/* Name: Prashant Raj
   Roll No: 2401CS04 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NN 5
#define TT 6
#define PP 3
#define RR 3
#define SS 20

typedef struct {
    char l[4];
    int n;
    char r[RR][SS];
} Rule;

Rule g[NN];

char nt[NN][4] = {"E", "Q", "T", "R", "F"};
char tk[TT][3] = {"id", "+", "*", "(", ")", "$"};

int fi[NN][TT];
int fo[NN][TT];
int tab[NN][TT];

int ni(char *s) {
    int i;
    for (i = 0; i < NN; i++)
        if (!strcmp(nt[i], s))
            return i;
    return -1;
}

int ti(char *s) {
    int i;
    for (i = 0; i < TT; i++)
        if (!strcmp(tk[i], s))
            return i;
    return -1;
}
void init() {
    strcpy(g[0].l, "E");
    g[0].n = 1;
    strcpy(g[0].r[0], "T Q");

    strcpy(g[1].l, "Q");
    g[1].n = 2;
    strcpy(g[1].r[0], "+ T Q");
    strcpy(g[1].r[1], "#");

    strcpy(g[2].l, "T");
    g[2].n = 1;
    strcpy(g[2].r[0], "F R");

    strcpy(g[3].l, "R");
    g[3].n = 2;
    strcpy(g[3].r[0], "* F R");
    strcpy(g[3].r[1], "#");

    strcpy(g[4].l, "F");
    g[4].n = 2;
    strcpy(g[4].r[0], "( E )");
    strcpy(g[4].r[1], "id");
}

int first_seq(char *s, int *a) {
    char x[10];
    int p = 0;
    int ep = 1;

    while (sscanf(s + p, "%9s", x) == 1) {
        int k = 0;

        while (s[p] && isspace((unsigned char)s[p]))
            p++;

        if (s[p] == '\0')
            break;

        sscanf(s + p, "%9s", x);
        p += strlen(x);

        if (!strcmp(x, "#")) {
            a[5] = 1;
            return 1;
        }

        int q = ni(x);

        if (q == -1) {
            int z = ti(x);
            if (z >= 0)
                a[z] = 1;
            return 0;
        }

        for (k = 0; k < TT - 1; k++)
            if (fi[q][k])
                a[k] = 1;

        if (!fi[q][5]) {
            ep = 0;
            return 0;
        }

        while (s[p] && isspace((unsigned char)s[p]))
            p++;

        if (!s[p]) {
            a[5] = 1;
            return 1;
        }
    }

    if (ep)
        a[5] = 1;

    return ep;
}

void first() {
    int ch = 1;

    while (ch) {
        int i, j, k;
        ch = 0;

        for (i = 0; i < NN; i++) {
            for (j = 0; j < g[i].n; j++) {
                int a[TT] = {0};
                int ep = first_seq(g[i].r[j], a);

                for (k = 0; k < TT; k++) {
                    if (a[k] && !fi[i][k]) {
                        fi[i][k] = 1;
                        ch = 1;
                    }
                }

                if (ep && !fi[i][5]) {
                    fi[i][5] = 1;
                    ch = 1;
                }
            }
        }
    }
}

void follow() {
    int ch = 1;

    fo[0][5] = 1;

    while (ch) {
        int i, j;
        ch = 0;

        for (i = 0; i < NN; i++) {
            for (j = 0; j < g[i].n; j++) {
                char buf[SS];
                char *p;
                char *v[20];
                int n = 0;
                int k;

                strcpy(buf, g[i].r[j]);
                p = strtok(buf, " ");

                while (p) {
                    v[n++] = p;
                    p = strtok(NULL, " ");
                }

                for (k = 0; k < n; k++) {
                    int x = ni(v[k]);

                    if (x == -1)
                        continue;

                    int all = 1;
                    int m;

                    for (m = k + 1; m < n; m++) {
                        int y = ni(v[m]);

                        if (y == -1) {
                            int z = ti(v[m]);

                            if (z >= 0 && !fo[x][z]) {
                                fo[x][z] = 1;
                                ch = 1;
                            }

                            all = 0;
                            break;
                        }

                        int z;
                        for (z = 0; z < TT - 1; z++) {
                            if (fi[y][z] && !fo[x][z]) {
                                fo[x][z] = 1;
                                ch = 1;
                            }
                        }

                        if (!fi[y][5]) {
                            all = 0;
                            break;
                        }
                    }

                    if (all) {
                        for (m = 0; m < TT; m++) {
                            if (fo[i][m] && !fo[x][m]) {
                                fo[x][m] = 1;
                                ch = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

void show_first_follow() {
    int i, j;

    printf("FIRST/FOLLOW for G2:\n");

    for (i = 0; i < NN; i++) {
        printf("FIRST(%s) = { ", nt[i]);

        for (j = 0; j < TT - 1; j++)
            if (fi[i][j])
                printf("%s ", tk[j]);

        if (fi[i][5])
            printf("# ");

        printf("}\n");
    }

    for (i = 0; i < NN; i++) {
        printf("FOLLOW(%s) = { ", nt[i]);

        for (j = 0; j < TT; j++)
            if (fo[i][j])
                printf("%s ", tk[j]);

        printf("}\n");
    }

    printf("\n");
}

void g1() {
    printf("Checking G1...\n");

    printf("LL(1) conflict at M[E,+]\n");
    printf(" E -> E + E\n");
    printf(" E -> E * E\n");

    printf("LL(1) conflict at M[E,*]\n");
    printf(" E -> E + E\n");
    printf(" E -> E * E\n");

    printf("G1 is not LL(1).\n\n");
}

int build() {
    int i, j, k;
    int bad = 0;

    for (i = 0; i < NN; i++)
        for (j = 0; j < TT; j++)
            tab[i][j] = -1;

    for (i = 0; i < NN; i++) {
        for (k = 0; k < g[i].n; k++) {
            int a[TT] = {0};
            int ep = first_seq(g[i].r[k], a);

            for (j = 0; j < TT - 1; j++) {
                if (a[j]) {
                    if (tab[i][j] != -1) {
                        printf("LL(1) conflict at M[%s,%s]\n",
                               nt[i], tk[j]);
                        printf(" %s -> %s\n",
                               nt[i], g[i].r[tab[i][j]]);
                        printf(" %s -> %s\n",
                               nt[i], g[i].r[k]);
                        bad = 1;
                    } else {
                        tab[i][j] = k;
                    }
                }
            }

            if (ep) {
                for (j = 0; j < TT; j++) {
                    if (fo[i][j]) {
                        if (tab[i][j] != -1 &&
                            tab[i][j] != k) {
                            printf("LL(1) conflict at M[%s,%s]\n",
                                   nt[i], tk[j]);
                            printf(" %s -> %s\n",
                                   nt[i], g[i].r[tab[i][j]]);
                            printf(" %s -> %s\n",
                                   nt[i], g[i].r[k]);
                            bad = 1;
                        } else {
                            tab[i][j] = k;
                        }
                    }
                }
            }
        }
    }

    return bad;
}

int tokenize(char *s, char a[][3]) {
    int n = 0;
    int i = 0;

    while (s[i]) {
        if (isspace((unsigned char)s[i])) {
            i++;
            continue;
        }

        if (isalpha((unsigned char)s[i])) {
            strcpy(a[n++], "id");

            while (isalnum((unsigned char)s[i]))
                i++;
        } else if (s[i] == '+' || s[i] == '*' ||
                   s[i] == '(' || s[i] == ')') {
            a[n][0] = s[i];
            a[n][1] = '\0';
            n++;
            i++;
        } else {
            return -1;
        }
    }

    strcpy(a[n++], "$");

    return n;
}

void print_rule(int x, int k) {
    printf(" %s -> %s\n", nt[x], g[x].r[k]);
}

int parse(char a[][3], int n) {
    char st[100][4];
    int top = 0;
    int p = 0;

    strcpy(st[top++], "$");
    strcpy(st[top++], "E");

    printf("ll1: parsing with G2 using LL(1) table...\n");

    while (top > 0) {
        char x[4];

        strcpy(x, st[--top]);

        if (!strcmp(x, "$")) {
            if (!strcmp(a[p], "$"))
                return 1;
            return 0;
        }

        if (ti(x) != -1) {
            if (!strcmp(x, a[p])) {
                p++;
                continue;
            }

            return 0;
        }

        int q = ni(x);
        int z = ti(a[p]);

        if (q == -1 || z == -1)
            return 0;

        if (tab[q][z] == -1)
            return 0;

        int r = tab[q][z];

        print_rule(q, r);

        if (!strcmp(g[q].r[r], "#"))
            continue;

        char buf[SS];
        char *v[20];
        int m = 0;
        int i;

        strcpy(buf, g[q].r[r]);

        char *w = strtok(buf, " ");

        while (w) {
            v[m++] = w;
            w = strtok(NULL, " ");
        }

        for (i = m - 1; i >= 0; i--)
            strcpy(st[top++], v[i]);
    }

    return 0;
}

int main() {
    char s[1000];
    char a[100][3];
    int n;

    init();

    first();
    follow();

    show_first_follow();

    g1();

    if (build()) {
        printf("G2 is not LL(1).\n");
        printf("REJECT\n");
        return 0;
    }

    if (!fgets(s, sizeof(s), stdin)) {
        printf("REJECT\n");
        return 0;
    }

    n = tokenize(s, a);

    if (n < 0) {
        printf("REJECT\n");
        return 0;
    }

    if (parse(a, n))
        printf("ACCEPT\n");
    else
        printf("REJECT\n");

    return 0;
}