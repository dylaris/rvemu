.option norvc
.option nopic

# @ident: name=amoswap.w, opcode=0x2f, funct3=0x2, funct5high=0x01
# @expect: rd=1, rs1=2, rs2=3
amoswap.w x1, x3, (x2)
