#include <cpu/exec.h>
#include "rtl.h"

void read_ModR_M(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val);


static inline void operand_reg(DecodeExecState *s, Operand *op, bool load_val, int r, int width) { //acquire a reg operand (value),put in op.
  op->type = OP_TYPE_REG;
  op->reg = r;

  if (width == 4) {
    op->preg = &reg_l(r);
    if(load_val) rtl_lr(s,&op->val,r,width);	
  } else {  
    assert(width == 1 || width == 2);
     if (load_val){ rtl_lr(s, &op->val, r, width);op->preg=&op->val;}
  }

  print_Dop(op->str, OP_STR_SIZE, "%%%s", reg_name(r, width));
}

static inline void operand_imm(DecodeExecState *s, Operand *op, bool load_val, word_t imm, int width) { //acquire a imm opreand (value),put in op.
  op->type = OP_TYPE_IMM;
  op->imm = imm;

  if (load_val) {//1:get op->val    2:op->preg=&op->val
    rtl_li(s, &op->val, imm);  //imm to op->val
    op->preg = &op->val; 
  }
  print_Dop(op->str, OP_STR_SIZE, "$0x%x", imm);
}

// i have a question?
// op->preg=&reg_l(r) is reasonable,because preg can directly change the value of reg_l(r)
// but if u put reg_val in val,then preg to val,preg cant change the value of reg,why not just uese op->reg?
// because if width=1,2 ,then the size of &reg_l(r) is 8 or 16,but preg is 32 ,so at this time ,better use preg to val









/* I386 manual does not contain this abbreviation.
 * We decode everything of modR/M byte in one time.
 */
/* Eb, Ew, Ev
 * Gb, Gv
 * Cd,
 * M
 * Rd
 * Sw
 */
static inline void operand_rm(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  read_ModR_M(s, rm, load_rm_val, reg, load_reg_val);
 
}


// decode operand helper
#define def_DopHelper(name) void concat(decode_op_, name) (DecodeExecState *s, Operand *op, bool load_val)

/* Refer to Appendix A in i386 manual for the explanations of these abbreviations */



/* Ib, Iv */
static inline def_DopHelper(I) {                                   //decode_op_I(s,op,load_val)
  /* pc here is pointing to the immediate */
  //if(cpu.pc==0x100b46 ) printf("s->seq_pc1:%x\n",s->seq_pc);
  word_t imm = instr_fetch(&s->seq_pc, op->width);
  //if(cpu.pc==0x100b46 ) printf("s->seq_pc22:%x\n",s->seq_pc);

  operand_imm(s, op, load_val, imm, op->width);
}


/* I386 manual does not contain this abbreviation, but it is different from
 * the one above from the view of implementation. So we use another helper
 * function to decode it.
 */
/* sign immediate */
static inline def_DopHelper(SI) {                                  //decode_op_SI(s,op,load_val)
  assert(op->width == 1 || op->width == 4);

  /* TODO: Use instr_fetch() to read `op->width' bytes of memory
   * pointed by 's->seq_pc'. Interpret the result as a signed immediate,
   * and call `operand_imm()` as following.
   *
   operand_imm(s, op, load_val, ???, op->width);
   */
  sword_t imm=instr_fetch(&s->seq_pc,op->width);
  operand_imm(s,op,load_val,imm,op->width);
}


/* I386 manual does not contain this abbreviation.
 * It is convenient to merge them into a single helper function.
 */
/* AL/eAX */
static inline def_DopHelper(a) {                                    //decode_op_a(s,op,load_val)
  operand_reg(s, op, load_val, R_EAX, op->width);
}



/* This helper function is use to decode register encoded in the opcode. */
/* XX: AL, AH, BL, BH, CL, CH, DL, DH
 * eXX: eAX, eCX, eDX, eBX, eSP, eBP, eSI, eDI
 */                                     
static inline def_DopHelper(r) {                                   //decode_op_r(s,op,load_val)
  operand_reg(s, op, load_val, s->opcode & 0x7, op->width);
}


/* Ob, Ov */
static inline def_DopHelper(O) {                                   //decode_op_O(s,op,load_val)
  op->type = OP_TYPE_MEM;
  s->isa.moff = instr_fetch(&s->seq_pc, 4);
  s->isa.mbase = rz;
  if (load_val) {
    rtl_lm(s, &op->val, s->isa.mbase, s->isa.moff, op->width);
    op->preg = &op->val;
  }

  print_Dop(op->str, OP_STR_SIZE, "0x%x", s->isa.moff);
}




static inline void operand_write(DecodeExecState *s, Operand *op, rtlreg_t* src) {
  if (op->type == OP_TYPE_REG) {rtl_sr(s, op->reg, src, op->width); }
  else if (op->type == OP_TYPE_MEM) {rtl_sm(s, s->isa.mbase, s->isa.moff, src, op->width); }
  else { assert(0); }
}






//E:rm in modRM        M:Min modRM       I:immidiate   
//R:general register in modRM,   C:control register,   D:debug register,   S:segment register,   T:test register
//A:direct address(no modRm,encoded in instrution)   O:offset(no modRm,encoded in instrution)

