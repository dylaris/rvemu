.option norvc
.option nopic

# @ident: name=csrrs, opcode=0x73, funct3=0x2

# @expect: rd=1, rs1=2, csr=0x001
csrrs x1, 0x001, x2

