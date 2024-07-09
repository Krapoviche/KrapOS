#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "message.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"
#include "start.h"
#include "kbd.h"
#include "pci.h"
#include "primitive.h"
#include "virtio.h"
#include "intel.h"
#include "acpi.h"

void idle(void){
	while(1){
		sti();
		hlt();
		cli();
	}
}

void kernel_start(void)
{
    reset_screen();
    place_cursor(0, 0);

    init_clock();
    init_IT_handlers(32, IT_32_handler);
    init_IT_handlers(33, IT_33_handler);
    init_IT_handlers(49, IT_49_handler);
    mask_IRQ(0, false);
    mask_IRQ(1, false);
    process_table = init_process_table();
    init_keyboard_buffer();

    bios32* bios32_info = find_bios32_service_dir_header();

    uint32_t* pcibios_entrypoint = read_bios32_service(PCI_SERVICE, bios32_info);
    bios_service* pci_service = mem_alloc(sizeof(bios_service));

    read_pcibios(pcibios_entrypoint, pci_service);

    printf("PCI BIOS VERSION : %d.%d\n",pci_service->major_revision, pci_service->minor_revision);

    RSDP_t* rsdp = discover_rsdp();
    printf("Revision of rsdp is : 0x%x\n", rsdp->Revision);


    // Access the RSDT
    ACPISDTHeader_t* rsdt = rsdp->RsdtAddress;
    printf("RSDT is: %p\n", rsdt);
    printf("Signature of RSDT is: %.4s\n", rsdt->Signature);
    printf("Length of RSDT is: %u\n", rsdt->Length);

    // discover_pci_devices();

    // lspci();

    // pci_device* virtio_gpu = get_device_info(0,0,0);

    // enable_master_device(virtio_gpu);
    // print_device_info(virtio_gpu);
    // test_somth(virtio_gpu);
    // retrieve_intel_pci_max_link_speed(intel_gigabit_ethernet, get_intel_pci_capability(intel_gigabit_ethernet));

    start((void*)idle, 256, INT32_MIN, "p_idle", 0);

	process_table->running = queue_out(process_table->runnable_queue, process_t, queue_link);
	process_table->running->state = RUNNING;

	void* user_start = (void*)0x1000000;

	start(user_start, 4096, 1, "user_start", 0);

	idle();
	return;
}
