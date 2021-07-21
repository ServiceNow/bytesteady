--[[
Concatenate field from data
Copyright 2020 Xiang Zhang

Usage: th concat_field.lua [input] [output]
--]]

-- A Logic Named Joe
local joe = {}

function joe.main()
   local input = arg[1] or 'data/digitfreq1e65536split1train_shuffle.hex'
   local output = arg[2] or
      'data/digitfreq1e65536split1train_shuffle_concat.hex'
   joe.concatField(input, output)
end

function joe.concatField(input, output)
   local ifd = io.open(input, 'r')
   local ofd = io.open(output, 'w')

   local n = 0
   for line in ifd:lines() do
      content = {}
      for item in line:gmatch('[%S]+') do
         content[#content + 1] = item
      end
      for i = 1, #content - 1 do
         ofd:write(content[i])
      end
      ofd:write(' ', content[#content], '\n')

      n = n + 1
      if math.fmod(n, 1000) == 0 then
         io.write('\rProcessing line: ', n)
         io.flush()
      end
   end
   print('\rProcessed lines: '..n)

   ifd:close()
   ofd:close()
end


joe.main()
return joe

