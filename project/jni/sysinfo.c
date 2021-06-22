#include <stdlib.h>
#include <jni.h>

extern char *get_process_info();

JNIEXPORT jstring JNICALL Java_org_example_ndk_NDKExam_getProcessInfo(JNIEnv *env, jobject this) {
  char *buf = get_process_info();
  jstring jstr = (*env)->NewStringUTF(env, buf);
  free(buf);
  return jstr;
}
