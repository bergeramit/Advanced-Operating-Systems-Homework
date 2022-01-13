# Assignment 1
## The first part (individuals) is submitted using git

### Amit Berger
git: https://github.com/bergeramit/Advanced-Operating-Systems-Homework.git (under assignment1/spawn_new_process/ ) 
```
make
# showcase the working machanism
make test
```

### Guy Berger
will submit its own

## The second part (together) is submitted using git

We programmed in this repo: (we wanted to fork from the original repo but got into trouble there so we just created a new repo and added a mention from the original source)

For testing: https://github.com/bergeramit/Advanced-Operating-Systems-Homework.git (under assignment1/ptree_excercise/test_syscall/)
```
make
test 1
```

For linux changes and kernel module: https://github.com/bergeramit/Linux_for_Advance_OS_HW.git

Note: for dummy testing just register the get_ptree_dummy instead and run make (will take care of everything - even modprobe -r the last module and depmod the new one)

# Assignment 2
## The first part (individuals) is submitted using git

### Amit Berger
#### Question 1
git: https://github.com/bergeramit/Advanced-Operating-Systems-Homework.git (under assignment2/Timing_methodologies/) 
```
make
make test
```
#### Question 2
git: https://github.com/bergeramit/Advanced-Operating-Systems-Homework.git (under assignment2/Understanding_of_process_memory_maps/) 
Answered in file Q2_solution.md

### Guy Berger
will submit its own

## The second part (together) is submitted using git

For testing: https://github.com/bergeramit/Advanced-Operating-Systems-Homework.git (under assignment2/mapspages_test/)
```
make test
# Running tests 1-9. Test 8 expects an input pattern or the letter 'd' to use default ('123413..x').
```

For linux changes and kernel module: https://github.com/bergeramit/Linux_for_Advance_OS_HW.git

# Assignment 3



## The second part (together) is submitted using git

I am using the https://github.com/purplewall1206/kvm-hello-world.git in the commit:
```
d1e765f21e2b8b242d3f26037740c1c98f0c14e0
```

### (a.1) What is the size of the guest (physical) memory? How and where in the code does the hypervisor allocate it? At what host (virtual) address is this memory mapped?

#### What is the size of the guest (physical) memory?
As we can see in kvm-hello-world.c line 480:
```
vm_init(&vm, 0x200000);
```
we allocate 0x200000 bytes for the guest physical memory.

#### How and where in the code does the hypervisor allocate it?
In kvm-hello-world.c in line 123 (in vm_init) is a call for KVM_SET_USER_MEMORY_REGION which sets the guests memory region. 
By calling the IOCTL with the memreg struct (special struct that specify the guests memory region), we can tell the KVM how and what memory to map. In this case we are telling the KVM to set the guest physical address to start at 0, the memory size to map is 0x200000 (which is the second argument in vm_init, AKA mem_size) and to map this address range to userspace_addr.

#### At what host (virtual) address is this memory mapped?
To answer this we can check the value of the field userspace_addr for the mapping of the guest physical address space in the user (host). We can see that the value lies inside vm->mem and in line 109 we can see:
```
vm->mem = mmap(NULL, mem_size, PROT_READ | PROT_WRITE,
		   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
```
This is a request to map a section (without a specific address in mind) in order to use it later as the virtual address range that will be allocated to the guest as the physical memory.
This means we cannot know for sure the exact address in this memory mapping since this can change each run.

### (a.2) Besides the guest memory, what additional memory is allocated? What is stored in that memory? Where in the code is this memory allocated? At what host/guest? (virtual/physical?) address is it located?

#### Besides the guest memory, what additional memory is allocated? 

Besides the guest memory, an additional memory is required for the virtual CPU (vcpu). We need to create a virtual CPU to run the guest code and so we call KVM_CREATE_VCPU (line 138 in vcpu_init function in kvm-hello-world.c):
``` 
vcpu->fd = ioctl(vm->fd, KVM_CREATE_VCPU, 0);
```
After we created this vcpu we need to know the memory size this sections needs in order to map it into the host (user) address space for later access. This means we need to get the vcpu memory size and map it like so (line 144-151 in vcpu_init function in kvm-hello-world.c):
```
vcpu_mmap_size = ioctl(vm->sys_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
...
vcpu->kvm_run = mmap(NULL, vcpu_mmap_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, vcpu->fd, 0);
```

#### What is stored in that memory?

This memory stores the state of an emulated CPU, including processor registers and other execution state. This will come in handy when we need to understand exit statuses from the guest VM.

#### Where in the code is this memory allocated? At what host/guest? (virtual/physical?) address is it located?

This code is allocated in KVM_CREATE_VCPU for the guest, and in the host's mapping in line 150 in function vcpu_init in kvm-hello-world.c:
```
vcpu->kvm_run = mmap(NULL, vcpu_mmap_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, vcpu->fd, 0);
```
Placed into the variable vcpu->kvm_run. The address of the mapping is again unknown because we used mmap with NULL as the first argument.

### (a.3) The guest memory area is setup to contain the guest code, the guest page table, and a stack. For each of these, identify where in the code it is setup, and the address range it occupies (both guest-physical and host-virtual)

#### The guest code (assume "long" mode)

resides in the variable guest64, which is placed in the payload.ld at the .payload64 region and holds guest64.img.o inside (between guest64 and guest64_end).
```
.payload64 0 : AT(LOADADDR(.payload32)+SIZEOF(.payload32)) {
        guest64 = .;
        guest64.img.o
        guest64_end = .;
}
```
The guest64.img.o is generated from the Makefile:
```
guest64.o: guest.c
	$(CC) $(CFLAGS) -m64 -ffreestanding -fno-pic -c -o $@ $^
...
guest64.img: guest64.o
	$(LD) -T guest.ld $^ -o $@
...
%.img.o: %.img
    $(LD) -b binary -r $^ -o $@
```
Which means guest64.img.o contains the code from guest.c

#### The guest page table (assume "long" mode)

The setup_long_mode function creates the page table (hirarchy) and then, using the struct kvm_segment sets these values with KVM_SET_SREGS:
```
ioctl(vcpu->fd, KVM_SET_SREGS, &sregs)
```
line 414 in run_long_mode in kvm-hello-world.c

Address of the page table:
```
pml4 -> vm->mem + 0x2000
pdpt -> vm->mem + 0x3000
pd -> vm->mem + 0x4000
```

#### The guest stack

Is set in the run_long_mode fucntion using the regs struct (struct kvm_regs) to set the regs.rsp - which is the guests stack pointer to be:
```
regs.rsp = 2 << 20;
```
Then we set it to the guest using:
```
ioctl(vcpu->fd, KVM_SET_REGS, &regs)
```
Guest stack address: 2 << 20

### (a.4) Examine the guest page table. How many levels does it have? How many pages does it occupy? Describe the guest virtual-to-physical mappings: what part(s) of the guest virtual address space is mapped, and to where?

#### How many levels does it have?


### For both (a.3) and (a.4), illustrate/visualize the hypervisor memory layout and the guest page table structure and address translation. (Preferably in text form).

