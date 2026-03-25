.option rvc
.option nopic

# @ident: name=c.addi16sp, copcode=0x8, cfunct3=0x3, cfunct5high=0x9
# @expect: rd=2, rs1=2, imm=16
c.addi16sp x9, 16

