#include "date.h"
#include "tldlist.h"
#include <stdio.h>
#include <string.h>

#define USAGE "usage: %s begin_datestamp end_datestamp [file] ...\n"

static void process(FILE *fd, TLDList *tld) {
    char bf[1024], sbf[1024];
    Date *d;
    while (fgets(bf, sizeof(bf), fd) != NULL) {
        char *q, *p = strchr(bf, ' ');
        if (! p) {
            fprintf(stderr, "Illegal input line: %s", bf);
            return;
        }
        strcpy(sbf, bf);
        *p++ = '\0';
        while (*p == ' ')
            p++;
        q = strchr(p, '\n');
        if (! q) {
            fprintf(stderr, "Illegal input line: %s", sbf);
            return;
        }
        *q = '\0';
        d = date_create(bf);
        (void) tldlist_add(tld, p, d);
        date_destroy(d);
    }
}

int main(int argc, char *argv[]) {
    Date *begin, *end;
    int i;
    FILE *fd;
    TLDList *tld;
    TLDIterator *it;
    TLDNode *n;
    double total;
    
    if (argc < 3) {
        fprintf(stderr, USAGE, argv[0]);
        return -1;
    }
    begin = date_create(argv[1]);
    if (! begin) {
        fprintf(stderr, USAGE, argv[0]);
        return -1;
    }
    end = date_create(argv[2]);
    if (! end) {
        fprintf(stderr, USAGE, argv[0]);
        return -1;
    }
    tld = tldlist_create(begin, end);
    if (! tld) {
        fprintf(stderr, "Unable to create TLD list\n");
        return -2;
    }
    if (argc == 3)
        process(stdin, tld);
    else {
        for (i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0)
                fd = stdin;
            else
                fd = fopen(argv[i], "r");
            if (! fd) {
                fprintf(stderr, "Unable to open %s\n", argv[i]);
                continue;
            }
            process(fd, tld);
            if (fd != stdin)
                fclose(fd);
        }
    }
    total = (double)tldlist_count(tld);
    it = tldlist_iter_create(tld);
    if (! it) {
        fprintf(stderr, "Unable to create iterator\n");
        return -2;
    }
    while ((n = tldlist_iter_next(it))) {
        printf("%6.2f %s\n", 100.0 * (double)tldnode_count(n)/total, tldnode_tldname(n));
    }
    tldlist_iter_destroy(it);
    tldlist_destroy(tld);
    date_destroy(begin);
    date_destroy(end);
    return 0;
}
