# AVL-Tree-Implementation-in-Linux-CFS-Schedular

Description: Memory management of  Linux  kernel is based upon Red-Black trees through CFS  Scheduler, so we tried to modify the Red-Black tree with AVL tree as searching time for AVL tree is less compared to Red-Black but we need to compromise on the cost of insertion and deletion in AVL.

The actual working of the project: The AVL tree implementation has been completed successfully by  using filing as we append the specific file name’s in a given folder through which we then pass this value  and so the an AVL tree is being maintained. This implementation can also be done for the root directory and then a child node can be created for each folder and then the other child node. This implementation of AVL allows us to locate or search the file more efficiently then compared to Red-Black tree.

Explanation of source code:

*  List of files modified: linux/kernel/sched_fair.c
*  Modification: Red-Black tree was implemented in this file so we changed it to AVL tree.
*  Significance of Modification: Searching is faster as compare it to in Red-Black tree.
*  How to manifest your  correctness of your modification:  As it is possible to implement AVL instead of Red-Black so it clearly shows that this implementation is practically possible, but as  the searching time is reduced, the insertion and deletion is more costly now as compare  to in Red-Black.

System Requirements
 
Make sure that your system meets the minimum requirements to run this module:

·         Linux Os
·         Kernel (For All current Kernels)
·         300+MHz CPU
·         128MB RAM
·	        4GB hard drive space

Step 1: You have to be on your kernel root
Step 2: Now Make a directory for this module
Step 3: Copy .c file in that directory and Makefile as well
(Note: Make sure that you have your file name in place of mod1.o in line 18 of Makefile)
Step 4: Now use command "make" to compile the code
Step 5: Once it compiles, then use command "insmod yourfilename.ko" to insert the module in kernel
Step 6: Now use command "lsmod" to see that your module is inserted
Step 7: Now use command "tail -f /var/log/messages" to see the output of your module


References:
*  http://lxr.free-electrons.com/source/kernel/sched_fair.c?v=3.2
(It contains sched_fair.c i.e the scheduler file of kernel)
*  http://stackoverflow.com/questions/11193648/execute-shell-command-in-kernel-module
(It contains the code to run shell scripting command in kernel programming)  

CopyRights FASTIANS ©
All rights reserved.
