//加锁操作

//https://blog.csdn.net/u014582394/article/details/79418418
//使用条件变量最大的好处是可以避免忙等。相当与多线程中的信号。
//条件变量使我们可以睡眠等待某种条件出现。
//条件变量是利用线程间共享的全局变量进行同步的一种机制，主要包括两个动作：///一个线程等待”条件变量的条件成立”而挂起；另一个线程使”条件成立”（给出条件///成立信号）。为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。
//　　条件变量类型为pthread_cond_t、

条件变量和互斥锁一样，都有静态动态两种创建方式，静态方式使用PTHREAD_COND_INITIALIZER常量，如下：
　　pthread_cond_t cond=PTHREAD_COND_INITIALIZER
　　动态方式调用pthread_cond_init()函数，API定义如下：
　　int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr)
　　尽管POSIX标准中为条件变量定义了属性，但在LinuxThreads中没有实现，因此cond_attr值通常为NULL，且被忽略。
　　注销一个条件变量需要调用pthread_cond_destroy()，只有在没有线程在该条件变量上等待的时候才能注销这个条件变量，否则返回EBUSY。API定义如下：
　　int pthread_cond_destroy(pthread_cond_t *cond)

　　等待和激发
　　int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
　　int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
　　等待条件有两种方式：无条件等待pthread_cond_wait()和计时等待pthread_cond_timedwait()，其中计时等待方式如果在给定时刻前条件没有满足，则返回ETIMEOUT，结束等待，其中abstime以与time()系统调用相同意义的绝对时间形式出现，0表示格林尼治时间1970年1月1日0时0分0秒。
　　使用绝对时间而非相对时间的优点是。如果函数提前返回（很可能因为捕获了一个信号，）
　　无论哪种等待方式，都必须和一个互斥锁配合，以防止多个线程同时请求pthread_cond_wait()（或pthread_cond_timedwait()，下同）的竞争条件（Race Condition）。mutex互斥锁必须是普通锁（PTHREAD_MUTEX_TIMED_NP）或者适应锁（PTHREAD_MUTEX_ADAPTIVE_NP），且在调用pthread_cond_wait()前必须由本线程加锁（pthread_mutex_lock()），而在更新条件等待队列以前，mutex保持锁定状态，并在线程挂起进入等待前解锁。在条件满足从而离开pthread_cond_wait()之前，mutex将被重新加锁，以与进入pthread_cond_wait()前的加锁动作对应。
　　激发条件有两种形式，pthread_cond_signal()激活一个等待该条件的线程，存在多个等待线程时按入队顺序激活其中一个；而pthread_cond_broadcast()则激活所有等待线程。

　　其他
　　pthread_cond_wait()和pthread_cond_timedwait()都被实现为取消点，因此，在该处等待的线程将立即重新运行，在重新锁定mutex后离开pthread_cond_wait()，然后执行取消动作。也就是说如果pthread_cond_wait()被取消，mutex是保持锁定状态的，因而需要定义退出回调函数来为其解锁。

https://cloud.tencent.com/developer/article/1021109

#include <pthread.h>

    class CTestLock
    {
    public:
        CTestLock()        { pthread_mutex_init(&mutex_t_, NULL);  pthread_cond_init(&cond_t_, NULL);      }
        ~CTestLock()       { pthread_mutex_destroy(&mutex_t_);     pthread_cond_destroy(&cond_t_);          }
        int Lock()          { int nRetCode = pthread_mutex_lock(&mutex_t_);         return (nRetCode == 0); }
        int Unlock()      { int nRetCode = pthread_mutex_unlock(&mutex_t_);       return (nRetCode == 0); }
        int Wait()           { int nRetCode = pthread_cond_wait(&cond_t_,&mutex_t_); return (nRetCode == 0); }
        int Signal()       { int nRetCode = pthread_cond_signal(&cond_t_);          return (nRetCode == 0); }    
        int Broadcast()   { int nRetCode = pthread_cond_broadcast(&cond_t_);      return (nRetCode == 0); }
        
    private:
        pthread_mutex_t mutex_t_;
        pthread_cond_t cond_t_;
    };


// 本例程
1. 四个线程要给cup-sn赋值， 这个用一个互斥锁 就可以 不用条件变量
（有死锁的可能吗？）pthread_mutex_t mutex_sn;
2. 四个线程中条件（都完成检测）触发保存线程 需要一个互斥锁和条件变量\
    pthread_mutex_t mutex_??;


    //先只锁cpu-sn 其他用原子操作



另：在Android NDK中是没有libpthread.a的，有关的功能实现已经被集成到了libc.a中，所以使用时不需要使用-lpthread，而应该使用-lc链接参数。
// xx不让用锁 可以用
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= factory_test_server.c data.c factory_test_function.c serial_test.c can_test.c
LOCAL_MODULE:= factory_test_server
LOCAL_32_BIT_ONLY := true
LOCAL_LDLIBS += -lc
include $(BUILD_EXECUTABLE)

#include $(BUILD_SHARD_LIBRARY)