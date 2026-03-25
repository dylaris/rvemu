.option rvc
.option nopic

# @ident: name=c.beqz, copcode=0x1, cfunct3=0x6
# @expect: rd=8, rs1=8, rs2=0, imm=2
c.beqz x8, 1f
1:

