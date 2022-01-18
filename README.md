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

## The First part - individualy

### Amit Berger
I placed my answers in the repo under:
```
Advanced-Operating-Systems-Homework/assignment3/individual_problems.md
```

### Guy Berger
will submit its own

## The second part (together) is submitted using git

I am using the https://github.com/purplewall1206/kvm-hello-world.git in the commit, for the questions:
```
d1e765f21e2b8b242d3f26037740c1c98f0c14e0
```
For the programming assignment I have forked the kvm-hello-world into my repo:
```
https://github.com/bergeramit/kvm-hello-world.git
```
I added two tags:
1. for the assignment without 2 vCPU (sections (e)):
```
assignment3_without_e_with_temp_file
```
2. for the (e) assignment - **including the bonus**
```
assignment_3_e_bonus
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

#### The guest stack (assume "long" mode)

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

#### How many levels does it have? (assume "long" mode)

It has 4 levels. We can see that by looking at the setup_long_mode function:
```
uint64_t pml4_addr = 0x2000;
uint64_t *pml4 = (void *)(vm->mem + pml4_addr);

uint64_t pdpt_addr = 0x3000;
uint64_t *pdpt = (void *)(vm->mem + pdpt_addr);

uint64_t pd_addr = 0x4000;
uint64_t *pd = (void *)(vm->mem + pd_addr);
```
We got the top (and single) pml4 (Page Map Level 4) after that we can access the pdpt (Page Directory Pointer Table) followed by the pd (page directory) to access a single specific page.


### For both (a.3) and (a.4), illustrate/visualize the hypervisor memory layout and the guest page table structure and address translation. (Preferably in text form).

For the guest address translation:

We first access the pml4 table located at address: 
```
vm->mem + 0x2000
```
this has only one non-empty entry (the first entry) - pml4[0]
This includes bits 39-47


We then follow through to the index in the pdpt at address:
```
vm->mem + 0x3000
```
Only non-empty is again the first entry which directs to the pd table - pdpt[0]
This includes the followed 9 bits

We the follow to the pd table at address:
```
vm->mem + 0x4000
```
Again to the first and only non-empty entry that contains the address 0 - pd[0]

Because of the PAE the zero address is the guest physical address - 0 -> which we mapped with the start of the code!

This guest physical will translate into the host virtual address:
```
vm->mem
```

### (a.5) At what (guest virtual) address does the guest start execution? Where is this address configured?

This was answered in the previous question - we configure the start of the execution at the guest **virtual address 0** inside run_long_mode
```
regs.rip = 0;
```
Then we access the 4 level page table hirarchy just to end up in the **guest physical address 0** which is vm->mem (from previous question). There lies the begining of the guest.c binary code, we can see that in run_long_mode:
```
memcpy(vm->mem, guest64, guest64_end-guest64);
```

### (a.6) What port number does the guest use? How can the hypervisor know the port number, and read the value written? Which memory buffer is used for this value? How many exits occur during this print?

#### What port number does the guest use?
The guest used the 0xE9 port number - as it is specified in line 15, in guest.c:
```
outb(0xE9, *p);
```
Which is later used in the command outb where the 0xE9 is the port number argument.

#### How can the hypervisor know the port number, and read the value written?

When the host tries to access the IO port, a functionality that needs to be supplied by the user (host), there is a call to vmexit with the apropriate values in vcpu->kvm_run.
Then the host (user) can look to see whether the io port was 0xE9 by accessing the value in:
```
vcpu->kvm_run->io.port
``` 
After that it can access the data_offset to get the actual value that was send to that IO port by calculating the address:
```
vcpu->kvm_run + vcpu->kvm_run->io.data_offset
```

This results in the data that the guest tried to send to that port.

#### Which memory buffer is used for this value?

The 

#### How many exits occur during this print?

We can see in the guest that every charecter is printed separately and so we will exit == the length of the string "Hello, world!\n" (14)

### (a.7) At what guest virtual (and physical?) address is the number 42 written? And how (and where) does the hypervisor read it?

The guest wrote the address to the virtual address 0x400,
which is the guest physical address vm->mem + 0x400 which is also the host virtual.

The hypervisor reads this by accessing:
```
vm->mem[0x400]
```
inside the function run_vm under the check label, in line 204.


## (2) Containers and namespaces


### (a) Describe the process hierarchy produced by the sequence of commands in the "child shell" column. How can it be minimized, and what would the hierarchy look like?

The sequence commands in the child shell:
1. unshare -U --kill-child /bin/bash -> creates a new user namespace and running bash inside this container
2. echo "my-user-ns" > /proc/$$/comm -> naming this process "my-user-ns". Because we used the --kill-child we will fork the newly created ns and run bash in the child. Also, when we terminate this ns the child to be killed
3. id -> view the new user namespace
4. unshare --ipc --uts --kill-child /bin/bash -> creates ipc and uts namespaces and running bash inside this new container - with the user namespace already configured. Because we used the --kill-child we will fork the newly created ns and run bash in the child. Also, when we terminate this ns the child to be killed
5. hostname isolated -> only changes the hostname of the current uts namespace
6. unshare --net --kill-child /bin/bash ->  creates a new network namespace and running bash inside this container (with all previous ns already configured). Because we used the --kill-child we will fork the newly created ns and run bash in the child. Also, when we terminate this ns the child to be killed
7. echo "my-net-ns" > /proc/$$/comm -> naming this process "my-net-ns"
8. ip link set lo up && ip link set peer0 up && ip addr add 10.11.12.14/24 dev peer0 -> set up virtual interface peer0
9. unshare --pid --mount --fork --kill-child /bin/sh -> creates a new pid and mount namespaces and running sh inside this container (with all previous ns already configured). Because we used the --kill-child we will fork the newly created ns and run bash in the child. Also, when we terminate this ns the child to be killed

### (b) What would happen if you change the order of namespace creation, e.g. run unshare --ipc first? And what would happen if you defer lines 12-13 until a later time?

If we had done the --ipc first we will not be able to set the child as a root user because when we ran:
```
sudo bash -c 'echo "0 1000 1000" > /proc/5918/uid_map'
sudo bash -c 'echo "0 1000 1000" > /proc/5918/gid_map'
```
to set the uid and gid of the child to root, **we are sending IPC** to the child's process which means, we have to be in the same ipc ns to receive these changes.
This means that running lines 12-13 later will have no effect on the child's process

### (c) What is the purpose of line 4 and lines 9-10 (and similarly, line 27 and lines 29-30)? Why are they needed?

These lines make it so we would know for sure which bash is the target child's process. By changing the name of the process we can identify it from the others and can manage it from the parent shell.

### (d) Describe how to undo and cleanup the commands above. (Note: there is more than one way; try to find the minimal way). Make sure there are no resources left dangling around.

= = = = = = = = = = = = =

### (e) Test your program. Does it require root privileges? If so, then why? How can it be changed to not require these privileges?


