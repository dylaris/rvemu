.option norvc
.option nopic

# @ident: name=sub, opcode=0b0110011, funct3=0x0, funct7=0x20

# @expect: rd=1, rs1=2, rs2=3
sub x1, x2, x3

# @expect: rd=4, rs1=5, rs2=6
sub x4, x5, x6

# @expect: rd=7, rs1=8, rs2=9
sub x7, x8, x9
