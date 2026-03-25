.option norvc
.option nopic

# @ident: name=amoadd.w, opcode=0x2f, funct3=0x2, funct5high=0x00
# @expect: rd=1, rs1=2, rs2=3
amoadd.w x1, x3, (x2)
