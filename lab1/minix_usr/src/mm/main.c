/* This file contains the main program of the memory manager and some related
 * procedures.  When MINIX starts up, the kernel runs for a little while,
 * initializing itself and its tasks, and then it runs MM and FS.  Both MM
 * and FS initialize themselves as far as they can.  FS then makes a call to
 * MM, because MM has to wait for FS to acquire a RAM disk.  MM asks the
 * kernel for all free memory and starts serving requests.
 *
 * The entry points into this file are:
 *   main:	starts MM running
 *   setreply:	set the reply to be sent to process making an MM system call
 */

#include "mm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "mproc.h"
#include "param.h"

FORWARD _PROTOTYPE( void get_work, (void)				);
FORWARD _PROTOTYPE( void mm_init, (void)				);
/* JAN_BEGIN */
FORWARD _PROTOTYPE( int fndlngstpth, ( int proc_nr, int depth )		);
/* JAN_END */

#define click_to_round_k(n) \
	((unsigned) ((((unsigned long) (n) << CLICK_SHIFT) + 512) / 1024))

/*===========================================================================*
 *				main					     *
 *===========================================================================*/
PUBLIC void main()
{
/* Main routine of the memory manager. */

  int result, proc_nr;
  struct mproc *rmp;

  mm_init();			/* initialize memory manager tables */

  /* This is MM's main loop-  get work and do it, forever and forever. */
  while (TRUE) {
	get_work();		/* wait for an MM system call */

	/* If the call number is valid, perform the call. */
	if ((unsigned) mm_call >= NCALLS) {
		result = ENOSYS;
	} else {
		result = (*call_vec[mm_call])();
	}

	/* Send the results back to the user to indicate completion. */
	if (result != E_NO_MESSAGE) setreply(who, result);

	swap_in();		/* maybe a process can be swapped in? */

	/* Send out all pending reply messages, including the answer to
	 * the call just made above.  The processes must not be swapped out.
	 */
	for (proc_nr = 0, rmp = mproc; proc_nr < NR_PROCS; proc_nr++, rmp++) {
		if ((rmp->mp_flags & (REPLY | ONSWAP)) == REPLY) {
			if (send(proc_nr, &rmp->mp_reply) != OK)
				panic("MM can't reply to", proc_nr);
			rmp->mp_flags &= ~REPLY;
		}
	}
  }
}


/*===========================================================================*
 *				get_work				     *
 *===========================================================================*/
PRIVATE void get_work()
{
/* Wait for the next message and extract useful information from it. */

  if (receive(ANY, &mm_in) != OK) panic("MM receive error", NO_NUM);
  who = mm_in.m_source;		/* who sent the message */
  mm_call = mm_in.m_type;	/* system call number */

  /* Process slot of caller.  Misuse MM's own process slot for tasks (KSIG?). */
  mp = &mproc[who < 0 ? MM_PROC_NR : who];
}


/*===========================================================================*
 *				setreply				     *
 *===========================================================================*/
PUBLIC void setreply(proc_nr, result)
int proc_nr;			/* process to reply to */
int result;			/* result of the call (usually OK or error #)*/
{
/* Fill in a reply message to be sent later to a user process.  System calls
 * may occasionally fill in other fields, this is only for the main return
 * value, and for setting the "must send reply" flag.
 */

  register struct mproc *rmp = &mproc[proc_nr];

  rmp->reply_res = result;
  rmp->mp_flags |= REPLY;	/* reply pending */

  if (rmp->mp_flags & ONSWAP)
	swap_inqueue(rmp);	/* must swap this process back in */
}


/*===========================================================================*
 *				mm_init					     *
 *===========================================================================*/