//b:byte        v:depended      


//--------------mov_G-E-I-r-------------------------

/* Eb <- Gb
 * Ev <- Gv
 */
static inline def_DHelper(G2E) {
  operand_rm(s, id_dest, true, id_src1, true);
}

static inline def_DHelper(mov_G2E) {
  operand_rm(s, id_dest, false, id_src1, true);
}

/* Gb <- Eb
 * Gv <- Ev
 */
static inline def_DHelper(E2G) {
  operand_rm(s, id_src1, true, id_dest, true);
}

static inline def_DHelper(mov_E2G) {
  //if(cpu.pc==0x10019c) printf("come in decode_E2G");
  operand_rm(s, id_src1, true, id_dest, false);
  
}



static inline def_DHelper(mov_byteE2G){
  id_src1->width=1;
  operand_rm(s,id_src1,true,id_dest,false);
}

static inline def_DHelper(lea_M2G) {
  operand_rm(s, id_src1, false, id_dest, false);
}

/* Eb <- Ib
 * Ev <- Iv
 */
static inline def_DHelper(I2E) {
  operand_rm(s, id_dest, true, NULL, false);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(mov_I2E) {
//printf("mov_I2E,cpu.pc%x\n",cpu.pc);
  operand_rm(s, id_dest, false, NULL, false);
  decode_op_I(s, id_src1, true);
}

/* Gv <- EvIb
 * Gv <- EvIv
 * use for imul */
static inline def_DHelper(I_E2G) {
  operand_rm(s, id_src2, true, id_dest, false);
  decode_op_I(s, id_src1, true);
}

/* XX <- Ib
 * eXX <- Iv
 */
static inline def_DHelper(I2r) {
  decode_op_r(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}

static inline def_DHelper(mov_I2r) {
  decode_op_r(s, id_dest, false);
  decode_op_I(s, id_src1, true);
}

/* AL <- Ib
 * eAX <- Iv
 */
static inline def_DHelper(I2a) {
  decode_op_a(s, id_dest, true);
  decode_op_I(s, id_src1, true);
}




//----------------------------- used by unary operations-----------------------
static inline def_DHelper(I) {
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(r) {
  decode_op_r(s, id_dest, true);
}

static inline def_DHelper(E) {

  operand_rm(s, id_dest, true, NULL, false);
    
}

static inline def_DHelper(setcc_E) {
  operand_rm(s, id_dest, false, NULL, false);
}

static inline def_DHelper(gp7_E) {
  operand_rm(s, id_dest, false, NULL, false);
 // if(cpu.pc==0x1005c9) printf("gp7_E,id_dest->val:%x\n",id_dest->val);
}


static inline def_DHelper(O2a) {
  decode_op_O(s, id_src1,true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(a2O) {
  decode_op_a(s, id_src1, true);
  decode_op_O(s, id_dest, false);
}

//----------------------------- used by test in group3-------------------------------

static inline def_DHelper(test_I) {
  decode_op_I(s, id_src1, true);
}


//E<-rm(E) sub/abb/and/add/adc...imm8
static inline def_DHelper(SI2E) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_dest, true, NULL, false);    //get E/id_dest
  
  id_src1->width = 1;//because if opcode =83,nomatter extcode =5(sub),3(sbb),4(and),2(adc),0(add),the src1 is forever imm8 

  decode_op_SI(s, id_src1, true);  //get simm/id_src1
  rtl_sext(s,&id_src1->val,&id_src1->val,1); 

  if (id_dest->width == 2) {//means get imm to 2 instead of 4
	  *dsrc1 &= 0xffff;
  }

 }
//G<- rm(E) sub/sbb/and/add/adc...imm8
static inline def_DHelper(SI_E2G) {
  assert(id_dest->width == 2 || id_dest->width == 4);
  operand_rm(s, id_src2, true, id_dest, false);  //get E and G

  id_src1->width = 1;
  decode_op_SI(s, id_src1, true);//get simm

  if (id_dest->width == 2) {  
	  *dsrc1 &= 0xffff;

  }
}

static inline def_DHelper(gp2_1_E) {
  operand_rm(s, id_dest, true, NULL, false);
  operand_imm(s, id_src1, true, 1, 1);
}

static inline def_DHelper(gp2_cl2E) {
  operand_rm(s, id_dest, true, NULL, false);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}

static inline def_DHelper(gp2_Ib2E) {

  operand_rm(s, id_dest, true, NULL, false);
  id_src1->width = 1;

  decode_op_I(s, id_src1, true);
//  if(cpu.pc==0x10188e) printf("src:%x\n",id_src1->val);
}

/* Ev <- GvIb
 * use for shld/shrd */
static inline def_DHelper(Ib_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
}

/* Ev <- GvCL
 * use for shld/shrd */
static inline def_DHelper(cl_G2E) {
  operand_rm(s, id_dest, true, id_src2, true);
  // shift instructions will eventually use the lower
  // 5 bits of %cl, therefore it is OK to load %ecx
  operand_reg(s, id_src1, true, R_ECX, 4);
}


static inline def_DHelper(J) {
  decode_op_SI(s, id_dest, false);
  // the target address can be computed in the decode stage
  s->jmp_pc = id_dest->simm + s->seq_pc;
}


static inline def_DHelper(in_I2a) {
  id_src1->width = 1;
  decode_op_I(s, id_src1, true);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(in_dx2a) {
  operand_reg(s, id_src1, true, R_DX, 2);
  decode_op_a(s, id_dest, false);
}

static inline def_DHelper(out_a2I) {
  decode_op_a(s, id_src1, true);
  id_dest->width = 1;
  decode_op_I(s, id_dest, true);
}

static inline def_DHelper(out_a2dx) {
  decode_op_a(s, id_src1, true);
  operand_reg(s, id_dest, true, R_DX, 2);
}



//control

static inline def_DHelper(call){
  decode_op_I(s,id_src1,true);
  //if(cpu.pc==0x100b46) printf("src:%x\n",id_src1->val);

  id_src1->val+=cpu.pc;
  //+=the width of the instr:displacement relative to next instruction
  id_src1->val+=1;               //opcode
  id_src1->val+=s->src1.width;   //imm width
  //if(cpu.pc==0x100b46) printf("src:%x\n",id_src1->val);

  cpu.pc+=1;
  cpu.pc+=s->src1.width;
  //why+=the width of the instr? because we want to store next the address of next instr of call,
  //if we store the address of call,then we will get a lop!!!
}

static inline def_DHelper(jcc){

  decode_op_SI(s,id_src1,true);
  if(id_src1->width==1)
  {
  rtl_sext(s,&id_src1->val,&id_src1->val,1);//need signal extention
  s->jmp_pc=cpu.pc+1+id_src1->width+id_src1->val; 
  }

  if(id_src1->width==2)
  {
  rtl_sext(s,&id_src1->val,&id_src1->val,2);//need signal extention

  s->jmp_pc=cpu.pc+1+id_src1->width+id_src1->val;
  }

  if(id_src1->width==4){
  s->jmp_pc=cpu.pc+1+id_src1->width+id_src1->val;//dont need signal extention
  }

}

static inline def_DHelper(jcc_l){
  decode_op_SI(s,id_src1,true);
  if(id_src1->width==1)
  {
  rtl_sext(s,&id_src1->val,&id_src1->val,1);//need signal extention
  s->jmp_pc=cpu.pc+1+id_src1->width+id_src1->val; 
  }

  if(id_src1->width==2)
  {
  rtl_sext(s,&id_src1->val,&id_src1->val,2);//need signal extention

  s->jmp_pc=cpu.pc+2+id_src1->width+id_src1->val;
  }

  if(id_src1->width==4){
  s->jmp_pc=cpu.pc+2+id_src1->width+id_src1->val;//dont need signal extention
  }

}




//data-mov
static inline def_DHelper(push_r){
    decode_op_r(s,id_dest,true);
}

static inline def_DHelper(pop_r){
    decode_op_r(s,id_dest,false);
}

static inline def_DHelper(push_SI) {
  decode_op_SI(s, id_dest, true);
}

static inline def_DHelper(push_I){
  decode_op_I(s,id_dest,true);
}

static inline def_DHelper(lea){
	operand_rm(s,id_src1,false,id_dest,false);  //why rm false ,because we want to store the value of address ,not pmem[address].

}

static inline def_DHelper(movsx){
	id_src1->width=1;
   if(s->isa.is_operand_size_16) id_dest->width=2;
   else id_dest->width=4;
   operand_rm(s,id_src1,true,id_dest,true);
}

static inline def_DHelper(movswl){
	id_src1->width=2;
	operand_rm(s,id_src1,true,id_dest,true);
}


static inline def_DHelper(movzwl){
	id_src1->width=2;
	operand_rm(s,id_src1,true,id_dest,true);

}

//logic -r-rm
static inline def_DHelper(xor_r2rm){
   operand_rm(s,id_dest,true,id_src1,true);
}
static inline def_DHelper(xor_rm2r){
	operand_rm(s,id_src1,true,id_dest,true);
}


//arith -r-rm

static inline def_DHelper(cmp_r2rm){
   operand_rm(s,id_dest,true,id_src1,true);
}
static inline def_DHelper(cmp_rm2r){
   operand_rm(s,id_src1,true,id_dest,true);
}

static inline def_DHelper(cmp_I2a){
   decode_op_a(s,id_dest,true);
   decode_op_I(s,id_src1,true);
}



static inline def_DHelper(inc_r){
   decode_op_r(s,id_dest,true);
}

static inline def_DHelper(dec_r){
   decode_op_r(s,id_dest,true);
}

static inline def_DHelper(add_r2rm){
  //	printf("add_r2rm\n");
  // printf("add_r2rm,cpu.pc:%x opcode:%x\n",cpu.pc,s->opcode);
   operand_rm(s,id_dest,true,id_src1,true);
 // if(cpu.pc>=0x3000000)  printf("add_r2rm,src:%x dest:%x\n",id_src1->val,id_dest->val);
}
static inline def_DHelper(add_rm2r){
   operand_rm(s,id_src1,true,id_dest,true);
}





























