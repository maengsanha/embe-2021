#include <stdlib.h>
#include <jni.h>

#include "android/log.h"

#define LOG_TAG "MyTag"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

extern char *get_process_info();

JNIEXPORT jstring JNICALL Java_org_example_ndk_NDKExam_getProcessInfo(JNIEnv *env, jobject this) {
  char *buf = get_process_info();
  LOGV("info: %s\n", buf);
  jstring jstr = (*env)->NewStringUTF(env, buf);
  free(buf);
  return jstr;
}
