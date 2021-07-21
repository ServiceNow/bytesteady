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

Convert data to jpeg

th construct_jpeg.lua [input] [output_directory]
--]]

local ffi = require('ffi')
local image = require('image')
local io = require('io')
local paths = require('paths')
local os = require('os')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local input = arg[1] or 'data/cifar-10-batches-bin/data_batch_1.bin'
   local output_directory = arg[2] or 'data/train'
   joe.constructJPEG(input, output_directory)
end

function joe.constructJPEG(input, output_directory)
   local ifd = io.open(input, 'rb')
   local label = ifd:read(1)
   local n = 0
   while label ~= nil do
      n = n + 1
      local image_string = ifd:read(3072)
      local image_data = torch.ByteTensor(3, 32, 32)
      ffi.copy(torch.data(image_data), image_string, 3072)
      local image_directory = paths.concat(
         output_directory, tostring(string.byte(label)))
      os.execute('mkdir -p '..image_directory)
      local image_file = paths.concat(
         image_directory, paths.basename(input, 'bin')..'_'..n..'.jpg')
      image.save(image_file, image_data)
      label = ifd:read(1)
   end
   print('Processed images: '..n)
   ifd:close()
end

joe.main()
return joe
