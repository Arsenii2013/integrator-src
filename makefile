################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

LIBS := -Wl,--start-group,-lxil,-lgcc,-lc,--end-group

all:
	$(MAKE) --no-print-directory pre-build
	$(MAKE) --no-print-directory main-build

# Main-build Target
main-build: integrator_application.elf secondary-outputs

# Tool invocations
integrator_application.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM v7 gcc linker'
	arm-none-eabi-gcc -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -Wl,-build-id=none -specs=Xilinx.spec -Wl,-T -Wl,../src/lscript.ld -L/home/sescer/fpga/integrator-hp/vitis/integrator_platform/export/integrator_platform/sw/integrator_platform/standalone_domain/bsplib/lib -o "integrator_application.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(S_UPPER_DEPS)$(C_DEPS)$(ELFSIZE) integrator_application.elf
	-@echo ' '

pre-build:
	-a9-linaro-pre-build-step
	-@echo ' '

test:
	gcc -lm -ggdb logger.c test_gen.c scheduler.c sin_integral_emulator.c main.c scr.c PSPL.c

.PHONY: all clean dependents main-build pre-build
