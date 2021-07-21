--[[
Copyright 2021 ServiceNow
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Create gender classification dataset

Usage: th create_gender.lua [m] [set_1] .. [set_m] [n] [dir_1] .. [dir_n]
   [output]
--]]

local io = require('io')
local math = require('math')

-- A Logic Named Joe
local joe = {}

joe.GENDER = {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,
              0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1}

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

   joe.createGender(sets, dirs, output)
end

function joe.createGender(sets, dirs, output)
   local ofd = io.open(output, 'w')
   local n = 0;
   for _, set in ipairs(sets) do
      local gender = joe.GENDER[tonumber(set)]
      local file_fd = io.popen('ls '..dirs[1]..'/'..set..'/*.raw')
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
         ofd:write(gender, '\n')
      end
      file_fd:close()
   end
   io.write('\n')
   ofd:close()
end

joe.main()
return joe
