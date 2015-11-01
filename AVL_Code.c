#include <linux/module.h>	// for all modules 
#include <linux/init.h>		// for entry/exit macros 
#include <linux/kernel.h>   	// for printk priority macros 
#include <asm/current.h>	// process information, just for fun 
#include <linux/sched.h>	// for "struct task_struct"
#include <linux/fs.h>		// Needed by filp
#include <linux/slab.h>		// for kmalloc();
#include <linux/gfp.h>
#include <asm/uaccess.h>	// Needed by segment descriptors
#include <linux/kmod.h>		// For Executing Shell Command

struct AVLTree_Node 
{
        char data[32]; 
        int bfactor;
        struct AVLTree_Node *link[2];
};

struct AVLTree_Node *root = NULL;

struct AVLTree_Node *createNode(char value[])
{
	int l, i;
        char ch;
	struct AVLTree_Node *newnode;
        newnode = (struct AVLTree_Node *)kmalloc(sizeof(struct AVLTree_Node), GFP_ATOMIC);
	l = strlen(value);
        for ( i = 0; i < l+1; ++i){
            ch = value[i];
            newnode->data[i]  = ch;
        }
        newnode->bfactor = 0;
        newnode->link[0] = newnode->link[1] = NULL;
        return newnode;
}

static void insertion (char value[]) {
        struct AVLTree_Node *bf, *parent_bf, *subtree, *temp, *curr, *parent, *newnode, *ptr;
    //    struct AVLTree_Node 
        int res = 0,i=0 ,num=100, compareLimit = 100, result;
        char link_dir[32];

        if (!root) {
                root = createNode(value);
                return;
        }

        bf = parent_bf;
	parent_bf = root;
        // find the location for inserting the new node
        for (curr = root; curr != NULL; ptr = curr) {
                num = strcmp(value,curr->data);
                if (num == 0) {
                        printk(KERN_INFO "Cannot insert duplicates!!\n");
                        return;
                }
		result = strncmp(value,curr->data, compareLimit);
                if(result > 0) 
                        res = 1;  
                else if(result <= 0) 
                        res =0;
                parent = curr;

                if (curr->bfactor != 0) {
                        bf = curr;
                        parent_bf = ptr;
                        i = 0;
                }
                link_dir[i++] = res;
		curr = curr->link[res];        
	}
        // create the new node 
        newnode = createNode(value);
        parent->link[res] = newnode;
        res = link_dir[i = 0];
        // updating the height balance after insertion 
        for (curr = bf; curr != newnode; res = link_dir[++i]) {
                if (res == 0)
                        curr->bfactor--;
                else
                        curr->bfactor++;
                curr = curr->link[res];
        }

        // right sub-tree 
        if (bf->bfactor == 2) {
                printk(KERN_INFO "bfactor = 2\n");
                temp = bf->link[1];
                if (temp->bfactor == 1) {                   
                        subtree = temp;
                        bf->link[1] = temp->link[0];
                        temp->link[0] = bf;
                        temp->bfactor = bf->bfactor = 0;
                } else {
                        subtree = temp->link[0];
                        temp->link[0] = subtree->link[1];
                        subtree->link[1] = temp;
                        bf->link[1] = subtree->link[0];
                        subtree->link[0] = bf;
                        // update balance factors 
                        if (subtree->bfactor == -1) {
                                bf->bfactor = 0;
                                temp->bfactor = 1;
                        } else if (subtree->bfactor == 0) {
                                bf->bfactor = 0;
                                temp->bfactor = 0;
                        } else if (subtree->bfactor == 1) {
                                bf->bfactor = -1;
                                temp->bfactor = 0;
                        }
                        subtree->bfactor = 0;
                }
        // left sub-tree 
        } else if (bf->bfactor == -2) {
                temp = bf->link[0];
                if (temp->bfactor == -1) {
                         // single rotation(SR) right
                        subtree = temp;
                        bf->link[0] = temp->link[1];
                        temp->link[1] = bf;
                        temp->bfactor = bf->bfactor = 0;
                } else {
                        // double rotation - (SR left + SR right)
                        subtree = temp->link[1];
                        temp->link[1] = subtree->link[0];
                        subtree->link[0] = temp;
                        bf->link[0] = subtree->link[1];
                        subtree->link[1] = bf;
                        // update balance factors 
                        if (subtree->bfactor == -1) {
                                bf->bfactor = 1;
                                temp->bfactor = 0;
                        } else if (subtree->bfactor == 0) {
                                bf->bfactor = 0;
                                temp->bfactor = 0;
                        } else if (subtree->bfactor == 1) {
                                bf->bfactor = 0;
                                temp->bfactor = -1;
                        }
                        subtree->bfactor = 0;
                }
        } else {
                return;
        }

        if (bf == root) {
                root = subtree;
                return;
        }
        if (bf != parent_bf->link[0]) {
                parent_bf->link[1] = subtree;
        } else {
                parent_bf->link[0] = subtree;
        }
        return;
}


