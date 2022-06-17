static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
 
//  if(cpu.pc==0x10078d){if(*dsrc1!=0) printf("mov,al:%x\n",*dsrc1);}
//  if(cpu.pc==0x1004ab)printf("id_src->val:%x\n",id_src1->val);
//  if(cpu.pc==0x3002a44) printf("mov,dsrc1:%x\n",*dsrc1);
  print_asm_template2(mov);

}

static inline def_EHelper(push) {
	if(cpu.pc==0x100216) printf("push,dest:%x ebx:%x\n",id_dest->val,cpu.ebx);
    rtl_push(s,&id_dest->val); 
	
  print_asm_template1(push);
  
}

static inline def_EHelper(pop) {
  rtl_pop(s,t1);

  operand_write(s,id_dest,t1);

  print_asm_template1(pop);


}

static inline def_EHelper(pusha) {
  if(s->isa.is_operand_size_16){
    uint16_t temp =cpu.gpr[4]._16;
	rtl_pusha(s,&cpu.gpr[0]._16);
    rtl_pusha(s,&cpu.gpr[1]._16);
    rtl_pusha(s,&cpu.gpr[2]._16);
    rtl_pusha(s,&cpu.gpr[3]._16);
    rtl_pusha(s,&temp);
    rtl_pusha(s,&cpu.gpr[5]._16);
    rtl_pusha(s,&cpu.gpr[6]._16);
    rtl_pusha(s,&cpu.gpr[7]._16);
  }
  else{
    uint32_t temp=cpu.esp;
	rtl_push(s,&cpu.eax);
    rtl_push(s,&cpu.ecx);
    rtl_push(s,&cpu.edx);
    rtl_push(s,&cpu.ebx);
    rtl_push(s,&temp);
    rtl_push(s,&cpu.ebp);
    rtl_push(s,&cpu.esi);
    rtl_push(s,&cpu.edi);

  }
    print_asm("pusha");
}

static inline def_EHelper(popa) {
  if(s->isa.is_operand_size_16){
     uint16_t temp=0;
	 rtl_popa(s,&cpu.gpr[7]._16);
     rtl_popa(s,&cpu.gpr[6]._16);
     rtl_popa(s,&cpu.gpr[5]._16);
	 rtl_popa(s,&temp);
     rtl_popa(s,&cpu.gpr[3]._16);
     rtl_popa(s,&cpu.gpr[2]._16);
     rtl_popa(s,&cpu.gpr[1]._16);
     rtl_popa(s,&cpu.gpr[0]._16);
  }
  else{
     uint32_t temp=0;
	 rtl_pop(s,&cpu.edi);
     rtl_pop(s,&cpu.esi);
     rtl_pop(s,&cpu.ebp);
     rtl_pop(s,&temp);
     rtl_pop(s,&cpu.ebx);
     rtl_pop(s,&cpu.edx);
     rtl_pop(s,&cpu.ecx);
     rtl_pop(s,&cpu.eax);
  }

  print_asm("popa");
}

static inline def_EHelper(leave) {
  rtl_mv(s,&cpu.esp,&cpu.ebp);
  rtl_pop(s,&cpu.ebp);

  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    rtl_msb(s,t1,(unsigned int *)&cpu.gpr[0]._16,2);
	if(*t1) rtl_mv(s,(unsigned int*)&cpu.gpr[2]._16,r2);
	else rtl_mv(s,(unsigned int*)&cpu.gpr[2]._16,rz);
  }
  else {
    rtl_msb(s,t1,&cpu.eax,4);
	if(*t1) rtl_mv(s,&cpu.edx,r4);
	else rtl_mv(s,&cpu.edx,rz);
  }
  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    rtl_sext(s,(unsigned int*)&cpu.gpr[0]._16,(unsigned int*)&cpu.gpr[0]._8[0],1);
  }
  else {
    rtl_sext(s,&cpu.eax,(unsigned int*)&cpu.gpr[0]._16,2);
  }
  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);

	
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4; //compared to mov,just more this? i think this is unnecessary
  operand_write(s, id_dest, dsrc1);
 // if(cpu.pc==0x100267) printf("movzx,src:%x\n",*dsrc1);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest); //operand_write(s,operand* op,rtlreg_t *src) 

//  if(cpu.pc==0x100264)printf("lea,edx:%x\n",*ddest);


  print_asm_template2(lea);
}

static inline def_EHelper(xchg){
   rtl_mv(s,t1,&id_dest->val);
     if(id_dest->width==2){
        id_dest->val=cpu.gpr[0]._16;
		cpu.gpr[0]._16=*t1;  
	 // wrong:because,different pointers cant assigned,but num can!    if a uint32 = uint16,then the high 16bytes will be 0!!	
     // rtl_mv(s,&id_dest->val,&cpu.gpr[0]._16); 
	 // rtl_mv(s,&cpu.gpr[0]._16,t1);
   }
   else if (id_dest->width==4){
      rtl_mv(s,&id_dest->val,&cpu.eax);
	  rtl_mv(s,&cpu.eax,t1);
   }
}



static inline def_EHelper(movsb){
   // if(cpu.pc==0x3003824) printf("1:cpu.esi:%x cpu.edi:%x\n",cpu.esi,cpu.edi);
    int width=id_dest->width;
	 rtl_lm(s,t1,&cpu.esi,0,width);
  // if(cpu.pc==0x3003824) printf("t1:%x  width:%x\n",*t1,width);

	 rtl_sm(s,&cpu.edi,0,t1,width);
 // if(cpu.pc==0x3003824) printf("2:cpu.esi:%x cpu.edi:%x\n",cpu.esi,cpu.edi);

	if(cpu.DF==0) {cpu.esi+=width;cpu.edi+=width;}
	else if(cpu.DF==1) {cpu.esi-=width;cpu.edi-=width;}
 // if(cpu.pc==0x3003824) printf("3:cpu.esi:%x cpu.edi:%x\n\n",cpu.esi,cpu.edi);

}






