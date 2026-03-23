.option norvc
.option nopic

# @ident: name=auipc, opcode=0x17
# @expect: rd=1, imm=0x01000000
auipc x1, 0x1000
# @expect: rd=2, imm=0x08000000
auipc x2, 0x8000
# @expect: rd=3, imm=0x0
auipc x3, 0x0
