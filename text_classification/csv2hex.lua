--[[
Convert CSV text classification datasets to hex format with base-0 class
Copyright 2019 Xiang Zhang All Rights Reserved.

Usage: th csv2hex.lua [input] [output]
--]]

local io = require('io')
local math = require('math')

-- A Logic Named Joe
local joe = {}

function joe.main()
   local input = arg[1] or 'data/dianping/train.csv'
   local output = arg[2] or 'data/dianping/train.hex'

   print('Convert to hex')
   joe.csv2hex(input, output);
end

function joe.csv2hex(input, output)
   local n = 0
   local ifd = io.open(input, 'r')
   local ofd = io.open(output, 'w')
   for line in ifd:lines() do
      n = n + 1
      if math.fmod(n, 10000) == 0 then
         io.write('\rProcessing line: ', n)
         io.flush()
      end

      local content = joe.parseCSVLine(line)
      local class = tonumber(content[1])

      if #content > 1 then
         content[2] = content[2]:gsub('\\n', '\n'):gsub("^%s*(.-)%s*$", "%1")
         ofd:write(joe.str2hex(content[2]))
      end
      for i = 3, #content do
         content[i] = content[i]:gsub('\\n', '\n'):gsub("^%s*(.-)%s*$", "%1")
         ofd:write('00', joe.str2hex(content[i]));
      end
      ofd:write(' ', class - 1, '\n')
   end
   print('\rProcessed lines: '..n)
   ifd:close()
   ofd:close()
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
