.option norvc
.option nopic

# @ident: name=xor, opcode=0b0110011, funct3=0x4, funct7=0x00

# @expect: rd=1, rs1=2, rs2=3
xor x1, x2, x3

# @expect: rd=4, rs1=5, rs2=6
xor x4, x5, x6

# @expect: rd=7, rs1=8, rs2=9
xor x7, x8, x9
