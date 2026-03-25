.option norvc
.option nopic

# @ident: name=addw, opcode=0x3b, funct3=0x0, funct7=0x00

# @expect: rd=1, rs1=2, rs2=3
addw x1, x2, x3

# @expect: rd=4, rs1=5, rs2=6
addw x4, x5, x6

