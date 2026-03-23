.option norvc
.option nopic
# @ident: name=flt.s, opcode=0x53, funct3=0x1, funct7=0x50
# @expect: rd=0, rs1=1, rs2=2
flt.s x0, f1, f2
