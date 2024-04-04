#!/usr/bin/ruby

for i in 0..255 do
    print "VECTOR(0x#{i.to_s 16}); "
    if (i + 1) % 4 == 0 then
      print "\n"
    end
end

for i in 0..255 do
    print "VENTRY(0x#{i.to_s 16}); "
    if (i + 1) % 4 == 0 then
      print "\n"
    end
end
