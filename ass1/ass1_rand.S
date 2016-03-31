.intel_syntax noprefix

# uint32_t cpu_supports_rdrand()
.global _cpu_supports_rdrand
_cpu_supports_rdrand:
	# Registers ECX, EAX, and EDX are caller saved
	cpuid
	mov edx, eax
	and edx, (1<<30)
	ret

.global _native_get_random_number
_native_get_random_number:
	rdrand eax
	ret
