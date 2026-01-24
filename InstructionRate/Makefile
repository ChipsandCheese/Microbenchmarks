include ../Common/arch_detect.mk

CFLAGS = -O3

all: $(TARGET)

amd64:
	$(CC) $(CFLAGS) x86_instructionrate.s x86_instructionrate.c -o InstructionRate_amd64 $(LDFLAGS)

aarch64:
	$(CC) $(CFLAGS) -march=native -pthread arm_instructionrate.s arm_instructionrate.c -o InstructionRate_aarch64 $(LDFLAGS)

<<<<<<< Updated upstream
=======
riscv64:
	$(CC) $(CFLAGS) -march=rv64gc -pthread riscv_instructionrate.s riscv_instructionrate.c -o InstructionRate_riscv64 $(LDFLAGS)

>>>>>>> Stashed changes
termux:
	clang -march=armv8+aes arm_instructionrate.s arm_instructionrate.c -o InstructionRate_aarch64 $(LDFLAGS)

ppc64:
	$(CC) $(CFLAGS) -mcpu=native -c ppc64_instructionrate_tests.c -S ppc64_instructionrate_tests.s
	$(CC) $(CFLAGS) -mcpu=native ppc64_instructionrate_tests.s ppc64_instructionrate.c -o ppc64_instructionrate $(LDFLAGS)
amd64_fusion:
	$(CC) $(CFLAGS) x86_fusion.s x86_fusion.c -o InstructionRateFusion_amd64 $(LDFLAGS)

w64:
	$(CC) $(CFLAGS) x86_instructionrate.c x86_instructionrate.s -o InstructionRate_w64.exe $(LDFLAGS)

<<<<<<< Updated upstream
ci: amd64 amd64_fusion aarch64 w64
=======
ci: amd64 amd64_fusion aarch64 riscv64 w64
>>>>>>> Stashed changes

clean:
	rm -f *.o && find . -type f -executable -delete

.PHONY: all ci clean
