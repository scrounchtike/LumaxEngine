
#include "cpuid.hpp"

cpuid::cpu_features cpuid::features;

cpuid_desc::RAW_DESC cpuid_desc::raw_desc;
cpuid_desc::CPU_DESC cpuid_desc::cpu;

void cpuid::get_system_info() {
	cpuid_desc::get_raw_data();
	cpuid_desc::process_data();
}

void cpuid::print_cpu_report(){
	std::cout << "System Info Report based on x86 CPUID instruction" << std::endl;
	
}

// Assembly code from Anrieff's libcpuid project
// at https://github.com/anrieff/libcpuid
void cpuid_desc::execute_cpuid(uint32* regs){
	__asm __volatile(
		"	mov	%0,	%%rdi\n"

		"	push	%%rbx\n"
		"	push	%%rcx\n"
		"	push	%%rdx\n"
		
		"	mov	(%%rdi),	%%eax\n"
		"	mov	4(%%rdi),	%%ebx\n"
		"	mov	8(%%rdi),	%%ecx\n"
		"	mov	12(%%rdi),	%%edx\n"
		
		"	cpuid\n"
		
		"	movl	%%eax,	(%%rdi)\n"
		"	movl	%%ebx,	4(%%rdi)\n"
		"	movl	%%edx,	8(%%rdi)\n"
		"	movl	%%ecx,	12(%%rdi)\n"
		
		"	pop	%%rdx\n"
		"	pop	%%rcx\n"
		"	pop	%%rbx\n"
		:
		:"m"(regs)
		:"memory", "eax", "rdi"
	);
}

void cpuid_desc::get_raw_data(){
	execute_cpuid((uint32*)(&raw_desc));
	execute_cpuid((uint32*)(&raw_desc)+4);
}

void cpuid_desc::process_data(){
	cpu.max_cpu_id = raw_desc.max_cpu_id;
	for(int i = 0; i < 3; ++i){
		cpu.vendor[i*4+0] = (uint8)((raw_desc.vendor_raw[i] & 0x000000FF) >> 0);
		cpu.vendor[i*4+1] = (uint8)((raw_desc.vendor_raw[i] & 0x0000FF00) >> 8);
		cpu.vendor[i*4+2] = (uint8)((raw_desc.vendor_raw[i] & 0x00FF0000) >> 16);
		cpu.vendor[i*4+3] = (uint8)((raw_desc.vendor_raw[i] & 0xFF000000) >> 24);
	}
	// Basic info
	cpu.stepping        = (raw_desc.processor_info & 0x0000000F);
	cpu.model           = (raw_desc.processor_info & 0x000000F0) >> 4;
	cpu.family          = (raw_desc.processor_info & 0x00000F00) >> 8;
	cpu.processor_type  = (raw_desc.processor_info & 0x00003000) >> 12;
	cpu.extended_model  = (raw_desc.processor_info & 0x000F0000) >> 16;
	cpu.extended_family = (raw_desc.processor_info & 0x0FF00000) >> 20;

	cpu.brand_index               = (raw_desc.feature_bits[0] & 0x000000FF);
	cpu.clflush_line_size         = (raw_desc.feature_bits[0] & 0x0000FF00) >> 8;
	cpu.max_logical_proessors_ids = (raw_desc.feature_bits[0] & 0x00FF0000) >> 16;
	cpu.local_apic_id             = (raw_desc.feature_bits[0] & 0xFF000000) >> 24;

	uint32 flag = 0;
	char* edx = (char*)(&cpu.onboard_x87_fpu);
	for(int i = 0; i < 32; ++i){
		*edx++ = (raw_desc.feature_bits[1] & (1 << flag++) >> i);
	}
}
