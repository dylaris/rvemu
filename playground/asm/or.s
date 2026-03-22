.option norvc
.option nopic

# @ident: name=or, opcode=0b0110011, funct3=0x6, funct7=0x00

# @expect: rd=1, rs1=2, rs2=3
or x1, x2, x3

# @expect: rd=4, rs1=5, rs2=6
or x4, x5, x6

# @expect: rd=7, rs1=8, rs2=9
or x7, x8, x9
