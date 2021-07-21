--[[
Convert directory of data to JPEG2000 (jpe2)
Copyright 2019 Xiang Zhang

Usage: th convert_train.lua [data_directory] [output_directory] [size] [filter]
   [level] [strategy] [threads]
--]]

local io = require('io')
local os = require('os')
local paths = require('paths')
local tunnel = require('tunnel')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local data_directory = arg[1] or 'data/train'
   local output_directory = arg[2] or
      'data/trainsize32filter5level0strategy0png'
   local size = arg[3] or '32'
   local filter = arg[4] or '5'
   local level = arg[5] or '0'
   local strategy = arg[6] or '0'
   local threads = arg[7] and tonumber(arg[7]) or 8

   joe.constructTrain(data_directory, output_directory, size, filter, level,
                      strategy, threads)
end

function joe.constructTrain(
      data_directory, output_directory, size, filter, level, strategy, threads)
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
   local vector = tunnel.Vector(threads * 2)
   local printer = tunnel.Printer()
   local block = tunnel.Block(threads)
   block:add(vector, printer)
   local run_id = block:run(threadExecute)

   local convert_options = '-colorspace RGB -depth 24'..
      ' -resize '..size..'x'..size..'\\^'..
      ' -colorspace sRGB -strip'..
      ' -define png:compression-filter='..filter..
      ' -define png:compression-level='..level..
      ' -define png:compression-strategy='..strategy
   local category = 0
   while paths.dirp(paths.concat(data_directory, tostring(category))) do
      local category_directory = paths.concat(
         data_directory, tostring(category))
      local convert_directory = paths.concat(
         output_directory, tostring(category))
      os.execute('mkdir -p '..convert_directory)
      for file in paths.iterfiles(category_directory) do
         local cmd = 'convert '..paths.concat(category_directory, file)..
            ' '..convert_options..' '..paths.concat(
               convert_directory, paths.basename(file, '.jpg')..'.png')
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
