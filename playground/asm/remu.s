.option norvc
.option nopic

# @ident: name=remu, opcode=0x33, funct3=0x7, funct7=0x01
# @expect: rd=1, rs1=2, rs2=3
remu x1, x2, x3
# @expect: rd=5, rs1=6, rs2=7
remu x5, x6, x7
