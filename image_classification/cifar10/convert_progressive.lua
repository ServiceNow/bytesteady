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

Convert directory of data to jpeg

Usage: th convert_progressive.lua [data_directory] [output_directory] [size]
   [quality] [threads]
--]]

local io = require('io')
local os = require('os')
local paths = require('paths')
local tunnel = require('tunnel')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local data_directory = arg[1] or 'data/train'
   local output_directory = arg[2] or 'data/trainsize32quality75progressive'
   local size = arg[3] or '32'
   local quality = arg[4] or '75'
   local threads = arg[5] and tonumber(arg[5]) or 8

   joe.constructTrain(data_directory, output_directory, size, quality, threads)
end

function joe.constructTrain(
      data_directory, output_directory, size, quality, threads)
   -- Tunnel RPC executer
   local function threadExecute(vector, printer)
      local os = require('os')
      local rpc = vector:popFront()
      while rpc.callback == 'execute' do
         printer(__threadid, rpc.callback, rpc.parameters[1])
         os.execute(rpc.parameters[1])
         rpc = vector:popFront()
      end
      printer(__threadid, rpc.callback)
   end
   local vector = tunnel.Vector(threads*2)
   local printer = tunnel.Printer()
   local block = tunnel.Block(threads)
   block:add(vector, printer)
   local run_id = block:run(threadExecute)

   local convert_options = '-colorspace RGB -depth 24'..
      ' -resize '..size..'x'..size..'\\^'..
      ' -colorspace sRGB '..' -quality '..quality..' -strip'..
      ' -define jpeg:fancy-upsampling=off'..
      ' -define jpeg:optimize-coding=off'..
      ' -interlace plane'
   local category = 0
   while paths.dirp(paths.concat(data_directory, tostring(category))) do
      local category_directory = paths.concat(
         data_directory, tostring(category))
      local convert_directory = paths.concat(
         output_directory, tostring(category))
      os.execute('mkdir -p '..convert_directory)
      for file in paths.iterfiles(category_directory) do
         local cmd = 'convert '..paths.concat(category_directory, file)..
            ' '..convert_options..' '..paths.concat(convert_directory, file)
         vector:pushBack({callback = 'execute', parameters = {cmd}})
      end
      category = category + 1
   end
   
   for i = 1, threads do
      vector:pushBack({callback = 'exit'})
   end
   block:synchronize()
end

joe.main()
return joe
