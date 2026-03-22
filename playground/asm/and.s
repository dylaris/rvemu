.option norvc
.option nopic

# @ident: name=and, opcode=0b0110011, funct3=0x7, funct7=0x00

# @expect: rd=1, rs1=2, rs2=3
and x1, x2, x3

# @expect: rd=4, rs1=5, rs2=6
and x4, x5, x6

# @expect: rd=7, rs1=8, rs2=9
and x7, x8, x9
