--[[
Construct training data of ImageNet for byteSteady
Copyright 2019 Xiang Zhang

Usage: th construct_train.lua [category_file] [data_directory1]
   [data_directory2] ... [output]
--]]

local io = require('io')
local paths = require('paths')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local category_file = arg[1] or 'data/imagenet-val-results/categories.txt'
   local data_directories = {'data/trainsize72quality75centercrop64'}
   local output = 'data/trainsize72quality75centercrop64.hex'
   if #arg > 2 then
      for i = 2, #arg - 1 do
         data_directories[i - 1] = arg[i]
      end
      output = arg[#arg]
   end

   joe.constructTrain(category_file, data_directories, output)
end

function joe.constructTrain(category_file, data_directories, output)
   local category_fd = io.open(category_file, 'r')
   local output_fd = io.open(output, 'w')
   local category_id = 0
   for line in category_fd:lines() do
      local category = line:gsub(" .*$", ""):gsub("^%s*(.-)%s*$", "%1")
      for _, data_directory in ipairs(data_directories) do
         local category_directory = paths.concat(data_directory, category)
         for file in paths.iterfiles(category_directory) do
            local cmd = 'hexdump -ve \'1/1 \"%.2x\"\' '..
               paths.concat(category_directory, file)
            print(cmd)
            local pfd = io.popen(cmd, 'r')
            -- Read the hex dump
            local hex = pfd:read('*a'):gsub('%s', '')
            output_fd:write(hex, ' ', category_id, '\n')
            pfd:close()
         end
      end
      category_id = category_id + 1
   end
   category_fd:close()
   output_fd:close()
end

joe.main()
return joe
