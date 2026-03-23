.option norvc
.option nopic
# @ident: name=feq.s, opcode=0x53, funct3=0x2, funct7=0x50
# @expect: rd=0, rs1=1, rs2=2
feq.s x0, f1, f2
