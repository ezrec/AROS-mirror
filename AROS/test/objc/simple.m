/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 *
 * Trivial Objective-C test
 */

#import <stdio.h>

#import "food.h"

int main(int argc, char **argv)
{
    const int kcal = 190;
    Food *beer = [Food new];

    [beer calories:kcal];

    printf("Beer has %d calories, expected %d\n", [beer calories], kcal);

    return 0;
}
