.option norvc
.option nopic

# @ident: name=amoswap.d, opcode=0x2f, funct3=0x3, funct5=0x01

# @expect: rd=1, rs1=2, rs2=3
amoswap.d x1, x3, (x2)

