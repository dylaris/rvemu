.option norvc
.option nopic

# @ident: name=amomaxu.d, opcode=0x2f, funct3=0x3, funct5=0x1c

# @expect: rd=1, rs1=2, rs2=3
amomaxu.d x1, x3, (x2)

