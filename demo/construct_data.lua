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

Read the IMDB dataset in CSV format and produce datasets

Usage: th construct_data.lua [input] [train_file] [test_file] [unsup_file]
--]]

local io = require('io')
local math = require('math')

-- A Logic Named Joe
local joe = {}

joe.CLASS = {['neg'] = 0, ['pos'] = 1, ['unsup'] = 2}

function joe.main()
   local input = arg[1] or 'data/imdb_master.csv'
   local train = arg[2] or 'data/train.hex'
   local test = arg[3] or 'data/test.hex'
   local unsup = arg[4] or 'data/unsup.hex'

   print('Process data from '..input)
   joe.constructData(input, train, test, unsup)
end

function joe.constructData(input, train, test, unsup)
   local n = 0;
   local ifd = io.open(input, 'r')
   local train_fd = io.open(train, 'w')
   local test_fd = io.open(test, 'w')
   local unsup_fd = io.open(unsup, 'w')
   local lines = ifd:lines()
   lines()
   for line in lines do
      n = n + 1
      if math.fmod(n, 10000) == 0 then
         io.write('\rProcessing line: ', n)
         io.flush()
      end

      local content = joe.parseCSVLine(line)
      local split = content[2]:gsub("^%s*(.-)%s*$", "%1")
      local review = content[3]:gsub("^%s*(.-)%s*$", "%1")
      local class = joe.CLASS[content[4]:gsub("^%s*(.-)%s*$", "%1")]
      if split == 'train' and class < 2 then
         train_fd:write(joe.str2hex(review), ' ', class, '\n')
      elseif split == 'test' then
         test_fd:write(joe.str2hex(review), ' ', class, '\n')
      else
         unsup_fd:write(joe.str2hex(review), ' ', class, '\n')
      end
   end
   print('\rProcessed lines: '..n)
   ifd:close()
   train_fd:close()
   test_fd:close()
   unsup_fd:close()
end

function joe.str2hex(str)
   local char2hex = function (c)
      return string.format('%02x', string.byte(c))
   end
   return str:gsub('.', char2hex)
end

-- Parsing csv line
-- Ref: http://lua-users.org/wiki/LuaCsv
function joe.parseCSVLine(line,sep)
   local res = {}
   local pos = 1
   sep = sep or ','
   while true do
      local c = string.sub(line,pos,pos)
      if (c == "") then break end
      if (c == '"') then
         -- quoted value (ignore separator within)
         local txt = ""
         repeat
            local startp,endp = string.find(line,'^%b""',pos)
            txt = txt..string.sub(line,startp+1,endp-1)
            pos = endp + 1
            c = string.sub(line,pos,pos)
            if (c == '"') then txt = txt..'"' end
            -- check first char AFTER quoted string, if it is another
            -- quoted string without separator, then append it
            -- this is the way to "escape" the quote char in a quote.
         until (c ~= '"')
         table.insert(res,txt)
         assert(c == sep or c == "")
         pos = pos + 1
      else
         -- no quotes used, just look for the first separator
         local startp,endp = string.find(line,sep,pos)
         if (startp) then
            table.insert(res,string.sub(line,pos,startp-1))
            pos = endp + 1
         else
            -- no separator found -> use rest of string and terminate
            table.insert(res,string.sub(line,pos))
            break
         end
      end
   end
   return res
end

joe.main()
return joe

