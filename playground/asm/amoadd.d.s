.option norvc
.option nopic

# @ident: name=amoadd.d, opcode=0x2f, funct3=0x3, funct5=0x00

# @expect: rd=1, rs1=2, rs2=3
amoadd.d x1, x3, (x2)