static void deletion(char value[]) 
{
        char link_dir[32]; 
        int res = 0, i = 0, j = 0, index = 0, num=100, comp=100, compareLimit = 100;
	int result;
	struct AVLTree_Node *ptr[32];
 	struct AVLTree_Node *curr, *temp, *x, *y, *z;

        curr = root;
        if (!root) {
                printk(KERN_INFO "Tree not present\n");
                return;
        }
        
        num = strcmp(root->data,value);
        if ((num == 0) && (root->link[0] == NULL) && (root->link[1] == NULL)) {
                kfree(root);
                root = NULL;
                return;
        }
        // search the node to delete 
        while (curr != NULL) {
                comp = strcmp(curr->data,value);
                if (comp == 0)
                        break;
		result = strncmp(value,curr->data, compareLimit);
                if(result > 0) 
                        res = 1;  
                else if(result <= 0) 
                        res =0;

                link_dir[i] = res;
                ptr[i++] = curr;
                curr = curr->link[res];
        }

        if (!curr) {
                printk(KERN_INFO "Given data is not present!!\n");
                return;
        }
        index = link_dir[i - 1];
        temp = curr->link[1];
        // delete the node from the AVL tree - similar to BST deletion 
        if (curr->link[1] == NULL) {
                if (i == 0) {
                        temp = curr->link[0];
                        kfree(curr);
                        root = temp;
                        return;
                } else {
                        ptr[i - 1]->link[index] = curr->link[0];
                }
        } else if (temp->link[0] == NULL) {
                temp->link[0] = curr->link[0];
                temp->bfactor = curr->bfactor;
                if (i > 0) {
                        ptr[i-1]->link[index] = temp;
                } else {
                        root = temp;
                }
                link_dir[i] = 1;
                ptr[i++] = temp;
        } else {
                // delete node with two children 
                j = i++;
                while (1) {
                        link_dir[i] = 0;
                        ptr[i++] = temp;
                        x = temp->link[0];
                        if (x->link[0] == NULL)
                                break;
                        temp = x;
                }
                x->link[0] = curr->link[0];
                temp->link[0] = x->link[1];
                x->link[1] = curr->link[1];
                x->bfactor = curr->bfactor;
                if (j > 0) {
                        ptr[j - 1]->link[index] = x;
                } else {
                        root = x;
                }
                link_dir[j] = 1;
                ptr[j] = x;
        }
        kfree(curr);
        for (i=(i-1); i >= 0;i--) {
                x = ptr[i];
                if (link_dir[i] == 0) {
                        x->bfactor++;
                        if (x->bfactor == 1) {
                                break;
                        } else if (x->bfactor == 2) {
                                y = x->link[1];
                                if (y->bfactor == -1) {
                                // double rotation - (SR right + SR left) 
                                        z = y->link[0];
                                        y->link[0] = z->link[1];
                                        z->link[1] = y;
                                        x->link[1] = z->link[0];
                                        z->link[0] = x;
                                        // update balance factors 
                                        if (z->bfactor == -1) {
                                                x->bfactor = 0;
                                                y->bfactor = 1;
                                        } else if (z->bfactor == 0) {
                                                x->bfactor = 0;
                                                y->bfactor = 0;
                                        } else if (z->bfactor == 1) {
                                                x->bfactor = -1;
                                                y->bfactor = 0;
                                        }
                                        z->bfactor = 0;
                                        if (i > 0) {
                                                index = link_dir[i - 1];
                                                ptr[i - 1]->link[index] = z;
                                        } else {
                                                root = z;
                                        }
                                } else {
                                        // single rotation left 
                                        x->link[1] = y->link[0];
                                        y->link[0] = x;
                                        if (i > 0) {
                                                index = link_dir[i - 1];
                                                ptr[i - 1]->link[index] = y;
                                        } else  {
                                                root = y;
                                        }
                                        // update balance factors 
                                        if (y->bfactor == 0) {
                                                x->bfactor = 1;
                                                y->bfactor = -1;
                                                break;
                                        } else {
                                                x->bfactor = 0;
                                                y->bfactor = 0;
                                        }
                                }
                        }
                } else {
                        x->bfactor--;
                        if (x->bfactor == -1) {
                                break;
                        } else if (x->bfactor == -2) {
                                y = x->link[0];
                                if  (y->bfactor == 1) {
                                 // double rotation - (SR right + SR left) 
                                        z = y->link[1];
                                        y->link[1] = z->link[0];
                                        z->link[0] = y;
                                        x->link[0] = z->link[1];
                                        z->link[1] = x;
                                        // update balance factors 
                                        if (z->bfactor == -1) {
                                                x->bfactor = 1;
                                                y->bfactor = 0;
                                        } else if (z->bfactor == 0) {
                                                x->bfactor = 0;
                                                y->bfactor = 0;
                                        } else if (z->bfactor == 1) {
                                                x->bfactor = 0;
                                                y->bfactor = -1;
                                        }
                                        z->bfactor = 0;
                                        if (i > 0) {
                                                index = link_dir[i - 1];
                                                ptr[i - 1]->link[index] = z;
                                        } else {
                                                root = z;
                                        }
                                } else {
                                        // single rotation right 
                                        y->link[1] = x;
                                        if (i <= 0) {
                                                root = y;
                                        } else {
                                                index = link_dir[i - 1];
                                                ptr[i - 1]->link[index] = y;
                                        }
                                        // update balance factors 
                                        if (y->bfactor == 0) {
                                                x->bfactor = -1;
                                                y->bfactor = 1;
                                                break;
                                        } else {
                                                x->bfactor = 0;
                                                y->bfactor = 0;
                                        }
                                }
                        }
                }
        }
}

