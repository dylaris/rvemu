.option norvc
.option nopic

# @ident: name=csrrc, opcode=0x73, funct3=0x3

# @expect: rd=1, rs1=2, csr=0x001
csrrc x1, 0x001, x2

