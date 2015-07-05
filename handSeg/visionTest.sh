#!/usr/bin/expect
cd /Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg
spawn scp output_noiseTest1.mpeg output_noiseTest2.mpeg output_noiseTest3.mpeg output_noiseTest4.mpeg jw859@amdpool.ece.cornell.edu:./
expect "wujilongece2013!!!"
send "wujilongece2013!!!\r"
interact

spawn ssh jw859@amdpool.ece.cornell.edu  /home/student/jw859/CVProject/EvaluationTest
expect "wujilongece2013!!!"
send "wujilongece2013!!!\r"
interact


