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

@implementation Food
- (int) calories {
    return calories;
}

- (id) calories: (int)kcal {
    calories = kcal;

    return self;
}
@end
