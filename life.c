#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#define ALIVE '@'
#define DEAD  '.'

int a(int, int, int);
int m(int,int,int);


/* Neighbours of (x,y) in generation g. */

int n( int x, int y, int g )
{
        return m( x - 1, y - 1, g )
             + m( x,     y - 1, g )
             + m( x + 1, y - 1, g )
             + m( x - 1, y,     g )
             + m( x + 1, y,     g )
             + m( x - 1, y + 1, g )
             + m( x,     y + 1, g )
             + m( x + 1, y + 1, g );
}


/* Dictionary key for (x,y) in generation g. */

char *i(int x, int y, int g)
{
        static char s[256];

        sprintf(s, "%d,%d,%d", x, y, g);

        return strdup(s);
}


/* An empty cell becomes alive if it has 3 neighbours,
   and a living cell survives if it has 2 neighbours.   */

int a(int x, int y, int g)
{
        return g == 0 ? 0 :
                n(x, y, g-1) == 3 ||
                n(x, y, g-1) == 2 && m(x, y, g-1);
}


/* Memoizes the results of a(). */

int m(int x, int y, int g)
{
        ENTRY e, *ep;

        e.key = i(x, y, g);
        ep = hsearch(e, FIND);
        if (!ep) {
                e.data = (void *)(intptr_t)a(x, y, g);
                ep = hsearch(e, ENTER);
                if (!ep) {
                        perror("m");
                        exit(EXIT_FAILURE);
                }
        }
        else {
                free(e.key);
        }

        return (int)(intptr_t)(ep->data);
}


/* Display generation G, starting from X, Y on a WxH screen.
d(g, x, y, w, h) =
        for i = 0 to w
                for j = 0 to h
                        pixel(i, j) = a(i+x, j+y)
*/
void d (char *s, int g, int x, int y, int w, int h)
{
        int i, j;

        printf("\x1b[2J\x1b[1;1H");
        printf("=== Generation: %d =================\n", g);

        for (j = 0; j < h; j++) {
                for (i = 0; i < w; i++) {
                        putchar(m(i+x, j+y, g) ? ALIVE : DEAD);
                        //s[j*(w+1)+i] = m(i+x, j+y, g) ? ALIVE : DEAD;
                }
                //s[j*(w+1)+i] = '\n';
                putchar('\n');
        }
        //s[j*(w+1)] = '\0';
        fflush(stdout);
}


#define WIDTH  20
#define HEIGHT 20
#define NEL 1000000

int main (int argc, char *argv)
{
        int x = 0, y = 0, g = 0, c;
        char v[HEIGHT*(WIDTH+1)+1];
        ENTRY e;

        hcreate(NEL);

        while ((c=getchar()) != EOF) {
                switch (c) {
                        case '\n':
                                x=0; y++;
                                break;
                        case ALIVE:
                                e.key = i(x, y, g);
                                e.data = (void *) 1;
                                hsearch(e, ENTER);
                                // fall through
                        case DEAD:
                                x++;
                                break;
                }
        }

        for(;;) {
                d(v, g, 0, 0, WIDTH, HEIGHT);
                sleep(1);
                g++;
        }

        hdestroy();
}
