/*	**********************************************
 * 	数据结构：二叉树 -- C语言程序
 *	简介：左树比当前节点数据小，右树等于或大于当前结点数据。
 *	作者：robe.zhang (82796620@qq.com)
 *	日期：2019.04.21
 *	*********************************************
 */

#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

/*	**********************************************
 * 	二叉树节点结构体:
 *	data：节点的数据
 *	lp：左树指针
 *	rp:右树指针
 *	**********************************************
 */
struct node{
	int data;
	struct node *lp;
	struct node *rp;
};
typedef struct node node_t;

/*	**********************************************
 * 	二叉树节点操作方法: 插入
 *	root: 要插入的树的根节点
 *	value：要插入的节点的数据
 *	return: -1；malloc出错，0：插入正常。 
 *	**********************************************
 */
int insert_node(node_t** root, int value){
	if( *root == NULL){
		*root = malloc(sizeof(struct node));
		if(*root == NULL){
			printf("malloc error.\n");
			return -1;
		}
		(*root)->data = value ;
		(*root)->lp = NULL;
		(*root)->rp = NULL;
	}else{
		if( value >= (*root)->data ){
			insert_node(&((*root)->rp),value);
		}else{
			insert_node(&((*root)->lp),value); 
		}
	}
	return 0;
}

/*	**********************************************
 * 	二叉树节点操作方法: 查找 		search_node
 *	root：要查找节点的树根
 *	value：要查找的节点的值
 *	temp：函数内部当作中间变量使用的 
 *	parent：用于接受查找到的节点的父节点 
 *	this：用于接受查找到的节点
 *	**********************************************
 */

int search_node(node_t** root, int value, node_t** temp,node_t** parent, node_t** this){
	if((*root) != NULL){
		if( (*root)->data == value ){
			*this = *root;
			*parent = *temp;
			return 0;
		}
		*temp = *root;
		if( value > (*root)->data  ){
			search_node(&((*root)->rp),value,temp,parent,this);
		}
		if( value < (*root)->data ){
			search_node(&((*root)->lp),value,temp,parent,this);
		}
	}
	return 0;
}

int search_node_l(node_t** root,node_t** temp, node_t** parent, node_t** this){
	if( *root != NULL ){
		if((*root)->lp == NULL){
			*this = *root;
			*parent = *temp;
			return 0;
		}
		*temp = *root;
		search_node_l(&((*root)->lp),temp,parent,this);
	}
	return 0;
}

int search_node_r(node_t** root, node_t** temp, node_t** parent, node_t** this){
	if( *root != NULL ){
		if((*root)->rp == NULL){
			*this = *root;
			*parent = *temp;
			return 0;
		}
		*temp = *root;
		search_node_l(&((*root)->rp),temp,parent,this);
	}
	return 0;
}

/*	**********************************************
 * 	二叉树节点操作方法: 删除
 *	root：要删除节点的树的根
 *	value：要删除的节点的值
 *	删除算法：
 *	1，找到删除节点
 *	2，查找删除节点的替代节点(右树最左节点)(插入算法时候，大于等于都插入右子节点)
 *	3，把替代节点先从树中去除，不free。
 *	4，用替代节点去替换要删除节点。
 *	5，free 删除节点
 *	**********************************************
 */

int delete_node(node_t** root, int value){
	node_t *delp=NULL,*del=NULL,*temp=NULL;	
	node_t *delp_=NULL,*del_=NULL,*temp_=NULL;
	int ret = 0;
	node_t *rep;

	search_node(root,value,&temp,&delp,&del);
	if(*root == NULL){									//树根不存在
		ret=-2;
		goto end_delete_node;
	}
	if( del == NULL){									//要删除的节点不存在
		ret=-1;
		goto end_delete_node;
	}
	search_node_l(&(del->rp),&temp_,&delp_,&del_);
	if(delp_!=NULL){									//找替换的节点，先处理替换节点
		if(del_->rp != NULL){
			delp_->lp = del_->rp;
		}else{
			delp_->lp = NULL;
		}
		ret = ret + 1 ;
	}else{
		if(del_!=NULL){
			if(del_->rp != NULL){
				del->rp = del_->rp;
			}else{
				del->rp = NULL;
			}
			ret = ret + 2 ;
		}
	}
	if(delp!=NULL){										//删除节点有父节点
		if(del_!=NULL){
			del_->rp = del->rp;
			del_->lp = del->lp;
			del->rp = NULL;
			del->lp = NULL;
			if(delp->rp == del){
				delp->rp = del_;
			}else{
				delp->lp = del_;
			}
			ret = ret + 4 ;
		}else{
			if(delp->lp == del){
				delp->lp = del->lp;
			}else{
				delp->rp = del->lp;
			}
		}
		free(del);
	}else{				//删除节点无父节点，删除的是根节点
		if((del!=NULL)){
			if(del_!=NULL){								//删除节点存在，是根节点，有替代节点
				del_->rp = del->rp;
				del_->lp = del->lp;
				del->rp = NULL;
				del->lp = NULL;
				(*root) = del_;
				free(del);
				ret = ret + 8 ;;
			}else{										//删除节点存在，是根节点，没有替代节点
				temp = *root;
				*root = (*root)->lp;
				free(temp);
				temp = NULL;
			}
		};
	}
#if 0
#endif
	end_delete_node:
	return ret;
}


