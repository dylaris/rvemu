.option norvc
.option nopic
# @ident: name=fsw, opcode=0x27, funct3=0x2
# @expect: rs1=2, rs2=1, imm=0
fsw f1, 0(x2)
