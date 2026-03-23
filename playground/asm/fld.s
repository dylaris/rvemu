.option norvc
.option nopic
# @ident: name=fld, opcode=0x07, funct3=0x3
# @expect: rd=1, rs1=2, imm=0
fld f1, 0(x2)
