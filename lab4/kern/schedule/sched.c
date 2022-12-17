#include <list.h>
#include <sync.h>
#include <proc.h>
#include <sched.h>
#include <assert.h>

void
wakeup_proc(struct proc_struct *proc) {
    assert(proc->state != PROC_ZOMBIE && proc->state != PROC_RUNNABLE);
    proc->state = PROC_RUNNABLE;
}

void
schedule(void) {
    bool intr_flag;//定义中断变量
    list_entry_t *le, *last;
    struct proc_struct *next = NULL;//下一进程
    local_intr_save(intr_flag);//关闭中断
    {
        current->need_resched = 0;
        //last是否是idle进程(第一个创建的进程),如果是，则从表头开始搜索  否则获取下一链表
        last = (current == idleproc) ? &proc_list : &(current->list_link);
        le = last;
        //循环找到可调度的进程
        do {
            if ((le = list_next(le)) != &proc_list) {
                //获取下一进程
                next = le2proc(le, list_link);
                //找到一个可以调度的进程，break
                if (next->state == PROC_RUNNABLE) {
                    break;
                }
            }
        } while (le != last);
        //如果没有找到可调度的进程
        if (next == NULL || next->state != PROC_RUNNABLE) {
            next = idleproc;
        }
        //运行次数加一
        next->runs ++;
        //运行新进程,
        if (next != current) {
            proc_run(next);
        }
    }
    //恢复中断
    local_intr_restore(intr_flag);
}

