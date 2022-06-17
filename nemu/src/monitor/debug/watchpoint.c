#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {                 //why array? easy to creat a link one step by for cycle
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP* new_wp(){
    if (free_==NULL) assert(0);
	else {
		//detete a wp from free_
		WP* Tmp=free_;
		free_=free_->next;
		Tmp->next=NULL;
		//insert a wp to head
		if(head==NULL) head=Tmp;
		else {Tmp->next=head;head=Tmp;}


		return Tmp ;
	}
}



void init_wp(WP *wp){
	for(int i=0;i<32;i++){
		wp->exp[i]='\0';
	}
	wp->value=0;
}

void free_wp1(WP *wp){
	//init
	init_wp(wp);
	//delete a wp from head
	if(head->next==NULL) head=NULL;
	else head=head->next;
	//insert a wp tp free_
	wp->next=free_;
	free_=wp;
}

void free_wp(int N){
	 WP* Tmp=head;
	 if(Tmp->NO==N){       //NO:N watchpoint is at head
		 init_wp(Tmp);
         //delete  head
	     if(head->next==NULL) head=NULL;
	     else head=head->next;
	     //insert a wp tp free_
	     Tmp->next=free_;
	     free_=Tmp;
	 }
	 while(Tmp->next!=NULL){     //NO:N watchpoint is not at head
			 if(Tmp->next->NO==N){
		     init_wp(Tmp->next);
			 WP* Tmp2=Tmp->next;
			 Tmp->next=Tmp->next->next;

			 Tmp2->next=free_;
			 free_=Tmp2;
			 break;
		 }
			 Tmp=Tmp->next;
	 }
}



WP* whereishead(){
	return head;
}

WP* whereisfree(){
	return free_;
}
