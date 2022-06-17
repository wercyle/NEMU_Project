#include <dlfcn.h>

#include <isa.h>
#include <memory/paddr.h>
#include <monitor/monitor.h>

void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n) = NULL;
void (*ref_difftest_getregs)(void *c) = NULL;
void (*ref_difftest_setregs)(const void *c) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;

static bool is_skip_ref = false;
static int skip_dut_nr_instr = 0;

// this is used to let ref skip instructions which
// can not produce consistent behavior with NEMU
void difftest_skip_ref() {
  is_skip_ref = true;
  // If such an instruction is one of the instruction packing in QEMU
  // (see below), we end the process of catching up with QEMU's pc to
  // keep the consistent behavior in our best.
  // Note that this is still not perfect: if the packed instructions
  // already write some memory, and the incoming instruction in NEMU
  // will load that memory, we will encounter false negative. But such
  // situation is infrequent.
  skip_dut_nr_instr = 0;
}

// this is used to deal with instruction packing in QEMU.
// Sometimes letting QEMU step once will execute multiple instructions.
// We should skip checking until NEMU's pc catches up with QEMU's pc.
// The semantic is
//   Let REF run `nr_ref` instructions first.
//   We expect that DUT will catch up with REF within `nr_dut` instructions.
void difftest_skip_dut(int nr_ref, int nr_dut) {
  skip_dut_nr_instr += nr_dut;//2 we expect that dut will catch up with ref within nr_dut instrutions

  while (nr_ref -- > 0) {//1 ref run nr_ref step
    ref_difftest_exec(1);
  }
}

void init_difftest(char *ref_so_file, long img_size, int port) {
#ifndef DIFF_TEST
  return;
#endif

  assert(ref_so_file != NULL);
  // dynamic libary file(x86-qemu-so,the compiled executable file)
  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  assert(handle);
  // read api sign from the dynamic libary file
  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(int) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);

  //init and run the  QEMU
  ref_difftest_init(port);
  //copy dut memory to ref
  ref_difftest_memcpy_from_dut(IMAGE_START + PMEM_BASE, guest_to_host(IMAGE_START), img_size);
  //copy dut regs to ref
  ref_difftest_setregs(&cpu);
}

static void checkregs(CPU_state *ref, vaddr_t pc) {

  if (!isa_difftest_checkregs(ref, pc)) {
    isa_reg_display();
    nemu_state.state = NEMU_ABORT;
    nemu_state.halt_pc = pc;
  }
}

void difftest_step(vaddr_t this_pc, vaddr_t next_pc) {

  CPU_state ref_r;


  //note: difftest_skip_ref and difftest_skip_dut means we meet situation that need skip,and do something preparatory work
  // the following means we now need to figure out these problems!
  // Q:where do we use the skip function?
  if (skip_dut_nr_instr > 0) {
    ref_difftest_getregs(&ref_r);
    if (ref_r.pc == next_pc) {
      checkregs(&ref_r, next_pc);
      skip_dut_nr_instr = 0;
      return;
    }
    skip_dut_nr_instr --;
    if (skip_dut_nr_instr == 0)
      panic("can not catch up with ref.pc = " FMT_WORD " at pc = " FMT_WORD, ref_r.pc, this_pc);

    //if next_pc==ref_r.pc,means dut.pc catch ref within skip_dut_nr_instr
	//if next_pc!=ref_r.pc && skip_dut_nr_instr!=0, means return and dut run 1 step
	//if next_pc!=ref_r.pc && skip_dut_nr_instr==0,means we cant catch up ref within skip_dut_nr_instr
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
    return;
  }



  ref_difftest_exec(1);// ref run 1 step

  ref_difftest_getregs(&ref_r);//get ref reg

  checkregs(&ref_r, this_pc);//get dut reg, and compare
}
