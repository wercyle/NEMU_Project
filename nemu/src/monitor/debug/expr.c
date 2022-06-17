#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>
#include "expr.h"
#include <memory/vaddr.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_Left,TK_Right,TK_Num,TK_letter,DEREF

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
 
  {"\\(",TK_Left},
  {"\\)",TK_Right},
  {" +", TK_NOTYPE},    // spaces
  {"[0-9]",TK_Num},
  {"[a-z]",TK_letter},

  {"\\+", '+'},         // plus
  {"\\-",'-'},          
  {"\\*",'*'},
  {"\\/",'/'},
  {"==", TK_EQ},        // equal

  {"\\$",'$'}           //reg
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {     //1111111
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}




typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens1[100] __attribute__((used)) = {};
static Token tokens[100] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
  int numnum=0;                       //the repetition of the num or letter,true nr_token=nr_token-numnum;
  int hexflag=0;                      //confirm cout hex or dec


////////////////////////make token-start//////////////////////////////


void inittoken(){                     //used for init the token
     for(int i=0;i<100;i++){
		 tokens[i].type=0;
		 tokens1[i].type=0;
		for(int j=0;j<32;j++){
           tokens1[i].str[j]='\0';
		   tokens[i].str[j]='\0';
		}
	 }
	 numnum=0;
	 hexflag=0;
}

int Hex_to_Dec(int aHex){            //hex to dec
	int DecArr[50]={0};
	int Dec=0,temp=0,count=0;
	while(aHex!=0){
		temp=aHex;
		aHex=aHex/16;
		temp=temp%16;
		DecArr[count++]=temp;
	}
	int j=0;
	for(int i=0;i<count;i++){
		if(i<1) Dec=Dec+DecArr[i];
		else {
			Dec=(Dec+(DecArr[i]*(16<<j)));
			j+=4;
		}
	}
	return Dec;
}



void changetokens(){ //used for tokens1 to tokens
      for(int i=0,se=0;i<nr_token;i++,se++){
	      tokens[se].type=tokens1[i].type;
		  if((tokens1[i].str[0]=='0')&&(tokens1[i+1].str[0]='x')){             // 0x:identify and change to dec
              int j=2;
			  numnum++; //cause the letter x
			  while((tokens1[i+j].type==TK_Num)||(tokens1[i+j].type==TK_letter)){
				  tokens1[i].str[j-2]=tokens1[i+j].str[0];
				  j++;
				  numnum++; //cause the num
			  }
			  int nValue=0;
			  sscanf(&tokens1[i].str[0],"%x",&nValue);
              int Dec=Hex_to_Dec(nValue);
              sprintf(&tokens[se].str[0],"%d",Dec);

			  i=i+j-1;
			  hexflag=1;
			  continue;
		  }
		  if((tokens1[i].type=='*')&&(i==0||(tokens1[i-1].type!=TK_Num&&tokens1[i-1].type!=TK_letter&&tokens1[i-1].type!=TK_Right))){  //DEREF
			 tokens[se].type=DEREF;
			 
              int j=1;
			  if(tokens1[i+1].type!=TK_Left){
				  while(tokens1[i+j].type==TK_Num||tokens1[i+j].type==TK_letter||tokens1[i+j].type=='$'){
					  tokens1[i].str[j-1]=tokens1[i+j].str[0];
					  j++;
					  numnum++;
				  }
			  
			  strcpy(&tokens[se].str[0],&tokens1[i].str[0]);
			  i=i+j-1;
			  continue;
			  
		  }
		  }


		  if(tokens1[i].type==TK_Num){              //num
			  int j=1;
			  while(tokens1[i+j].type==TK_Num){
				  tokens1[i].str[j]=tokens1[i+j].str[0];
				  j++;
				  numnum++;
			  }
			  strcpy(&tokens[se].str[0],&tokens1[i].str[0]);
			  i=i+j-1;
			  continue;

  }
           if(tokens1[i].type=='$'){               //$reg
			  int j=1;
			  while(tokens1[i+j].type==TK_letter){
				  tokens1[i].str[j-1]=tokens1[i+j].str[0];
				  j++;
				  numnum++;
			  }
			  strcpy(&tokens[se].str[0],&tokens1[i].str[0]);
			  i=i+j-1;
			  continue;

  }
  }
}






