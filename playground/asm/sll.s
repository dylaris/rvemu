.option norvc
.option nopic

# @ident: name=sll, opcode=0b0110011, funct3=0x1, funct7=0x00

# @expect: rd=1, rs1=2, rs2=3
sll x1, x2, x3

# @expect: rd=4, rs1=5, rs2=6
sll x4, x5, x6

# @expect: rd=7, rs1=8, rs2=9
sll x7, x8, x9
