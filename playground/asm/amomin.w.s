.option norvc
.option nopic

# @ident: name=amomin.w, opcode=0x2f, funct3=0x2, funct5high=0x10
# @expect: rd=1, rs1=2, rs2=3
amomin.w x1, x3, (x2)
