#include "mmu.h"
#include "elf64.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

static void mmu__load_segment(MMU *mmup, ProgHeader *phdrp, int fd)
{
    UNUSED(mmup);

    u64       pagesz = (u64) getpagesize();
    u64       offset = phdrp->offset;
    HostVAddr vaddr  = TO_HOST(phdrp->vaddr);
    HostVAddr aligned_vaddr = ROUNDDOWN(vaddr, pagesz);
    u64       memsz  = phdrp->memsz + (vaddr - aligned_vaddr);
    u64       filesz = phdrp->filesz + (vaddr - aligned_vaddr);

    int prot = 0; // mmap protection
    if (phdrp->flags & PF_R) prot |= PROT_READ;
    if (phdrp->flags & PF_W) prot |= PROT_WRITE;
    if (phdrp->flags & PF_X) prot |= PROT_EXEC;

    HostVAddr mmap_vaddr = (HostVAddr) mmap(
        (void *) aligned_vaddr,
        memsz, prot,
        MAP_FIXED | MAP_PRIVATE,
        fd, ROUNDDOWN(offset, pagesz));
    assert(mmap_vaddr == aligned_vaddr);

    // .bss section
    u64 remaining_bss = ROUNDUP(memsz, pagesz) - ROUNDUP(filesz, pagesz);
    if (remaining_bss > 0) {
        mmap_vaddr = (HostVAddr) mmap(
            (void *) (aligned_vaddr + ROUNDUP(filesz, pagesz)),
            remaining_bss, prot,
            MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
        assert(mmap_vaddr == aligned_vaddr + ROUNDUP(filesz, pagesz));
    }
}

void mmu_load_elf(MMU *mmup, FILE *fp)
{
    ELFHeader ehdr;

    // Read ELF header
    if (fread((void *) &ehdr, sizeof(ELFHeader), 1, fp) != 1)
        fatal("file too small");

    // Check magic number
    if (*(u32 *) &ehdr != *(u32 *) ELFMAG)
        fatal("bad elf file");

    // Check architecture
    if (ehdr.machine != EM_RISCV || ehdr.ident[EI_CLASS] != ELFCLASS64)
        fatal("only support riscv64 elf file");

    // Set entry point
    mmup->entry = (GuestVAddr) ehdr.entry;

    // Load segment
    for (u64 i = 0; i < (u64) ehdr.phnum; i++) {
        u64 offset = ehdr.phoff + i * sizeof(ProgHeader);
        if (fseek(fp, (long) offset, SEEK_SET) != 0)
            fatalf("seek file failed when loading No.%ld program header", i);

        ProgHeader phdr;
        if (fread((void *) &phdr, sizeof(ProgHeader), 1, fp) != 1)
            fatal("file tool small");

        if (phdr.type == PT_LOAD)
            mmu__load_segment(mmup, &phdr, fileno(fp));
    }
}
