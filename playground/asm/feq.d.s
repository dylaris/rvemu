.option norvc
.option nopic
# @ident: name=feq.d, opcode=0x53, funct3=0x2, funct7=0x51
# @expect: rd=0, rs1=1, rs2=2
feq.d x0, f1, f2
