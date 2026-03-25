.option norvc
.option nopic

# @ident: name=srliw, opcode=0x1b, funct3=0x5, funct7=0x00

# @expect: rd=1, rs1=2, shamt=1
srliw x1, x2, 1

# @expect: rd=3, rs1=4, shamt=15
srliw x3, x4, 15