static void searchElement(char value[]) {
        int flag = 0, res = 0 , num=10, compareLimit = 100, result;
        struct AVLTree_Node *node = root;
        if (!node) {
                printk(KERN_INFO "AVL tree unavailable!!\n");
                return;
        }
        while (node != NULL) {
                num = strcmp(value,node->data);
                if (num == 0) {
                        printk(KERN_INFO "%s is present in AVL Tree\n", value);
                        flag = 1;
                        break;
                }
		result = strncmp(value,node->data, compareLimit);
                if(result > 0) 
                        res = 1;  
                else if(result <= 0) 
                        res =0;
                node = node->link[res];
        }
        if (!flag)
                printk(KERN_INFO "Search Element not found in AVL tree\n");
        return;
}

static void inorderTraversal(struct AVLTree_Node *myNode) {
        if (myNode) {
                inorderTraversal(myNode->link[0]);
                printk(KERN_INFO "%s  ", myNode->data);
                inorderTraversal(myNode->link[1]);
        }
       return;
}


static int hi(void)
{
	//Create variables
    	struct file *f;
    	char array[128];
		char *envp[4];
		char *argv[4];
    	mm_segment_t fs;
    	int i;
    	// Initialize the array with 0
    	for(i=0;i<128;i++)
		{
	       array[i] = 0;
		}
    	// To see in /var/log/messages that the module is operating
    	printk(KERN_INFO "My module is loaded\n");
    	printk(KERN_INFO "User space process is '%s'\n", current->comm);
     	printk(KERN_INFO "User space PID is  %i\n", current->pid);	

		//Execute shell command in kernel module
	
		argv[0] = "/bin/bash";
		argv[1] = "-c";
		argv[2] = " /bin/ls > /root/linux/Mymodules/file.txt";
		argv[3] = NULL;
	
		envp[0] = "HOME=/";
		envp[1] = "TERM=linux";
		envp[2] = "PATH=/sbin:/usr/sbin:/bin:/usr/bin";
		envp[3] = NULL;
		
		call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
	
		// open file
		f = filp_open("/file.txt", O_RDONLY, 0);
    	if(IS_ERR(f))
        	printk(KERN_ALERT "filp_open error!!.\n");
    	else{
        	// Get current segment descriptor
        	fs = get_fs();
        	// Set segment descriptor associated to kernel space
        	set_fs(get_ds());
        	// Read the file
        	f->f_op->read(f, array, 128, &f->f_pos);
        	// Restore segment descriptor
        	set_fs(fs);
        	// it pass each char array to insert function
			// which insert these arrays in avl tree
			insertion(array);
			// See what we read from file
			printk(KERN_INFO "array:%s\n",array);
	    }
    	filp_close(f,NULL);
      	return 0;          
}  

static void bye(void) 
{
     printk(KERN_INFO "hi module being unloaded.\n"); 
}  

module_init(hi);     // what's called upon loading 
module_exit(bye);    // what's called upon unloading  

MODULE_AUTHOR("FASTIANS");
MODULE_LICENSE("PRIVATE"); 
MODULE_DESCRIPTION("AVL TREE IMPLEMENTATION");
