/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef FOOD_H
#define FOOD_H

#import <objc/Object.h>

@interface Food : Object {
    int calories;
}

- (int) calories;
- (id)  calories: (int)kcal;
@end

#endif /* FOOD_H */
