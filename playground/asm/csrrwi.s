.option norvc
.option nopic

# @ident: name=csrrwi, opcode=0x73, funct3=0x5

# @expect: rd=1, zimm=1, csr=0x001
csrrwi x1, 0x001, 1

