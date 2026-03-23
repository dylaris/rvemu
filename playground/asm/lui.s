.option norvc
.option nopic

# @ident: name=lui, opcode=0x37
# @expect: rd=1, imm=0x01000000
lui x1, 0x1000
# @expect: rd=2, imm=0x08000000
lui x2, 0x8000
# @expect: rd=3, imm=0x0
lui x3, 0x0
