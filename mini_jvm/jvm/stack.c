

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jvm.h"
#include "garbage.h"


/* Stack Initialization */
StackFrame *stack_init(s32 entry_size) {
    StackFrame *stack = jvm_alloc(sizeof(StackFrame));
    memset(stack, 0, sizeof(StackFrame));
    stack->store = (StackEntry *) jvm_alloc(sizeof(StackEntry) * entry_size);
    stack->size = 0;
    stack->max_size = entry_size;
}

void stack_destory(StackFrame *stack) {
    if (stack->store) {
        jvm_free(stack->store);
    }
}

void push_entry(StackFrame *stack, StackEntry *entry) {
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(tmp, entry, sizeof(StackEntry));
    stack->size++;
}

/* push Integer */
void push_int(StackFrame *stack, s32 value) {
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(tmp, &value, sizeof(s32));
    stack->store[stack->size].type = STACK_ENTRY_INT;
    stack->size++;
}





/* push Double */
void push_double(StackFrame *stack, f64 value) {
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(tmp, &value, sizeof(f64));
    stack->store[stack->size].type = STACK_ENTRY_DOUBLE;
    stack->size++;
}

/* push Float */
void push_float(StackFrame *stack, f32 value) {
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(tmp, &value, sizeof(f32));
    stack->store[stack->size].type = STACK_ENTRY_FLOAT;
    stack->size++;
}


/* pop Integer */
s32 pop_int(StackFrame *stack) {
    stack->size--;
    u8 *tmp = stack->store[stack->size].entry;
    s32 value = 0;
    memcpy(&value, tmp, sizeof(s32));
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    return value;
}
/* push Long */
void push_long(StackFrame *stack, s64 value) {
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(tmp, &value, sizeof(s64));
    stack->store[stack->size].type = STACK_ENTRY_LONG;
    stack->size++;
}
/* pop Long */
s64 pop_long(StackFrame *stack) {
    stack->size--;
    s64 value = 0;
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(&value, tmp, sizeof(s64));
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    return value;
}
/* push Ref */
void push_ref(StackFrame *stack, __refer value) {
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(tmp, &value, sizeof(__refer));
    stack->store[stack->size].type = STACK_ENTRY_REF;
    stack->size++;
}

__refer pop_ref(StackFrame *stack) {
    stack->size--;
    __refer value = 0;
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(&value, tmp, sizeof(__refer));
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    return value;
}
/* pop Double */
f64 pop_double(StackFrame *stack) {
    stack->size--;
    f64 value = 0;
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(&value, tmp, sizeof(f64));
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    return value;
}

/* pop Float */
f32 pop_float(StackFrame *stack) {
    stack->size--;
    f32 value = 0;
    u8 *tmp = stack->store[stack->size].entry;
    memcpy(&value, tmp, sizeof(f32));
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
    return value;
}

/* Pop Stack Entry */
void pop_entry(StackFrame *stack,StackEntry * entry) {
    stack->size--;
    memcpy(entry, stack->store[stack->size].entry, sizeof(StackEntry));
    memset(&stack->store[stack->size], 0, sizeof(StackEntry));
}

/* Entry to Int */
s32 entry_2_int(StackEntry *entry) {
    s32 value = 0;
    memcpy(&value, entry->entry, sizeof(s32));
    return value;
}
s64 entry_2_long(StackEntry *entry) {
    s64 value = 0;
    memcpy(&value, entry->entry, sizeof(s64));
    return value;
}

__refer entry_2_refer(StackEntry *entry) {
    __refer value = 0;
    memcpy(&value, entry->entry, sizeof(__refer));
    return value;
}


s32 is_ref_entry(StackFrame *stack) {
    if (stack->store[stack->size - 1].type == STACK_ENTRY_REF)
        return 1;
    return 0;
}

void peekEntry(StackFrame* stack,StackEntry* entry,int index){
    memcpy(entry, stack->store[index].entry, sizeof(StackEntry));
}