/*	**********************************************
 * 	二叉树节点操作方法:  打印
 *	root：要打印的树的根
 *	position: 打印格式
 *	**********************************************
 */
int print_tree(node_t** root, int position){
	int loop;

	if ( (*root) != NULL ) {
		print_tree(&((*root)->lp), position + 4);
		for (loop = 0; loop < position; loop++) {
			printf(" ");
		}
		printf("%d\n", ((*root)->data));
		print_tree(&((*root)->rp), position + 4);
	}
	return 0;
}

/*	**********************************************
 * 	二叉树应用程序
 *	**********************************************
 */
#define SIZE_OF_DATA 0x10

int main(void)
{
	int arr[SIZE_OF_DATA];

	int i;
	node_t *rootPtr = NULL ;
	node_t *nodep = NULL;
	node_t *nodes = NULL;
	node_t *nodet = NULL;
	node_t *temp = NULL;
	int ret =0;
	

	srand(time(NULL));
	printf("\n================ general %2d rand data ==========\n\n",SIZE_OF_DATA);

	
	for (i = 0; i < SIZE_OF_DATA; i++) {
		arr[i] = rand()%100;
		printf("%4d", arr[i]);
	}
	
	printf("\n\n=========== insert and print the tree ===========\n\n");

	for (i = 0; i < SIZE_OF_DATA; i++) {
		insert_node(&rootPtr, arr[i]);
	}
	print_tree(&rootPtr, 2);

	printf("\n\n=============== node search result ==============\n\n");

	for (i = 0; i < SIZE_OF_DATA; i++) {
		nodep = NULL;
		nodes = NULL;
		temp = NULL;
		ret = search_node(&rootPtr,arr[i],&temp,&nodep,&nodes);
		printf("No.%2d: data is [%2d]. parent is ",i+1,arr[i]);
		if(nodep==NULL)printf("[null]");
		if(nodep!=NULL)printf("[%4d]",nodep->data);
		printf(" left child is ");
		if(nodes->lp == NULL )
			printf("[null]");
		if(nodes->lp != NULL )
			printf("[%4d]",nodes->lp->data);
		printf(" right child is ");
		
		if(nodes->rp != NULL )
			printf("[%4d].\n",nodes->rp->data);
		if(nodes->rp == NULL )
			printf("[null].\n");
	}

	printf("\n\n=================  search node =================\n\n");

	for (i = 0; i < SIZE_OF_DATA; i++) {
		nodep = NULL;
		nodes = NULL;
		temp = NULL;
		search_node(&rootPtr,arr[i],&temp,&nodep,&nodes);
		printf("No.%2d: data is [%2d]. r-tree left is ",i+1,arr[i]);
		nodet = nodes;
		nodep = NULL;
		nodes = NULL;
		temp = NULL;
		search_node_l(&(nodet->rp),&temp,&nodep,&nodes);
		if(nodes==NULL)printf("[null].\n");
		if(nodes!=NULL)printf("[%4d].\n",nodes->data);
	}
	printf("\n");
	for (i = 0; i < SIZE_OF_DATA; i++) {
		nodep = NULL;
		nodes = NULL;
		temp = NULL;
		search_node(&rootPtr,arr[i],&temp,&nodep,&nodes);
		printf("No.%2d: data is [%2d]. l-tree right is ",i+1,arr[i]);
		nodet = nodes;
		nodep = NULL;
		nodes = NULL;
		temp = NULL;
		search_node_r(&(nodet->lp),&temp,&nodep,&nodes);
		if(nodes==NULL)printf("[null].\n");
		if(nodes!=NULL)printf("[%4d].\n",nodes->data);
	}

	printf("\n\n================ delete a node  ================\n\n");

	print_tree(&rootPtr, 2);
	
	i = rand()%SIZE_OF_DATA -1 ;
	delete_node(&rootPtr,arr[i]);
	printf("------------------ delete [ %d ] ------------\n",arr[i]);
	print_tree(&rootPtr, 2);

	printf("\n\n==================== end  =====================\n\n");
	
	return 0; 

}

