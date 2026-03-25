.option norvc
.option nopic

# @ident: name=csrrci, opcode=0x73, funct3=0x7

# @expect: rd=1, zimm=1, csr=0x001
csrrci x1, 0x001, 1