static bool make_token(char *e) {   //22222222
  inittoken();
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  int k=0;//k means put token in the tokens[] in sequence instead of i

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

       
        switch (rules[i].token_type) {
			case TK_NOTYPE:nr_token--;k--;break;

			case TK_Left: tokens1[k].type='(';break;
			case TK_Right: tokens1[k].type=')';break;
			case TK_Num: tokens1[k].type=TK_Num;
						tokens1[k].str[0]=e[position-1];break;
			case TK_letter: tokens1[k].type=TK_letter;
						 tokens1[k].str[0]=e[position-1];break;

			case '+': tokens1[k].type='+';break;
			case '-': tokens1[k].type='-';break;
			case '*': tokens1[k].type='*';break;
			case '/': tokens1[k].type='/';break;
			case TK_EQ: tokens1[k].type=TK_EQ;break;

			case '$': tokens1[k].type='$';
					  tokens1[k].str[0]='$';break;
          default: TODO();
        }
		k++;
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }


  for(int i=0;i<nr_token;i++){    //testing
	  printf(" %d",tokens1[i].type);
	  if(tokens1[i].type==TK_Num) {printf(" (%s",&tokens1[i].str[0]);printf(")");}
      if(tokens1[i].type==TK_letter) {printf(" (%s",&tokens1[i].str[0]);printf(")");}

  }
  printf("\n");
  

   changetokens();	      //change tokens
   nr_token-=numnum;

  
  for(int i=0;i<nr_token;i++){    //testing
	  printf(" %d",tokens[i].type);
	  if(tokens[i].type==TK_Num) {printf(" (%s",&tokens[i].str[0]);printf(")");}
	  if(tokens[i].type=='$'){printf(" (%s",&tokens[i].str[0]);printf(")");}
      if(tokens[i].type==DEREF){printf(" (%s",&tokens[i].str[0]);printf(")");}

  }
  printf("\n");

    return true;
}

///////////////////////////make token-end////////////////////////////





///////////////////////////calcu-start////////////////////////////////


bool check_parentheses(word_t p,word_t q){       //check if the parentheses matches
     if((tokens[p].type==40)&&(tokens[q].type==41)){
		 sword_t flag=0;     //record the relative of the left(++) or right(--)
		 bool ifright=true;  //record if matches
		 for(int i=p;i<=q;i++){
			 if(tokens[i].type==40) flag++;
			 if(tokens[i].type==41) flag--;
             if((flag==0)&&(i!=q)) ifright=false;
			 if(flag<0) assert(0);
		 }
		 return ifright;
	 }
	 else return false;
}

word_t FindMainOperator(word_t p,word_t q){    //find the main operator
	sword_t flag=0;     //record the relative of the left or right
	word_t mainop=p;
	for (int i=p;i<=q;i++){
		if(tokens[i].type==40) flag++;
		if(tokens[i].type==41) flag--;
		if((flag>0)||(i==q)||(tokens[i].type==260)||(tokens[i].type==261)||(tokens[i]).type=='$') continue;  //not in bracket,not num,not letter,not $
		
		if((tokens[mainop].type!=43)&&(tokens[mainop].type!=45)&&(tokens[mainop].type!=42)&&(tokens[mainop].type!=47)) { mainop=i;continue;}
		else if((tokens[mainop].type==42)||(tokens[mainop].type==47)) { mainop=i;continue;}
		else if((tokens[mainop].type==43)||(tokens[mainop].type==45)) {
			if((tokens[i].type==43)||(tokens[i].type==45)) { mainop=i;continue;}
		}
	}
    

	return mainop;
}

word_t eval(word_t p,word_t q){   //3333333333
	if(p>q) assert(0);
	else if(p==q) {
		if(tokens[p].type==TK_Num)	return atoi(&tokens[q].str[0]); //case1:dec or hex (but or return dec)(only printf hex or dec)
		else if(tokens[p].type=='$') {                              //case2:$reg
              word_t num=0;
			bool* ifsuccess=false;
			num=isa_reg_str2val(&tokens[p].str[0],ifsuccess);
			hexflag=1;
			  return num;
		}
		else if(tokens[p].type==DEREF){                             //case3:DEREF
			int value=0;
			if(tokens[p].str[1]=='x') {       //case3.1:DEREF-0x
				sscanf(&tokens[p].str[0],"%x",&value);
				value=Hex_to_Dec(value);
				hexflag=1;
			    return vaddr_read(value,4);
			}
			else if(tokens[p].str[0]=='$'){  //case3.2:DEREF-$reg
				bool* ifsuccess=false;
                value=isa_reg_str2val(&tokens[p].str[1],ifsuccess);
				hexflag=1;
				return vaddr_read(value,4);
			}
			else {
			sscanf(&tokens[p].str[0],"%d",&value); //case3.3:DEREF-dec
			hexflag=1;
			return vaddr_read(value,4);
			}
			
		}

	}
	else if(check_parentheses(p,q)==true) return eval(p+1,q-1);
    else {
		word_t op=FindMainOperator(p,q);
	   	
		word_t val1=eval(p,op-1);
		word_t val2=eval(op+1,q);
        switch (tokens[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;
			default:assert(0);
		}
	}
	return 0;
}


//////////////////////////////calcu-end////////////////////////////////////





word_t expr(char *e,bool *success) {   //444444444
  if (!make_token(e)) {
	  *success=false;
    return 0;
  }
   
  word_t result=eval(0,nr_token-1);//print hex or dec
  if (hexflag==0)  printf("%d\n",result);
  else if (hexflag==1) printf("%#x\n",result);

  return result;// only return dec
}
