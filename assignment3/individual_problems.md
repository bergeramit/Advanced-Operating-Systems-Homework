## (1) Explain, in your own words, the mechanisms depicted in the diagrams in slides 26 and 27 of Lecture #9

First we want to configure the guest VM with ioctl:
1. Calls ioctl “set memory” etc to configure the VM
2. Calling ioctl “run” to run the VM. - the KVM driver is called and issues VMLAUNCH to start executing the guest OS
In this process we are telling the KVM that we want it to use our virtual memory as physical memory for the guest os.

When guest VM exit (traps occurs – decide how to act and if we cannot handle this request in the kernel (for example this is an IO request for the user mode (QEMU)) than we return from the ioctl “run” with the indicative information to the QEMU in order to retrieve the IO information - and place it in the guest OS memroy. Then the the user mode will call ioctl “continue” again to resume the guest OS. This machenism is also respondible for checking for signals send from the user like "ctrl+c" and handles this as well in the user mode.

In the 27th slide there is a general purpose flowchart that describes the same mechanism of the KVM - we begin by exiting the guest os with vmexit and, depends on the exit condition (IO, page faults, etc) we call the dedicated handlers.
Some handlers include: division by 0, page fault.
If the KVM do not know how to handle this exit, we need to understand what the guest OS tried to do and handover this information to the user mode to handle the exit:
we fetch the instruction, decode it, privilege check and then trying to execute the command read and write memory - will happen in the qemu and writing results to the guest OS's memory. After we executed this we need to update the guest's registers and rip.

## (2) When the OS runs out of memory it resorts to swapping pages to storage. Consider a system with QEMU/KVM hypervisor running several guests

### (a) What happens if the guest runs out of memory and starts swapping? How does it affect the host? Describe in (low-level) detail what happens when a guest-userspace process tries to access a guest-swapped page)

When the guest kernel decides it needs to swap pages for the guest-userspace, it will send a page to the swap and place in the page table entry "not-present - swapped out". This will cause a pagefault in the guest-userspace when the guest-user tries to access this page. The guest-kernel will then fetch this page from the swap and place it in the entry and return. Just like when we handle swap in the kernel for the guest-OS physical pages we did not need the QEMU, here we do not need to bother the KVM with these memory operations.

#### Effects of Guest
Will have a page fault and corrent it inside the guest-kernel.

#### Effects of Host
Will not be bothered.

### (b) What happens when the host runs out of the memory, and swaps out pages that are used by the hypervisor? How does it affect the guest? Describe in (low-level) details what happens when a guest-userspace process tries to access a host-swapped page

When the kernel decide we need to send page to the swap, it will do that and place in the page table entry - "not present - swapped out".
The guest OS continues its execution and at some time in the future, it will try to access this page - this 
will cause a page fault -> the KVM will look at the systems, know it is not an EPT violation, the page is not present (at the KVM level, not at the guest level) - the KVM sees that the page is swapped out, and will retrieve this page and return to the Guest OS with the page in memroy.

#### Effects of Guest
The guest did not expirience anything but a delay in retrieving the page (instead of micro second in took 10ms).

#### Effects of Host
The host will not notice this happending (the not-present hanlde) since this entire handling will be done in the kernel and KVM.

## (3) One difference between plain virtualization and nested virtualization is that the former can leverage EPT/NPT hardware extensions, while the latter cannot do so directly.

### (a) What are the pros and cons of adding another EPT/NPT layer for nested virtualization?

#### Pros
- Reduce the amount of VMEXITS and context switches

#### Cons
- The amount of refrences we need in order to access memory upon TLB miss is very large:
    - N - depth of host page tables
    - M - depth of guest page tables
    - References = (N x M) + N + M
- Implementation needs many registers and HW support to notify VMM

### (b) In the Turtles Project, nested virtalization uses "EPT compression" to optimize memory virtualization. To some extent, this technique reminds of the optimization used in the L3 µ-kernel to reduce TLB flushed. Explain the common theme to both. In a hindsight, could the L3 µ-kernel benefit from today's EPT? If so, how?

Both tries to compress the translation of pages in order to improve performance and memory usage
- In the L3 µ-kernel we use segments and taggs in order to avoid the TLB flushes and still be able to access relevant pages.
    - translation from tag/ segment + offset in TLB -> page instead of switching TLBs and address spaces
- In the Turtles project, we reduce 3 logical levels into two HW tables in order to avoid excess accesses and refrences.
    - translation from L1-v + l2-p -> L0-p instead of L2-v + L2-p + L1 + L0

L3 µ-kernel can benefit from today's EPT in that it can create "Guest VM" per process or protected domain. This can help in handling large protection domains (processes) in which we cannot use the tagging or segment methods to optimize memory operations. By using guest VMs we can hold multi-dimensional paging scheme to translate different guests (protected domains) and use smart translations to handle different accesses.

### (4) The Antfarm project relied on shadow page-tables for introspection, to learn about processes inside the guest. It was built before the introduction of EPT/NPT. How would the use of EPT affect the techniques describes in the paper? What changes (if any) would you propose to adapt their system to EPT, and how would such changes affect its performance?

The Antfarm solution focuses on monitoring the processes of the Guest by monitoring:
- Process Creation - See value in CR3 where **we did not** see before
- Context Switch - we will **see a change** in CR3 to a new value that we already know
- Process Exit - If we see:
    - In CR3 a clean user space page table
    - We had invalidate for the page table
    - We can say there was a process termination
        - If there wasn't a reuse we can estimate the process terminated if we did not see the page table access for a long time

The use of EPT/NPT solution in order to track process creationg, context switches and process exit will effect the solution in the following ways:
1. The "master TLB" (Extended caches) that manages both the gVA->gPA and the gPA->hPA. Because of this approach there will not by any VM-exits due to guest page faults, INVLPG or CR3 changes - which poses a threat to the Antfarm monitoring solution. So in order to track Process Creation and Context Switch we cannot rely on VM-exits anymore and would need to monitor the TLB misses instead:
    <!-- 1. Process Creation - TLB miss with a new gVA entries (with no previous mapping)
    2. Context Switch - TLB miss with an already present gVA (but not in the TLB)
    3. Process Exit - TLB miss with a  -->
2. Downside to this approach is that we will need to handle a lot of work when we have a TLB miss (as we discussed in class)