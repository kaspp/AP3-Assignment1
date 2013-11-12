#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"

struct date {
    int d;
    int m;
    int y;
};

Date *date_create(char *datestr) {
    
    //date format: DD/MM/YYYY
    Date *d = (Date *) malloc(sizeof(Date));
    char * dmy = strtok(datestr, "/");
    int count = 0;
    
    while (dmy != NULL) {
        switch (count) {
            case 0:
                d->d = atoi(dmy);
                break;
                
            case 1:
                d->m = atoi(dmy);
                break;
                
            case 2:
                d->y = atoi(dmy);
                break;
                
        }
        
        dmy = strtok(NULL, "/");
        count++;
    }
    
    int leapyear = 2012;
    int checky;
   
    //from 1900 to 2099: total of around 200 years
    if (d->y > 1900 && d->y < 2099) {
        if ((d->m > 0) && (d->m < 13)) {
            switch (d->m) {
                    //for days with 31 days
                case 1:
                case 3:
                case 5:
                case 7:
                case 8:
                case 10:
                case 12:
                 
                    if (d->d > 0 && d->d < 32 ) {
                        return d;
                    }
                    break;
                    //for feb
                case 2:
                
                    checky = d->y - leapyear;
                    
                    if (checky % 4 == 0) {
                        if (d->d > 0 && d->d < 30 ) {
                            return d;
                        }
                    } else {
                        if (d->d > 0 && d->d < 29 ) {
                            return d;
                        }
                    }
                    break;
                    
                    //for days with 30 days
                case 4:
                case 6:
                case 9:
                case 11:
                   
                    if (d->d > 0 && d->d < 31 ) {
                        return d;
                    }
                    break;
                }
            }
        
        } else {
            //date_destroy(d);
            return NULL;
        }
    //date_destroy(d);
    return NULL;
}


Date *date_duplicate(Date *d) {
    
    Date *dup = (Date *) malloc (sizeof(d));
    memcpy(dup, d, sizeof(*dup));
    return dup;
    
}


int date_compare(Date *date1, Date *date2) {
    int checksum = 0;
    // check if it is the same year
    if (date1->y == date2->y) {
        // check if the month is the same
        if (date1->m == date2->m) {
            
            // check if the day is the same
            if (date1->d == date2->d) {
                checksum = 0;
            } else if (date1->d > date2->d) {
                checksum = 1;
            } else if (date1->d < date2->d) {
                checksum = -1;
            }
        
        } else if (date1->m > date2->m) {
            checksum = 1;
        } else if (date1->m < date2->m) {
            checksum = -1;
        }

    } else if (date1->y > date2->y) {
        checksum = 1;
    } else if (date1->y < date2->y) {
        checksum = -1;
    }
    
    return checksum;
}



void date_destroy(Date *d) {
    if (d != NULL) {
        free(d);
    }
}