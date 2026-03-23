.option norvc
.option nopic
# @ident: name=flt.d, opcode=0x53, funct3=0x1, funct7=0x51
# @expect: rd=0, rs1=1, rs2=2
flt.d x0, f1, f2
