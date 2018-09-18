#include <jni.h>
#include <unistd.h>
#include "log.h"
#include <stdlib.h>
#include "daemon.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_erlei_keepalive_NativeDaemon_doDaemon(JNIEnv *env, jobject instance, jobjectArray args,
                                               jstring serviceName_) {
    const char *serviceName = (*env)->GetStringUTFChars(env, serviceName_, 0);
    int len = (*env)->GetArrayLength(env, args);
    const char **files = (const char **) malloc(len * sizeof(char *));
    jstring jstr;
    for (int i = 0; i < len; ++i) {
        jstr = (*env)->GetObjectArrayElement(env, args, i);
        files[i] = (char *) (*env)->GetStringUTFChars(env, jstr, 0);
        LOGI("file name %s", files[i]);
    }
    (*env)->ReleaseStringUTFChars(env, serviceName_, serviceName);
    if (fork() == 0) {
        setsid();
        do_daemon(len, files);
        exit(0);
    }
    return;
}

#ifdef __cplusplus
}
#endif