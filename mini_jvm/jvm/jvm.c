
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jvm.h"
#include "../utils/utf8_string.h"
#include "garbage.h"
#include "jvm_util.h"


void destoryAllClasses(hmap_t classes) {
    hashtable_destory(classes);
}

s32 execute(c8 *p_classpath, c8 *p_mainclass) {
    //为指令创建索引
    instructionsIndexies = createInstructIndexies();
    //创建类容器
    classes = hashtable_create(_UNI_STR_HashtableHash, _UNI_STR_HashtableEquals);
    //创建垃圾收集器
    garbage_collector_create();
    //创建线程容器
    thread_list = arraylist_create(0);

    //创建运行时栈
    Runtime runtime;
    runtime_create(&runtime);

    //开始装载类
    Utf8String *classpath = utf8_create_c(p_classpath);
    Utf8String *mainclass = utf8_create_c(p_mainclass);
    load_class(classpath, mainclass, classes);
    Utf8String *jstring_class = utf8_create_c("java/lang/String");
    load_class(classpath, jstring_class, classes);

    HashtableIterator hti;
    hashtable_iterate(classes, &hti);
#if _JVM_DEBUG
    printf("classes size:%d\n", hashtable_num_entries(classes));
#endif
    for (; hashtable_iter_has_more(&hti);) {
        Utf8String *k = hashtable_iter_next_key(&hti);
        Class *clazz = hashtable_get(classes, k);
#if _JVM_DEBUG
        printf("classes entry : %s,%d\n", utf8_cstr(k), clazz);
#endif
        if (clazz->status != CLASS_STATUS_PREPARED)class_link(clazz);
    }
    hashtable_iterate(classes, &hti);
    for (; hashtable_iter_has_more(&hti);) {
        Utf8String *k = hashtable_iter_next_key(&hti);
        Class *clazz = hashtable_get(classes, k);
        if (clazz->status != CLASS_STATUS_CLINITED)class_clinit(clazz, &runtime);//初始化
    }

    Class *clazz = hashtable_get(classes, mainclass);

    s32 ret = 0;
    if (clazz) {
        //printf("class: %s : %d\n", utf8_cstr(mname), obj_of_clazz);

#if _JVM_DEBUG
        printConstantPool(clazz);
        printMethodPool(clazz, &(clazz->methodPool));
        printFieldPool(clazz, &(clazz->fieldPool));
        printClassFileFormat(&(clazz->cff));
#endif

        Utf8String *methodName = utf8_create_c("main");
        Utf8String *methodType = utf8_create_c("([Ljava/lang/String;)V");
        Instance arr;
        memset(&arr, 0, sizeof(Instance));
        push_ref(runtime.stack, (__refer) &arr);//main(String[])参数

        MethodInfo *main = find_methodInfo_by_name(mainclass, methodName, methodType);
        if (main) {
            Class *thread_clazz = getClass("java/lang/Thread", &runtime);
            //为主线程创建Thread实例
            Instance *main_thread = instance_create(thread_clazz);
            pthread_t pthread = pthread_self();
            thread_create_reg(main_thread, &pthread);
            runtime.thread = main_thread;
            instance_init(main_thread, &runtime);//必须放在最好，初始化时需要用到前面的赋值
            printf("\n\n\n\n\n\n================================= main start ================================\n");
            //调用主方法
            ret = execute_method(main, &runtime, clazz, METHOD_INVOKE_STATIC);
            printf("================================= main  end  ================================\n");
            garbage_collect();
        }
        utf8_destory(methodName);
    }

//    if (clazz) {
//#if _JVM_DEBUG
//        printConstantPool(&(obj_of_clazz->constantPool));
//        printMethodPool(&(obj_of_clazz->constantPool), &(obj_of_clazz->methodPool));
//        printClassFileFormat(&(obj_of_clazz->cff));
//#endif
//    }
    stack_destory(runtime.stack);
    stack_destory(runtime.method_para_stack);

    utf8_destory(classpath);
    utf8_destory(jstring_class);
    utf8_destory(mainclass);
    destoryAllClasses(classes);
    printf("over\n");
    //getchar();
    return ret;
}
