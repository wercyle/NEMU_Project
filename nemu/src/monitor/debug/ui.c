#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include "memory/paddr.h"
#include"memory/vaddr.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {     //continue to run
  cpu_exec(-1);   //src/monitor/cpu_exec.c
  return 0;
}

static int cmd_si(char *args) {   //run single step
	if(args==NULL) cpu_exec(1);
    else cpu_exec(atoi(args));
  return 0;
}


static int cmd_q(char *args) {    //quit
  return -1;
}

static int cmd_help(char *args);   //help


static int isa_display(char *args){ //print thr reg or watch point
	if(strcmp(args,"r")==0) isa_reg_display();
    if (strcmp(args,"w")==0){
        WP* head=whereishead();
		while(head!=NULL){
			printf("%d\t%s\t%x\n",head->NO,&head->exp[0],head->value);
			head=head->next;
		}
	}

    if (strcmp(args,"wfree")==0){
        WP* free=whereisfree();
		while(free!=NULL){
			printf("%d\t%s\t%d\n",free->NO,&free->exp[0],free->value);
			free=free->next;
		}
	}


    return 0;
}

static int cmd_x(char *args){      //scan the memory
	char *N=strtok(NULL," ");
	char *EXPR=strtok(NULL," ");
	int len;
	paddr_t address;

	sscanf(N,"%d",&len);
	sscanf(EXPR,"%x",&address);

	printf("0x%08x",address);
	for(int i=0;i<len;i++){
		printf(" %08x",vaddr_read(address,4));
		address+=4;
	}
    printf("\n");

	return 0;

}

static int cmd_expr(char *args){    //calculate the expr for p comand
   bool *ifsuccess=false;
   int result;

   result=expr(args,ifsuccess);
   if(result>=0)   return result;
   else return 0;


}

static int cmd_expr_copy(char *args){    //calculate the expr for w comand
   bool *ifsuccess=false;
   int result;

   result=expr(args,ifsuccess);
   return result;


}
static int cmd_w(char *exp){      //add watchpoint
	WP* watch=new_wp();
    strcpy(&watch->exp[0],exp);
	watch->value=cmd_expr_copy(exp);
    
	return 0;
}
static int cmd_d(char *args){    //delete NO:N watchpoint
	free_wp(atoi(args));
	return 0;
}


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","Single step",cmd_si},
  {"info","Print Register",isa_display},
  {"x","scan the memory",cmd_x},
  {"p","caculate",cmd_expr},
  {"w","add watchpoint",cmd_w},
  {"d","delete NO:N watchpoint",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}



/////////////////////////////////////////////ui_mainloop////////////////////////////////

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {//start
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }

  }//end
}




//-----------------------------------------------------------------------------------------

