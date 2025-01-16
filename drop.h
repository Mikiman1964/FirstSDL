#ifndef DROP_H_INCLUDED
#define DROP_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

void ControlDrop(uint32_t I);
void ControlCheese(uint32_t I);
void ControlSpreadCheese(uint32_t I);
bool IsCheeseAround(uint32_t I);
bool IsFieldPosForCheeseFree(uint32_t uPosition);

#endif // DROP_H_INCLUDED
