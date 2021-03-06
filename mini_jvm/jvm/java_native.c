

#include <time.h>
#include "java_native.h"
#include "garbage.h"
#include "jvm_util.h"
#include <math.h>


s32 com_sun_cldc_io_ConsoleOutputStream_write(Runtime *runtime, Class *clazz, c8 *type) {
    s16 ch = runtime->localVariables.item[1].integer;
    printf("%c", ch);
#if _JVM_DEBUG
    printf("com_sun_cldc_io_ConsoleOutputStream_write\n");
#endif
    return 0;
}

s32 java_lang_Class_forName(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *jstr = (Instance *) runtime->localVariables.item[0].refer;
    Class *cl = NULL;
    s32 ret = RUNTIME_STATUS_NORMAL;
    if (jstr) {
        Utf8String *ustr = utf8_create();
        Instance *arr = jstring_get_value_array(jstr);
        unicode_2_utf8(ustr, (u16 *) arr->arr_body, arr->arr_length);
        utf8_replace_c(ustr, ".", "/");
        cl = hashtable_get(classes, ustr);

        if (!cl) {
            load_class(classpath, ustr, classes);
            cl = hashtable_get(classes, ustr);
            class_link(cl);
            class_clinit(cl, runtime);
        }
        if (!cl) {
            Instance *exception = exception_create(JVM_EXCEPTION_CLASSNOTFOUND, runtime);
            push_ref(stack, (__refer) exception);
            ret = RUNTIME_STATUS_EXCEPTION;
        } else
            push_ref(stack, (__refer) cl);
        utf8_destory(ustr);
    } else {
        Instance *exception = exception_create(JVM_EXCEPTION_NULLPOINTER, runtime);
        push_ref(stack, (__refer) exception);
        ret = RUNTIME_STATUS_EXCEPTION;
    }
#if _JVM_DEBUG
    printf("java_lang_Class_forName [%x]\n", cl);
#endif
    return ret;
}

s32 java_lang_Class_newInstance(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Class *cl = (Class *) runtime->localVariables.item[0].refer;
    Instance *ins = NULL;
    if (cl) {
        ins = instance_create(cl);
        instance_init(ins, runtime);
    }
    push_ref(stack, (__refer) ins);
#if _JVM_DEBUG
    printf("java_lang_Class_newInstance  class:[%x] ins:[%x]\n", cl, ins);
#endif
    return 0;
}

s32 java_lang_Class_isInstance(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Class *cl = (Class *) runtime->localVariables.item[0].refer;
    Instance *ins = (Instance *) runtime->localVariables.item[1].refer;
    if (instance_of(cl, ins)) {
        push_int(stack, 1);
    } else {
        push_int(stack, 0);
    }
#if _JVM_DEBUG
    printf("java_lang_Class_isInstance\n");
#endif
    return 0;
}

s32 java_lang_Class_isAssignableFrom(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Class *c0 = (Class *) runtime->localVariables.item[0].refer;
    Class *c1 = (Class *) runtime->localVariables.item[1].refer;

    if (assignable_from(c1, c0)) {
        push_int(stack, 1);
    } else {
        push_int(stack, 0);
    }
#if _JVM_DEBUG
    printf("java_lang_Class_isAssignableFrom\n");
#endif
    return 0;
}

s32 java_lang_Class_isInterface(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Class *c0 = (Class *) runtime->localVariables.item[0].refer;
    Class *c1 = (Class *) runtime->localVariables.item[1].refer;

    if (assignable_from(c1, c0)) {//todo
        push_int(stack, 1);
    } else {
        push_int(stack, 0);
    }
#if _JVM_DEBUG
    printf("java_lang_Class_isAssignableFrom\n");
#endif
    return 0;
}

s32 java_lang_Class_isArray(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Class *cl = (Class *) runtime->localVariables.item[0].refer;
    if (cl) {

        push_int(stack, 1);
    } else {
        push_int(stack, 1);
    }
#if _JVM_DEBUG
    printf("java_lang_Class_isArray\n");
#endif
    return 0;
}

