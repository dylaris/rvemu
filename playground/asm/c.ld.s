.option rvc
.option nopic

# @ident: name=c.ld, copcode=0x0, cfunct3=0x3
# @expect: rd=8, rs1=9, imm=0
c.ld x8, 0(x9)