PRIVATE void mm_init()
{
/* Initialize the memory manager. */

  static char core_sigs[] = { SIGQUIT, SIGILL, SIGTRAP, SIGABRT,
			SIGEMT, SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2 };
  static char ign_sigs[] = { SIGCHLD };
  register int proc_nr;
  register struct mproc *rmp;
  register char *sig_ptr;
  phys_clicks ram_clicks, total_clicks, minix_clicks, free_clicks;
  message mess;
  struct mem_map kernel_map[NR_SEGS];
  int mem;

  /* Build the set of signals which cause core dumps, and the set of signals
   * that are by default ignored.
   */
  sigemptyset(&core_sset);
  for (sig_ptr = core_sigs; sig_ptr < core_sigs+sizeof(core_sigs); sig_ptr++)
	sigaddset(&core_sset, *sig_ptr);
  sigemptyset(&ign_sset);
  for (sig_ptr = ign_sigs; sig_ptr < ign_sigs+sizeof(ign_sigs); sig_ptr++)
	sigaddset(&ign_sset, *sig_ptr);

  /* Get the memory map of the kernel to see how much memory it uses. */
  sys_getmap(SYSTASK, kernel_map);
  minix_clicks = (kernel_map[S].mem_phys + kernel_map[S].mem_len)
				- kernel_map[T].mem_phys;

  /* Initialize MM's tables. */
  for (proc_nr = 0; proc_nr <= INIT_PROC_NR; proc_nr++) {
	rmp = &mproc[proc_nr];
	rmp->mp_flags |= IN_USE;
	sys_getmap(proc_nr, rmp->mp_seg);
	if (rmp->mp_seg[T].mem_len != 0) rmp->mp_flags |= SEPARATE;
	minix_clicks += (rmp->mp_seg[S].mem_phys + rmp->mp_seg[S].mem_len)
				- rmp->mp_seg[T].mem_phys;
  }
  mproc[INIT_PROC_NR].mp_pid = INIT_PID;
  sigemptyset(&mproc[INIT_PROC_NR].mp_ignore);
  sigemptyset(&mproc[INIT_PROC_NR].mp_catch);
  procs_in_use = LOW_USER + 1;

  /* Wait for FS to send a message telling the RAM disk size then go "on-line".
   */
  if (receive(FS_PROC_NR, &mess) != OK)
	panic("MM can't obtain RAM disk size from FS", NO_NUM);

  ram_clicks = mess.m1_i1;

  /* Initialize tables to all physical mem. */
  mem_init(&total_clicks, &free_clicks);

  /* Print memory information. */
  printf("\nMemory size = %uK   ", click_to_round_k(total_clicks));
  printf("MINIX = %uK   ", click_to_round_k(minix_clicks));
  printf("RAM disk = %uK   ", click_to_round_k(ram_clicks));
  printf("Available = %uK\n\n", click_to_round_k(free_clicks));

  /* Tell FS to continue. */
  if (send(FS_PROC_NR, &mess) != OK)
	panic("MM can't sync up with FS", NO_NUM);

  /* Tell the memory task where my process table is for the sake of ps(1). */
  if ((mem = open("/dev/ram", O_RDWR)) != -1) {
	ioctl(mem, MIOCSPSINFO, (void *) mproc);
	close(mem);
  }
}
/* JAN_BEGIN */
/*===========================================================================*
 *				do_getpidmch				     *
 *===========================================================================*/
PUBLIC int do_getpidmch()
{
  register struct mproc *rmp; 
  int max_ch, proc_nr, c_proc_nr, children, found;

  rmp = mp; 
  found = -1;
  max_ch = 0;

  for (proc_nr = 2; proc_nr < NR_PROCS; ++proc_nr) {
  	if (mproc[proc_nr].mp_flags & IN_USE) {
  		children = 0;
  		for (c_proc_nr = 0; c_proc_nr < NR_PROCS; ++c_proc_nr) {
  			if (
  				(mproc[c_proc_nr].mp_flags & IN_USE) &&
  				(mproc[c_proc_nr].mp_parent == proc_nr)
  			) ++children;
  		}
  		if (children > max_ch) {
  			max_ch = children;
  			found = mproc[proc_nr].mp_pid;
  		}
  	}
  }

  rmp->mp_reply.m1_i1 = found;
  rmp->mp_reply.m1_i2 = max_ch;
  return (found != -1) ? OK : EINVAL;
}
/*===========================================================================*
 *				do_fndlngstpth				     *
 *===========================================================================*/
PRIVATE int fndlngstpth(proc_nr, depth)
int proc_nr;
int depth;
{
  int hasChildren, c_proc_nr, maxDepth, childDepth;

  if (mproc[proc_nr].mp_flags & IN_USE) {
  	hasChildren = 0;
  	for (c_proc_nr = 2; c_proc_nr < NR_PROCS; ++c_proc_nr) {
  		if (
  			(mproc[c_proc_nr].mp_flags & IN_USE) &&
			c_proc_nr != proc_nr &&
			mproc[c_proc_nr].mp_pid > 1 &&
			mproc[c_proc_nr].mp_parent == proc_nr
		) {
  			hasChildren = 1;
  			break;
  		}
  	}

  	if (!hasChildren) return depth;
  	        maxDepth = depth;

	for (c_proc_nr = 2; c_proc_nr < NR_PROCS; ++c_proc_nr) {
		if (
			(mproc[c_proc_nr].mp_flags & IN_USE) &&
			c_proc_nr != proc_nr &&
			mproc[c_proc_nr].mp_pid > 1 &&
			mproc[c_proc_nr].mp_parent == proc_nr
		) {
			childDepth = fndlngstpth(
				c_proc_nr, depth+1
			);
			if (childDepth > maxDepth) maxDepth = childDepth;
		}
	}
	return maxDepth;
  }
  return depth;
}
/*===========================================================================*
 *				do_getpidmln				     *
 *===========================================================================*/
PUBLIC int do_getpidmln()
{
  register struct mproc *rmp; 
  int maxDepth, pathDepth, proc_nr, exclude_pid, foundPid;

  rmp = mp;
  maxDepth = -1;
  foundPid = -1;
  exclude_pid = mm_in.m1_i1;

  for (proc_nr = 2; proc_nr < NR_PROCS; ++proc_nr) {
  	if (
  		(mproc[proc_nr].mp_flags & IN_USE) &&
  		mproc[proc_nr].mp_pid > 1 &&
  		mproc[proc_nr].mp_pid != exclude_pid
  	) {
  		pathDepth = fndlngstpth(proc_nr, 0);
  		if (pathDepth > maxDepth) {
  			maxDepth = pathDepth;
  			foundPid = mproc[proc_nr].mp_pid;
  		}
	}

  }

  rmp->mp_reply.m1_i1 = foundPid;
  rmp->mp_reply.m1_i2 = maxDepth;
  return (foundPid != -1) ? OK : EINVAL;
}
/* JAN_END */
