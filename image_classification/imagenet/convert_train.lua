--[[
Construct training data from imagenet
Copyright 2019 Xiang Zhang

Usage: th convert_train.lua [category_file] [data_directory] [output_directory]
   [size] [quality] [threads]
--]]

local io = require('io')
local os = require('os')
local paths = require('paths')
local tunnel = require('tunnel')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local category_file = arg[1] or 'data/imagenet-val-results/categories.txt'
   local data_directory = arg[2] or 'data/raw/train'
   local output_directory = arg[3] or 'data/trainsize72quality75'
   local size = arg[4] or '72'
   local quality = arg[5] or '75'
   local threads = arg[6] and tonumber(arg[6]) or 8

   joe.constructTrain(category_file, data_directory, output_directory,
                      size, quality, threads)
end

function joe.constructTrain(
      category_file, data_directory, output_directory, size, quality, threads)
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

   local category_fd = io.open(category_file, 'r')
   local convert_options = '-colorspace RGB -depth 24'..
      ' -resize '..size..'x'..size..'\\^'..
      ' -colorspace sRGB '..' -quality '..quality..' -strip'..
      ' -define jpeg:fancy-upsampling=off'..' -define jpeg:optimize-coding=off'
   for line in category_fd:lines() do
      local category = line:gsub(" .*$", ""):gsub("^%s*(.-)%s*$", "%1")
      local category_directory = paths.concat(data_directory, category)
      local convert_directory = paths.concat(output_directory, category)
      os.execute('mkdir -p '..convert_directory)
      for file in paths.iterfiles(category_directory) do
         local cmd = 'convert '..paths.concat(category_directory, file)..
            ' '..convert_options..' '..
            paths.concat(convert_directory, file)
         vector:pushBack({callback = 'execute', parameters = {cmd}})
      end
   end
   category_fd:close()
   
   for i = 1, threads do
      vector:pushBack({callback = 'exit'})
   end
   block:synchronize()
end

joe.main()
return joe
