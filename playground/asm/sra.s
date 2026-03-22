.option norvc
.option nopic

# @ident: name=sra, opcode=0b0110011, funct3=0x5, funct7=0x20

# @expect: rd=1, rs1=2, rs2=3
sra x1, x2, x3

# @expect: rd=4, rs1=5, rs2=6
sra x4, x5, x6

# @expect: rd=7, rs1=8, rs2=9
sra x7, x8, x9
