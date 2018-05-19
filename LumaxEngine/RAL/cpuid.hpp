
#ifndef CPUID_HPP
#define CPUID_HPP

#include "buildDesc.hpp"

#include <string>
#include <iostream>

#include "../math/types.hpp"

class cpuid {
private:
	struct cpu_features {
		bool sse1;
		bool sse2;
		bool sse3;
		bool ssse3;
		bool sse4_1;
		bool sse4_2;
		bool mmx;
		bool avx;
		bool aes;
	};
	static cpu_features features;
public:
	static bool is_MMX_supported() { return features.mmx; }
	static bool is_SSE1_supported() { return features.sse1; }
	static bool is_SSE2_supported() { return features.sse2; }
	static bool is_SSE3_supported() { return features.sse3; }
	static bool is_SSSE3_supported() { return features.ssse3; }
	static bool is_SSE4_1_supported() { return features.sse4_1; }
	static bool is_SSE4_2_supported() { return features.sse4_2; }
	static bool is_AVX_supported() { return features.avx; }
	static bool is_AES_supported() { return features.aes; }

	static std::string get_cpu_vendor();
	

	static void get_system_info();
	static void print_cpu_report();
};

class cpuid_desc {
private:
	static void execute_cpuid(uint32* regs);
public:
	static void get_raw_data();
	static void process_data();
	
	struct RAW_DESC {
		// EAX = 0
		uint32 max_cpu_id; // Max possible eax value
		uint32 vendor_raw[3]; // 12 char vendor string
		
		// EAX = 1
		uint32 processor_info;
		uint32 feature_bits[3];
		
		// EAX = 2
		uint32 cache_info[4];
		
		// EAX = 3
		uint32 processor_serial[4];
		
		// EAX = 4
		uint32 intel_topology[4];
		
		// EAX = 5,6
		uint32 __und1__[8];
		
		// EAX = 7
		uint32 __eax7__;
		uint32 ext_feature_bits[3];
	};
	static RAW_DESC raw_desc;
	
	struct CPU_DESC {
		// EAX = 0
		uint32 max_cpu_id;
		uint8 vendor[13] = { 0 };
		
		// EAX = 1, EAX register
		char stepping;
		char model;
		char family;
		char processor_type;
		char extended_model;
		char extended_family;
		
		// EAX = 1, EBX register
		char brand_index;
		char clflush_line_size;
		char max_logical_proessors_ids;
		char local_apic_id;
		
		// EAX = 1, EDX register
		bool onboard_x87_fpu;
		bool virtual_8086_extensions;
		bool debugging_extensions;
		bool page_size_extension;
		bool time_stamp_counter;
		bool model_specific_registers;
		bool physical_address_extension;
		bool machine_check_exception;
		bool CMP_and_SWAP_inst;
		bool apic;
		bool __reserved1__;
		bool SYSENTER_EXIT_inst;
		bool memory_type_range_registers;
		bool page_global_enable_bit;
		bool machine_check_architecture;
		bool conditional_move_inst;
		bool page_attribute_table;
		bool page_size_extension_36bit;
		bool processor_serial_number;
		bool clflush_inst;
		bool __reserved2__;
		bool debug_store;
		bool acpi;
		bool mmx;
		bool FXSAVE_FXRESTOR_inst;
		bool sse;
		bool sse2;
		bool cpu_cache_support;
		bool hyper_threading;
		bool thermal_monitor_auto_limit_temp;
		bool IA64_emulating_x86;
		bool pending_break_enable;
		
		// EAX = 1, ECX register
		bool sse3;
		bool PCLMULQDQ_support;
		bool debug_store_64bit;
		bool MONITOR_MWAIT_inst;
		bool cpl_qualified_debug_store;
		bool virtual_machine_extensions;
		bool safer_mode_extensions;
		bool enhanced_speed_step;
		bool thermal_monitor2;
		bool supplemental_sse3;
		bool l1_context_id;
		bool silicon_debug_interface;
		bool fused_multiply_add;
		bool CMPXCHG16B_inst;
		bool can_disable_sending_task_priority_messages;
		bool perfmon_capability;
		bool __reserved3__;
		bool process_context_ids;
		bool direct_cache_access;
		bool sse4_1;
		bool sse4_2;
		bool x2APIC_support;
		bool MOVBE_inst;
		bool POPCNT_inst;
		bool APIC_TSC_deadline_support;
		bool aes;
		bool XSAVE_XRESTOR_XSETBV_XGETBV_inst;
		bool os_enabled_xsave;
		bool avx;
		bool half_precision_fp_support;
		bool on_chip_RNG_support;
		bool hypervisor;
	};
	static CPU_DESC cpu;
};

#endif
