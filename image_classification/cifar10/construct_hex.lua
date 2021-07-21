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

Construct training data of ImageNet for byteSteady

Usage: th construct_train.lua [input_directory] [output]
--]]

local io = require('io')
local paths = require('paths')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local input_directory = arg[1] or 'data/trainsize32quality75'
   local output = arg[2] or 'data/trainsize32quality75.hex'
   joe.constructTrain(input_directory, output)
end

function joe.constructTrain(input_directory, output)
   local output_fd = io.open(output, 'w')
   local category = 0
   while paths.dirp(paths.concat(input_directory, tostring(category))) do
      local category_directory = paths.concat(
         input_directory, tostring(category))
      for file in paths.iterfiles(category_directory) do
         local cmd = 'hexdump -ve \'1/1 \"%.2x\"\' '..
            paths.concat(category_directory, file)
         print(cmd)
         local pfd = io.popen(cmd, 'r')
         -- Read the hex dump
         local hex = pfd:read('*a'):gsub('%s', '')
         output_fd:write(hex, ' ', category, '\n')
         pfd:close()
      end
      category = category + 1
   end
   output_fd:close()
end

joe.main()
return joe
