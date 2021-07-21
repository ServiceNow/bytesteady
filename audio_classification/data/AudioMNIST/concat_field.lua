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

Concatenate field from data

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

