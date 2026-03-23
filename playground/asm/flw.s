.option norvc
.option nopic
# @ident: name=flw, opcode=0x07, funct3=0x2
# @expect: rd=1, rs1=2, imm=0
flw f1, 0(x2)
