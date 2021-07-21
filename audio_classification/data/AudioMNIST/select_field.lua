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

Select field from data

Usage: th select_field.lua [input] [begin] [end] [output]
--]]

local io = require('io')
local math = require('math')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local input = arg[1] or 'data/digitfreq1e65536split1train_shuffle.hex'
   local begin_index = arg[2] and tonumber(arg[2]) or 1
   local end_index = arg[3] and tonumber(arg[3]) or 11
   local output = arg[4] or 'data/digitfreq1e1024split1train_shuffle.hex'

   joe.selectField(input, begin_index, end_index, output)
end

function joe.selectField(input, begin_index, end_index, output)
   local ifd = io.open(input, 'r')
   local ofd = io.open(output, 'w')

   local n = 0
   for line in ifd:lines() do
      content = {}
      for item in line:gmatch('[%S]+') do
         content[#content + 1] = item
      end
      for i = begin_index, end_index do
         ofd:write(content[i], ' ')
      end
      ofd:write(content[#content], '\n')

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