s32 java_lang_Class_getName(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Class *cl = (Class *) runtime->localVariables.item[0].refer;
    if (cl) {
        Instance *ins = jstring_create(cl->name, runtime);
        push_ref(stack, (__refer) ins);
    } else {
        push_ref(stack, NULL);
    }
#if _JVM_DEBUG
    printf("java_lang_Class_getName\n");
#endif
    return 0;
}

s32 java_lang_Double_doubleToLongBits(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
#if _JVM_DEBUG
    printf("java_lang_Double_doubleToLongBits %lf to %lld\n", l2d.d, l2d.l);
#endif
    push_long(stack, l2d.l);
    return 0;
}

s32 java_lang_Double_longBitsToDouble(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
#if _JVM_DEBUG
    printf("java_lang_Double_longBitsToDouble\n");
#endif
    push_double(stack, l2d.d);
    return 0;
}


s32 java_lang_Float_intBitsToFloat(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Int2Float i2f;
    i2f.i = runtime->localVariables.item[0].integer;

#if _JVM_DEBUG
    printf("java_lang_Float_intBitsToFloat r = %f\n", i2f.f);
#endif
    push_float(stack, i2f.f);
    return 0;
}

s32 java_lang_Float_floatToIntBits(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Int2Float i2f;
    i2f.i = runtime->localVariables.item[0].integer;

#if _JVM_DEBUG
    printf("java_lang_Float_floatToIntBits r = %f\n", i2f.i);
#endif
    push_int(stack, i2f.i);
    return 0;
}
s32 java_lang_Math_exp(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    f64 r = exp(l2d.d);
#if _JVM_DEBUG
    printf("java_lang_Math_sin r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}
s32 java_lang_Math_random(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    f64 r = 0.0f;
    s32 i;
    s32 times = 0;
    srand(time(0));
    times = rand() % 100;
    for (i = 0; i < times; i++)
        r = ((f64) rand() / (f64) RAND_MAX);
#if _JVM_DEBUG
    printf("java_lang_Math_random r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}

s32 java_lang_Math_sin(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    f64 r = sin(l2d.d);
#if _JVM_DEBUG
    printf("java_lang_Math_sin r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}


s32 java_lang_Math_cos(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    f64 r = cos(l2d.d);
#if _JVM_DEBUG
    printf("java_lang_Math_cos r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}

s32 java_lang_Math_tan(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    f64 r = tan(l2d.d);
#if _JVM_DEBUG
    printf("java_lang_Math_tan r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}

s32 java_lang_Math_sqrt(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    f64 r = sqrt(l2d.d);
#if _JVM_DEBUG
    printf("java_lang_Math_sqrt r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}

s32 java_lang_Math_ceil(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    f64 r = ceil(l2d.d);
#if _JVM_DEBUG
    printf("java_lang_Math_ceil r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}


s32 java_lang_Math_floor(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    f64 r = floor(l2d.d);
#if _JVM_DEBUG
    printf("java_lang_Math_floor r = %f\n", r);
#endif
    push_double(stack, r);
    return 0;
}

s32 java_lang_Object_getClass(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;
    push_ref(stack, (__refer) ins->obj_of_clazz);
#if _JVM_DEBUG
    printf("java_lang_Object_getClass %d\n", ins);
#endif
    return 0;
}

s32 java_lang_Object_hashCode(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;
    Long2Double l2d;
    l2d.r = ins;
    push_int(stack, l2d.i2l.i1);
#if _JVM_DEBUG
    printf("java_lang_Object_hashCode %llx\n", l2d.l);
#endif
    return 0;
}

s32 java_lang_Object_notify(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;

#if _JVM_DEBUG
    printf("java_lang_Object_notify %d\n", ins);
#endif
    return 0;
}

s32 java_lang_Object_notifyAll(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;

#if _JVM_DEBUG
    printf("java_lang_Object_notifyAll %d\n", ins);
#endif
    return 0;
}

s32 java_lang_Object_wait(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[1].integer;
    l2d.i2l.i0 = runtime->localVariables.item[2].integer;
    threadWait(l2d.l);
#if _JVM_DEBUG
    printf("java_lang_Object_wait %d\n", ins);
#endif
    return 0;
}

s32 java_lang_Runtime_exitInternal(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    s32 status = runtime->localVariables.item[1].integer;
    exit(status);
#if _JVM_DEBUG
    printf("java_lang_Runtime_exitInternal %d\n", status);
#endif
    return 0;
}

s32 java_lang_Runtime_freeMemory(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;

    push_long(stack, MAX_HEAP_SIZE - heap_size);
#if _JVM_DEBUG
    printf("java_lang_Runtime_freeMemory \n");
#endif
    return 0;
}

s32 java_lang_Runtime_totalMemory(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;

    push_long(stack, MAX_HEAP_SIZE);
#if _JVM_DEBUG
    printf("java_lang_Runtime_totalMemory \n");
#endif
    return 0;
}

s32 java_lang_Runtime_gc(Runtime *runtime, Class *clazz, c8 *type) {
    garbage_collect();
#if _JVM_DEBUG
    printf("java_lang_Runtime_gc \n");
#endif
    return 0;
}

s32 java_lang_String_charAt(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *jstr = (Instance *) runtime->localVariables.item[0].refer;
    s32 index = runtime->localVariables.item[1].integer;
    s16 ch = jstring_char_at(jstr, index);
#if _JVM_DEBUG
    printf("java_lang_String_charAt ch = %d\n", ch);
#endif
    push_int(stack, ch);
    return 0;
}

s32 java_lang_String_equals(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *jstr1 = (Instance *) runtime->localVariables.item[0].refer;
    Instance *jstr2 = (Instance *) runtime->localVariables.item[1].refer;
    s32 r = jstring_equals(jstr1, jstr2);
#if _JVM_DEBUG
    printf("java_lang_String_equals r = %f\n", r);
#endif
    push_int(stack, r);
    return 0;
}

s32 java_lang_String_indexOf(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *jstr = (Instance *) runtime->localVariables.item[0].refer;
    u16 ch = runtime->localVariables.item[1].integer;
    s32 r = jstring_index_of(jstr, ch, 0);
#if _JVM_DEBUG
    printf("java_lang_String_indexOf r = %f\n", r);
#endif
    push_int(stack, r);
    return 0;
}

s32 java_lang_String_indexOfFrom(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *jstr = (Instance *) runtime->localVariables.item[0].refer;
    u16 ch = runtime->localVariables.item[1].integer;
    s32 startAt = runtime->localVariables.item[2].integer;
    s32 r = jstring_index_of(jstr, ch, startAt);
#if _JVM_DEBUG
    printf("java_lang_String_indexOfFrom r = %f\n", r);
#endif
    push_int(stack, r);
    return 0;
}

s32 java_lang_String_intern(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *jstr = (Instance *) runtime->localVariables.item[0].refer;

#if _JVM_DEBUG
    printf("java_lang_String_intern \n");
#endif
    push_ref(stack, (__refer) jstr);
    return 0;
}

s32 java_lang_StringBuffer_append(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *jstr = (Instance *) runtime->localVariables.item[0].refer;

#if _JVM_DEBUG
    printf("java_lang_StringBuffer_append \n");
#endif
    push_ref(stack, (__refer) jstr);
    return 0;
}


s32 java_lang_System_arraycopy(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;

    s32 count = runtime->localVariables.item[4].integer;
    s32 dest_start = runtime->localVariables.item[3].integer;
    Instance *dest = (Instance *) runtime->localVariables.item[2].refer;
    s32 src_start = runtime->localVariables.item[1].integer;
    Instance *src = (Instance *) runtime->localVariables.item[0].refer;
    //根据元素宽
    src_start *= data_type_bytes[src->arr_data_type];
    count *= data_type_bytes[src->arr_data_type];
    dest_start *= data_type_bytes[src->arr_data_type];
    if (src && dest && src->arr_body && dest->arr_body && count > 0)
        memcpy(&(dest->arr_body[dest_start]), &(src->arr_body[src_start]), count);

#if _JVM_DEBUG
    printf("java_lang_System_arraycopy\n");
#endif

    return 0;
}


s32 java_lang_System_doubleToString(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;

    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    u8 buf[22];
    sprintf(buf, "%lf", l2d.d);
    Utf8String *str = utf8_create_c(buf);
    Instance *jstr = jstring_create(str, runtime);
    push_ref(stack, (__refer) jstr);
    utf8_destory(str);

#if _JVM_DEBUG
    printf("java_lang_System_doubleToString\n");
#endif

    return 0;
}

s32 java_lang_System_currentTimeMillis(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;

    s64 curMs = currentTimeMillis();
    push_long(stack, curMs);

#if _JVM_DEBUG
    printf("java_lang_System_currentTimeMillis\n");
#endif

    return 0;
}

s32 java_lang_System_nanotime(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;

    s64 nano = nanoTime();
    push_long(stack, nano);

#if _JVM_DEBUG
    printf("java_lang_System_nanotime\n");
#endif

    return 0;
}

s32 java_lang_System_identityHashCode(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *tmps = (Instance *) runtime->localVariables.item[0].refer;
    push_ref(stack, (__refer) tmps);

#if _JVM_DEBUG
    printf("java_lang_System_identityHashCode\n");
#endif

    return 0;
}

s32 java_lang_System_getProperty0(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *tmps = (Instance *) runtime->localVariables.item[0].refer;
    Utf8String *ustr = utf8_create_c("UTF-8");
    Instance *jstr = jstring_create(ustr, runtime);
    push_int(stack, (s32) 0);
    utf8_destory(ustr);
#if _JVM_DEBUG
    printf("java_lang_System_getProperty0 \n");
#endif

    return 0;
}

s32 java_lang_Thread_currentThread(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    push_ref(stack, (__refer) runtime->thread);
#if _JVM_DEBUG
    printf("java_lang_Thread_currentThread \n");
#endif

    return 0;
}

s32 java_lang_Thread_yield(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *tmps = (Instance *) runtime->localVariables.item[0].refer;

#if _JVM_DEBUG
    printf("java_lang_Thread_yield \n");
#endif

    return 0;
}

s32 java_lang_Thread_sleep(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Long2Double l2d;
    l2d.i2l.i1 = runtime->localVariables.item[0].integer;
    l2d.i2l.i0 = runtime->localVariables.item[1].integer;
    threadWait(l2d.l);
#if _JVM_DEBUG
    printf("java_lang_Thread_sleep \n");
#endif

    return 0;
}

s32 java_lang_Thread_start(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;
    thread_create_reg(ins, NULL);

#if _JVM_DEBUG
    printf("java_lang_Thread_start \n");
#endif

    return 0;
}

s32 java_lang_Thread_isAlive(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;

    push_int(stack, 1);//todo
#if _JVM_DEBUG
    printf("java_lang_Thread_isAlive \n");
#endif

    return 0;
}

s32 java_lang_Thread_activeCount(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;

    push_int(stack, thread_list->length);//todo
#if _JVM_DEBUG
    printf("java_lang_Thread_activeCount \n");
#endif

    return 0;
}


s32 java_lang_Thread_setPriority0(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;

#if _JVM_DEBUG
    printf("java_lang_Thread_setPriority0 \n");
#endif

    return 0;
}


s32 java_lang_Thread_interrupt0(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *ins = (Instance *) runtime->localVariables.item[0].refer;

#if _JVM_DEBUG
    printf("java_lang_Thread_interrupt0 \n");
#endif

    return 0;
}


s32 java_io_PrintStream_printImpl(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;

#if _JVM_DEBUG
    printf("java_io_PrintStream_printImpl \n");
#endif

    Instance *tmps = NULL;
    tmps = (Instance *) runtime->localVariables.item[0].refer;
    if (tmps) {

        c8 *fieldPtr = getFieldPtr_byName(tmps, "java/lang/String", "value", "[C");
        Instance *ptr = (Instance *) getFieldRefer(fieldPtr);
        //printf("printImpl [%x]\n", arr_body);
        if (ptr && ptr->arr_body) {
            u16 *jchar_arr = (u16 *) ptr->arr_body;
            s32 i = 0;
            for (; i < ptr->arr_length; i++) {
                u16 ch = jchar_arr[i];
                //swap_endian_little_big((u8*)&ch, sizeof(ch));
                printf("%c", (u8) ch);
            }
        }
    }
    return 0;
}

s32 java_io_Throwable_printStackTrace0(Runtime *runtime, Class *clazz, c8 *type) {
    StackFrame *stack = runtime->stack;
    Instance *tmps = (Instance *) runtime->localVariables.item[0].refer;
#if _JVM_DEBUG
    printf("java_io_Throwable_printStackTrace0 \n");
#endif
    printf("java_io_Throwable_printStackTrace0 %s \n", utf8_cstr(tmps->obj_of_clazz->name));
    return 0;
}

static java_native_method method_table[] = {
        {"com/sun/cldc/io/ConsoleOutputStream", "write",             com_sun_cldc_io_ConsoleOutputStream_write},
//        {"com/sun/cldc/io/ResourceInputStream", "open",              com_sun_cldc_io_ResourceInputStream_open},
//        {"com/sun/cldc/io/ResourceInputStream", "close",             com_sun_cldc_io_ResourceInputStream_close},
//        {"com/sun/cldc/io/ResourceInputStream", "size",              com_sun_cldc_io_ResourceInputStream_size},
//        {"com/sun/cldc/io/ResourceInputStream", "read",              com_sun_cldc_io_ResourceInputStream_read},
//        {"com/sun/cldc/io/ResourceInputStream", "readBytes",         com_sun_cldc_io_ResourceInputStream_readBytes},
//        {"com/sun/cldc/io/Waiter",              "waitForIO",         com_sun_cldc_io_ResourceInputStream_waitForIO},
        {"java/lang/Class",                     "forName",           java_lang_Class_forName},
        {"java/lang/Class",                     "newInstance",       java_lang_Class_newInstance},
        {"java/lang/Class",                     "isInstance",        java_lang_Class_isInstance},
        {"java/lang/Class",                     "isAssignableFrom",  java_lang_Class_isAssignableFrom},
        {"java/lang/Class",                     "isInterface",       java_lang_Class_isInterface},
        {"java/lang/Class",                     "isArray",           java_lang_Class_isArray},
        {"java/lang/Class",                     "getName",           java_lang_Class_getName},
        {"java/lang/Double",                    "doubleToLongBits",  java_lang_Double_doubleToLongBits},
        {"java/lang/Double",                    "longBitsToDouble",  java_lang_Double_longBitsToDouble},
        {"java/lang/Float",                     "floatToIntBits",    java_lang_Float_floatToIntBits},
        {"java/lang/Float",                     "intBitsToFloat",    java_lang_Float_intBitsToFloat},
        {"java/lang/Math",                      "exp",               java_lang_Math_exp},
        {"java/lang/Math",                      "random",            java_lang_Math_random},
        {"java/lang/Math",                      "sin",               java_lang_Math_sin},
        {"java/lang/Math",                      "cos",               java_lang_Math_cos},
        {"java/lang/Math",                      "tan",               java_lang_Math_tan},
        {"java/lang/Math",                      "sqrt",              java_lang_Math_sqrt},
        {"java/lang/Math",                      "ceil",              java_lang_Math_ceil},
        {"java/lang/Math",                      "floor",             java_lang_Math_floor},
        {"java/lang/Object",                    "getClass",          java_lang_Object_getClass},
        {"java/lang/Object",                    "hashCode",          java_lang_Object_hashCode},
        {"java/lang/Object",                    "notify",            java_lang_Object_notify},
        {"java/lang/Object",                    "notifyAll",         java_lang_Object_notifyAll},
        {"java/lang/Object",                    "wait",              java_lang_Object_wait},
        {"java/lang/Runtime",                   "exitInternal",      java_lang_Runtime_exitInternal},
        {"java/lang/Runtime",                   "freeMemory",        java_lang_Runtime_freeMemory},
        {"java/lang/Runtime",                   "totalMemory",       java_lang_Runtime_totalMemory},
        {"java/lang/Runtime",                   "gc",                java_lang_Runtime_gc},
        {"java/lang/String",                    "charAt",            java_lang_String_charAt},
        {"java/lang/String",                    "equals",            java_lang_String_equals},
        {"java/lang/String",                    "indexOf",           java_lang_String_indexOf},
        {"java/lang/String",                    "indexOfFrom",       java_lang_String_indexOfFrom},
        {"java/lang/String",                    "intern",            java_lang_String_intern},
//        {"java/lang/StringBuffer",              "append",            java_lang_StringBuffer_append},
//        {"java/lang/StringBuffer",              "appendi",            java_lang_StringBuffer_appendi},
//        {"java/lang/StringBuffer",              "toString",          java_lang_StringBuffer_toString},
        {"java/lang/System",                    "arraycopy",         java_lang_System_arraycopy},
        {"java/lang/System",                    "doubleToString",    java_lang_System_doubleToString},
        {"java/lang/System",                    "currentTimeMillis", java_lang_System_currentTimeMillis},
        {"java/lang/System",                    "nanoTime",          java_lang_System_nanotime},
        {"java/lang/System",                    "identityHashCode",  java_lang_System_identityHashCode},
        {"java/lang/System",                    "getProperty0",      java_lang_System_getProperty0},
        {"java/lang/Thread",                    "currentThread",     java_lang_Thread_currentThread},
        {"java/lang/Thread",                    "yield",             java_lang_Thread_yield},
        {"java/lang/Thread",                    "sleep",             java_lang_Thread_sleep},
        {"java/lang/Thread",                    "start",             java_lang_Thread_start},
        {"java/lang/Thread",                    "isAlive",           java_lang_Thread_isAlive},
        {"java/lang/Thread",                    "activeCount",       java_lang_Thread_activeCount},
        {"java/lang/Thread",                    "setPriority0",      java_lang_Thread_setPriority0},
        {"java/lang/Thread",                    "interrupt0",        java_lang_Thread_interrupt0},
        {"java/io/PrintStream",                 "printImpl",         java_io_PrintStream_printImpl},
        {"java/io/Throwable",                   "printStackTrace0",  java_io_Throwable_printStackTrace0},
};

static s32 java_lang_method_size = sizeof(method_table) / sizeof(java_native_method);

java_native_method *find_java_lang_method(c8 *cls_name, c8 *method_name) {
    s32 i;
    for (i = 0; i < java_lang_method_size; i++)
        if (strcmp(cls_name, method_table[i].clzname) == 0 &&
            strcmp(method_name, method_table[i].methodname) == 0)
            return &method_table[i];
    return 0;
}

s32 invoke_native_method(Runtime *runtime, Class *p,
                         c8 *cls_name, c8 *method_name, c8 *type) {
    StackFrame *stack = runtime->stack;
    java_native_method *method = find_java_lang_method(cls_name, method_name);
    if (method != 0) {
#if _JVM_DEBUG
        printf("invoke native %s/%s %s\n", method->clzname, method->methodname, type);
#endif
        method->method_runtime(runtime, p, type);
        return 1;
    }
    return 0;
}
