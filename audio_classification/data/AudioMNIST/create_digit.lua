--[[
Create digit classification dataset
Copyright 2020 Xiang Zhang

Usage: th create_digit.lua [m] [set_1] .. [set_m] [n] [dir_1] .. [dir_n]
   [output]
--]]

local io = require('io')
local math = require('math')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local set_count = tonumber(arg[1])
   local sets = {}
   for i = 1, set_count do
      sets[i] = arg[i + 1]
   end
   local dir_count = tonumber(arg[set_count + 2])
   local dirs = {}
   for i = 1, dir_count do
      dirs[i] = arg[i + set_count + 2]
   end
   local output = arg[set_count + dir_count + 3]

   joe.createDigit(sets, dirs, output)
end

function joe.createDigit(sets, dirs, output)
   local ofd = io.open(output, 'w')
   local n = 0;
   for digit = 0, 9 do
      for _, set in ipairs(sets) do
         local file_fd = io.popen(
            'ls '..dirs[1]..'/'..set..'/'..digit..'_*.raw')
         for file in file_fd:lines() do
            local filename = file:match('[^/]+$')
            for _, dir in ipairs(dirs) do
               local ifd = io.popen(
                  'hexdump -ve \"1/1 \\\"%.2x\\\"\" '..dir..'/'..set..'/'..
                     filename)
               local hex_text = ifd:read('*all')
               ifd:close()
               ofd:write(hex_text, '00 ')

               n = n + 1
               if math.fmod(n, 1000) == 0 then
                  io.write('\rProcessing file ', dir, '/', set, '/', filename)
                  io.flush()
               end
            end
            ofd:write(digit, '\n')
         end
         file_fd:close()
      end
   end
   io.write('\n')
   ofd:close()
end

joe.main()
return joe
