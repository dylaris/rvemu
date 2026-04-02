#include "memory.h"
#include "elf64.h"

#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

static void mem__load_segment(Memory *mem, ProgHeader *phdr, int fd)
{
    u64       page_size     = getpagesize();
    u64       offset        = phdr->offset;
    HostVAddr vaddr         = mmu_to_host(phdr->vaddr);
    HostVAddr aligned_vaddr = ROUNDDOWN(vaddr, page_size);
    u64       mem_size      = phdr->memsz + (vaddr - aligned_vaddr);
    u64       file_size     = phdr->filesz + (vaddr - aligned_vaddr);

    int mmap_prot = 0;
    if (phdr->flags & PF_R)
        mmap_prot |= PROT_READ;
    if (phdr->flags & PF_W)
        mmap_prot |= PROT_WRITE;
    if (phdr->flags & PF_X)
        mmap_prot |= PROT_EXEC;

    HostVAddr mmap_vaddr = (HostVAddr) mmap(
        (void *) aligned_vaddr,
        mem_size, mmap_prot,
        MAP_FIXED | MAP_PRIVATE,
        fd, ROUNDDOWN(offset, page_size)
    );
    assert(mmap_vaddr == aligned_vaddr);

    u64 remaining_bss = ROUNDUP(mem_size, page_size) - ROUNDUP(file_size, page_size);
    if (remaining_bss > 0) {
        mmap_vaddr = (HostVAddr) mmap(
            (void *) (aligned_vaddr + ROUNDUP(file_size, page_size)),
            remaining_bss, mmap_prot,
            MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
        assert(mmap_vaddr == aligned_vaddr + ROUNDUP(file_size, page_size));
    }

    mem->host_end = MAX(mem->host_end, aligned_vaddr + ROUNDUP(mem_size, page_size));
    mem->heap_base = mem->heap_brk = mem->heap_end = mmu_to_guest(mem->host_end);
}

void mem_load_elf(Memory *mem, FILE *f)
{
    ELFHeader ehdr;

    // Read ELF header
    if (fread((void *) &ehdr, sizeof(ELFHeader), 1, f) != 1)
        fatal("file too small");

    // Check magic number
    if (*(u32 *) &ehdr != *(u32 *) ELFMAG)
        fatal("bad elf file");

    // Check architecture
    if (ehdr.machine != EM_RISCV || ehdr.ident[EI_CLASS] != ELFCLASS64)
        fatal("only support riscv64 elf file");

    // Set entry point
    mem->entry = (GuestVAddr) ehdr.entry;

    // Load segment
    for (u64 i = 0; i < (u64) ehdr.phnum; i++) {
        u64 offset = ehdr.phoff + i * sizeof(ProgHeader);
        if (fseek(f, (long) offset, SEEK_SET) != 0)
            fatalf("seek file failed when loading No.%ld program header", i);

        ProgHeader phdr;
        if (fread((void *) &phdr, sizeof(ProgHeader), 1, f) != 1)
            fatal("file tool small");

        if (phdr.type == PT_LOAD)
            mem__load_segment(mem, &phdr, fileno(f));
    }
}

void mem_load_bin(Memory *mem, FILE *f, GuestVAddr base)
{
    mem->entry = base;

    fseek(f, 0, SEEK_END);
    u64 file_size = ftell(f);
    rewind(f);

    u64 page_size = getpagesize();
    HostVAddr vaddr = mmu_to_host(base);
    HostVAddr aligned_vaddr = ROUNDDOWN(vaddr, page_size);
    u64 mem_size = file_size + (aligned_vaddr - vaddr);

    HostVAddr mmap_vaddr = (HostVAddr) mmap(
        (void *) aligned_vaddr,
        mem_size, PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_FIXED | MAP_PRIVATE,
        fileno(f), 0
    );
    assert(mmap_vaddr == aligned_vaddr);

    if (fread((void *) vaddr, 1, file_size, f) != file_size)
        fatal("failed to read binary file");

    printf("loaded binary at 0x%016lx, size 0x%zx\n", base, file_size);
}

GuestVAddr mem_alloc(Memory *mem, i64 size)
{
    u64 page_size = getpagesize();
    GuestVAddr alloc_base = mem->heap_brk;
    assert(alloc_base >= mem->heap_base);

    mem->heap_brk += size;

    if (size > 0) {
        if (mem->heap_brk > mem->heap_end) {
            if (mmap((void *) mem->host_end, ROUNDUP(size, page_size),
                     PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0) == MAP_FAILED)
                fatal("mmap failed");
            mem->host_end += ROUNDUP(size, page_size);
        }
    } else if (size < 0) {
        if (ROUNDUP(mem->heap_brk, page_size) < mem->heap_end) {
            u64 len = mem->heap_end - ROUNDUP(mem->heap_brk, page_size);
            if (munmap((void *) mem->host_end, len) == -1)
                fatal(strerror(errno));
            mem->host_end -= len;
        }
    }

    mem->heap_end = mmu_to_guest(mem->host_end);

    return alloc_base;
}
