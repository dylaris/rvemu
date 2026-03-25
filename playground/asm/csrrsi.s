.option norvc
.option nopic

# @ident: name=csrrsi, opcode=0x73, funct3=0x6

# @expect: rd=1, zimm=1, csr=0x001
csrrsi x1, 0x001, 1

