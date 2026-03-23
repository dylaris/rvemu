.option norvc
.option nopic
# @ident: name=fsd, opcode=0x27, funct3=0x3
# @expect: rs1=2, rs2=1, imm=0
fsd f1, 0(x2)
