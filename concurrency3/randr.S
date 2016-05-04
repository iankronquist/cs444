.intel_syntax noprefix

#ifdef __APPLE__
#define cpu_supports_rdrand _cpu_supports_rdrand
#define native_get_random_number _native_get_random_number
#endif

# uint32_t cpu_supports_rdrand()
.global cpu_supports_rdrand
cpu_supports_rdrand:
	# Registers ECX, EAX, and EDX are caller saved
	mov eax, 0x1
	cpuid
	mov eax, ecx
	and eax, (1<<30)
	ret

.global native_get_random_number
native_get_random_number:
	rdrand eax
	ret
