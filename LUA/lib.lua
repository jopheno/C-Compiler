function split(str, pat)
	local t = {}  -- NOTE: use {n = 0} in Lua-5.0
	local fpat = "(.-)" .. pat
	local last_end = 1
	local s, e, cap = str:find(fpat, 1)

	while s do
		 if s ~= 1 or cap ~= "" then
			 table.insert(t,cap)
		 end

		 last_end = e+1
		 s, e, cap = str:find(fpat, last_end)
	end

	if last_end <= #str then
		 cap = str:sub(last_end)
		 table.insert(t, cap)
	end

	return t
end

string.explode = split

function bits(num, amount)
	 local t={}
	 while num>0 do
			 rest=num%2
			 table.insert(t,1,rest)
			 num=math.floor((num-rest)/2)
	 end

 local str = table.concat(t)

 if not amount then
	 return str
 end

 while (string.len(str) < amount) do
	 str = "0"..str
 end

 return str
end