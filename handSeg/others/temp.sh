#!/usr/bin/expect
spawn ssh xh243@amdpool.ece.cornell.edu /home/student/xh243/Project/server.sh
expect "IXrG2QHh0YNX"
send "IXrG2QHh0YNX\r"
interact


cd /Users/BboyKellen/Documents/Xcode/headSeg/handSeg
spawn scp noiseTest1.mpeg jw859@amdpool.ece.cornell.edu:./
expect "wujilongece2013!!!"
send "wujilongece2013!!!\r"
interact