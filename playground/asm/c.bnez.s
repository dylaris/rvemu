.option rvc
.option nopic

# @ident: name=c.bnez, copcode=0x1, cfunct3=0x7
# @expect: rd=8, rs2=0, imm=2
c.bnez x8, 1f
1:

