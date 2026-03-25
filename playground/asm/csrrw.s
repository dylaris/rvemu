.option norvc
.option nopic

# @ident: name=csrrw, opcode=0x73, funct3=0x1

# @expect: rd=1, rs1=2, csr=0x001
csrrw x1, 0x001, x2

