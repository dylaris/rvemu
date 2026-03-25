.option norvc
.option nopic

# @ident: name=addiw, opcode=0x1b, funct3=0x0

# @expect: rd=1, rs1=2, imm=1
addiw x1, x2, 1

# @expect: rd=3, rs1=4, imm=-1
addiw x3, x4, -1

# @expect: rd=5, rs1=6, imm=0x7ff
addiw x5, x6, 0x7ff

