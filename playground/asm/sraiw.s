.option norvc
.option nopic

# @ident: name=sraiw, opcode=0x1b, funct3=0x5, funct7=0x20

# @expect: rd=1, rs1=2, shamt=1
sraiw x1, x2, 1

# @expect: rd=3, rs1=4, shamt=15
sraiw x3, x4, 15

