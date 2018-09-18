//
// Created by lll on 2018/3/6.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
#include <pthread.h>
#include "log.h"
#include <sys/prctl.h>

const char *locked_file;

//检查文件是否上锁  , 0 没锁 -1 锁了
int checkFileLock(char const *pfile) {
    if (pfile == NULL)return -1;
    int lockfd = open(pfile, O_RDWR);
    int locked = -1;
    if (lockfd != -1) {
        if (flock(lockfd, LOCK_EX | LOCK_NB) == 0) {
            locked = 0;
            close(lockfd);
        }

    } else {
        LOGI("open file %s. fd:%d.\n", pfile, lockfd);
    }
    return locked;
}

//锁定一个文件
int lockFile(char const *pfile) {
    int lockfd = open(pfile, O_RDWR);
    if (lockfd != -1) {
        if (flock(lockfd, LOCK_EX | LOCK_NB) == 0) {
            locked_file = pfile;
        }
    }
    return 0;
}

void *run(void *arg) {
    char const *file = (char const *) arg;
    LOGI("pid = %u tid %u (0x%x) file name %s \n", (unsigned int) getpid(),
         (unsigned int) pthread_self(), (unsigned int) pthread_self(), file);
    sleep(1);
    // while(checkFileLock(file) == 0){

    // }
    int lockfd = open(file, O_RDWR);
    if (lockfd != -1) {
        LOGI("pid = %d  %s 阻塞读取文件\n", getpid(), file);

        if (flock(lockfd, LOCK_EX) == 0) {
            close(lockfd);
            LOGE("pid = %d  %s 进程死亡\n", getpid(), file);
            if ((fork()) == 0) {
                setsid();
                char *args[] = {
                        "/mnt/sdcard/Android/data/com.erlei.keepalive/files/Program/lock0",
                        "/mnt/sdcard/Android/data/com.erlei.keepalive/files/Program/lock2",
                        "/mnt/sdcard/Android/data/com.erlei.keepalive/files/Program/lock3", NULL};
                execv("/data/user/0/com.erlei.keepalive/app_daemon/arm64-v8a/daemon", args);
//            } else {
//                sleep(3);
//                LOGI("pid = %d  %s 需要重启进程并建立链接\n", getpid(), file);
////                goto watch;
            } else {
                exit(0);
            }

        }
    }
    return NULL;
}

//监视其他文件什么时候解锁
int watchUnlocking(int argc, int index, char const *argv[]) {
    // LOGI("文件已经上锁,检测什么时候释放了锁\n");

    pthread_t tids[argc];
    for (int i = 0; i < argc; ++i) {
        if (i != index) {
            pthread_create(&tids[i], NULL, run, (void *) argv[i]);
        }
    }
    pthread_exit(NULL);
}


int do_daemon(int argc, const char **argv) {
    LOGI("main pid = %d file name %d\n", getpid(), argc);
    pid_t pid = -2;
    int i;
    for (i = 0; i < argc; i++) {
        if (checkFileLock(argv[i]) == -1)continue;
        if ((pid = fork()) == 0 || pid == -1)break;
    }
    if (pid == -1) {
//        fLOGI(stderr, "can't fork, error %d\n", errno);
        LOGE("pid = %d fork process failed \n", getpid());
        exit(1);
    } else if (pid == 0) {
        LOGE("pid = %d fork process success %s \n", getpid(), argv[i]);
        setsid();
        prctl(PR_SET_NAME, "keepalive", NULL, NULL, NULL);
        //子进程
        lockFile(argv[i]);
        LOGE("pid = %d locked file %s \n", getpid(), argv[i]);
        watchUnlocking(argc, i, argv);
        exit(0);
    }
    exit(0);
}

int main(int argc, const char **argv) {
    const char **files = (const char **) malloc(argc - 1 * sizeof(char *));
    for (int i = 1; i < argc; i++) {
        files[i - 1] = argv[i];
    }
    return do_daemon(argc - 1, files);
}

