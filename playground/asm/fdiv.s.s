.option norvc
.option nopic
# @ident: name=fdiv.s, opcode=0x53, funct7=0x0c
# @expect: rd=0, rs1=1, rs2=2
fdiv.s f0, f1, f2
