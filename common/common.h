#ifndef __COMMON_H
#define __COMMON_H

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

int safe_strtol(const char *s) {
    char *endptr;

    errno = 0;
    long val = strtol(s, &endptr, 0);

    /* Check for various possible errors */

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
        perror("strtol");
        return -1;
    }

    if (*endptr != '\0')        /* Not necessarily an error... */
        printf("Further characters after number: %s\n", endptr); 

    return val;
}

#endif /* __COMMON_H */
