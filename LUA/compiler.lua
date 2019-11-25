

local regs = {
	["0"] = {
		opcode = "00000000",
		size = 32
	},
	["1"] = {
		opcode = "00000001",
		size = 32
	},
	["eax"] = {
		opcode = "10000000",
		size = 32
	},
	["ax"] = {
		opcode = "10000001",
		size = 16
	},
	["al"] = {
		opcode = "10000010",
		size = 8
	},
	["ah"] = {
		opcode = "10000011",
		size = 8
	},
	["aw"] = {
		opcode = "10000101",
		size = 16
	},
	["as"] = {
		opcode = "10000110",
		size = 8
	},
	["ap"] = {
		opcode = "10000111",
		size = 8
	},


	["ebx"] = {
		opcode = "10100000",
		size = 32
	},
	["bx"] = {
		opcode = "10100001",
		size = 16
	},
	["bl"] = {
		opcode = "10100010",
		size = 8
	},
	["bh"] = {
		opcode = "10100011",
		size = 8
	},
	["bw"] = {
		opcode = "10100101",
		size = 16
	},
	["bs"] = {
		opcode = "10100110",
		size = 8
	},
	["bp"] = {
		opcode = "10100111",
		size = 8
	},

	["ecx"] = {
		opcode = "11000000",
		size = 32
	},
	["cx"] = {
		opcode = "11000001",
		size = 16
	},
	["cl"] = {
		opcode = "11000010",
		size = 8
	},
	["ch"] = {
		opcode = "11000011",
		size = 8
	},
	["cw"] = {
		opcode = "11000101",
		size = 16
	},
	["cs"] = {
		opcode = "11000110",
		size = 8
	},
	["cp"] = {
		opcode = "11000111",
		size = 8
	},


	["edx"] = {
		opcode = "11100000",
		size = 32
	},
	["dx"] = {
		opcode = "11100001",
		size = 16
	},
	["dl"] = {
		opcode = "11100010",
		size = 8
	},
	["dh"] = {
		opcode = "11100011",
		size = 8
	},
	["dw"] = {
		opcode = "11100101",
		size = 16
	},
	["ds"] = {
		opcode = "11100110",
		size = 8
	},
	["dp"] = {
		opcode = "11100111",
		size = 8
	},

	["clock"] = {
		opcode = "00100010",
		size = 2,
	},

	["clk"] = {
		opcode = "00100010",
		size = 2,
	},

	["top"] = {
		opcode = "00100000",
		size = 32,
	},

	["stackamount"] = {
		opcode = "00100001",
		size = 16,
	},

	["esp"] = {
		opcode = "10010011",
		size = 16,
	},

	["kernel"] = {
		opcode = "01100100",
		size = 32,
	},
}

local arch = {

	["ALU"] = {
		opcode = "010",

		instr = {
			["ADD"] = 	{opcode = "00001", form = "dec:instr:third:second:first"},
			["SUB"] = 	{opcode = "00010", form = "dec:instr:third:second:first"},
			["MULT"] = 	{opcode = "00011", form = "dec:instr:third:second:first"},
			["DIV"] = 	{opcode = "00100", form = "dec:instr:third:second:first"},
			["REM"] = 	{opcode = "00101", form = "dec:instr:third:second:first"},
			["ABS"] = 	{opcode = "00110", form = "dec:instr:third:second:first"},
			["NOT"] = 	{opcode = "00111", form = "dec:instr:third:nop:first"},
			["AND"] = 	{opcode = "01000", form = "dec:instr:third:second:first"},
			["NAND"] = 	{opcode = "01001", form = "dec:instr:third:second:first"},
			["OR"] = 	{opcode = "01010", form = "dec:instr:third:second:first"},
			["NOR"] = 	{opcode = "01011", form = "dec:instr:third:second:first"},
			["XOR"] = 	{opcode = "01100", form = "dec:instr:third:second:first"},
			["XNOR"] = 	{opcode = "01101", form = "dec:instr:third:second:first"},
			["SET"] = 	{opcode = "10000", form = "dec:instr:third:second:first"},
			["SLT"] = 	{opcode = "10001", form = "dec:instr:third:second:first"},
			["SGT"] = 	{opcode = "10010", form = "dec:instr:third:second:first"},
			["SDT"] = 	{opcode = "10011", form = "dec:instr:third:second:first"},
			["SLET"] = 	{opcode = "10101", form = "dec:instr:third:second:first"},
			["SGET"] = 	{opcode = "10110", form = "dec:instr:third:second:first"},
		}
	},

	["ALUi"] = {
		opcode = "011",

		instr = {
			["ADD"] = 	{opcode = "00001", form = "dec:instr:third:im16"},
			["SUB"] = 	{opcode = "00010", form = "dec:instr:third:im16"},
			["MULT"] = 	{opcode = "00011", form = "dec:instr:third:im16"},
			["DIV"] = 	{opcode = "00100", form = "dec:instr:third:im16"},
			["REM"] = 	{opcode = "00101", form = "dec:instr:third:im16"},
			["ABS"] = 	{opcode = "00110", form = "dec:instr:third:im16"},
			["NOT"] = 	{opcode = "00111", form = "dec:instr:third:im16"},
			["AND"] = 	{opcode = "01000", form = "dec:instr:third:im16"},
			["NAND"] = 	{opcode = "01001", form = "dec:instr:third:im16"},
			["OR"] = 	{opcode = "01010", form = "dec:instr:third:im16"},
			["NOR"] = 	{opcode = "01011", form = "dec:instr:third:im16"},
			["XOR"] = 	{opcode = "01100", form = "dec:instr:third:im16"},
			["XNOR"] = 	{opcode = "01101", form = "dec:instr:third:im16"},
			["SET"] = 	{opcode = "10000", form = "dec:instr:third:im16"},
			["SLT"] = 	{opcode = "10001", form = "dec:instr:third:im16"},
			["SGT"] = 	{opcode = "10010", form = "dec:instr:third:im16"},
			["SDT"] = 	{opcode = "10011", form = "dec:instr:third:im16"},
			["SLET"] = 	{opcode = "10101", form = "dec:instr:third:im16"},
			["SGET"] = 	{opcode = "10110", form = "dec:instr:third:im16"},
		}
	},

	["DMA"] = {
		opcode = "100",

		instr = {
			["RAI"] = 	{opcode = "01001", form = "dec:instr:third:nop:nop"},
			["RI"] = 	{opcode = "01010", form = "dec:instr:third:im16"},
			["GIA"] = 	{opcode = "01011", form = "dec:instr:third:nop:nop"},
			["SAO"] = 	{opcode = "00001", form = "dec:instr:nop:nop:first"},
			["SO"] = 	{opcode = "00010", form = "dec:instr:im8:nop:first"},
			["SOi"] = 	{opcode = "00011", form = "dec:instr:third:im16"},
			["PAUSE"] = 	{opcode = "01100", form = "dec:instr:third:nop:nop"},
			--["MRB"] = 	{opcode = "10001", form = "dec:instr:third:second:first"},
			--["MSB"] = 	{opcode = "10010", form = "dec:instr:third:second:first"},
			["LOADi"] = 	{opcode = "10010", form = "dec:instr:third:im16"},
			["STOREi"] = 	{opcode = "10011", form = "dec:instr:third:im16"},
			["LOAD"] = 		{opcode = "10000", form = "dec:instr:third:nop:first"},
			["STORE"] = 	{opcode = "10001", form = "dec:instr:third:nop:first"},
			["POP"] = 		{opcode = "10100", form = "dec:instr:third:nop:nop"},
			["PUSH"] = 		{opcode = "10101", form = "dec:instr:nop:nop:first"},
			["SP"] = 		{opcode = "10110", form = "dec:instr:nop:nop:first"},
			["REQFHDD"] = 	{opcode = "11000", form = "dec:instr:third:second:first"},
			["REQFRAM"] = 	{opcode = "11001", form = "dec:instr:third:second:first"},
		}
	},

	["STACK"] = {
		opcode = "001",

		instr = {
			["POP"] = 	{opcode = "00001", form = "dec:instr:third:second:first"},
			["PUSH"] = 	{opcode = "00010", form = "dec:instr:third:second:first"},
			["PUSHi"] = {opcode = "00011", form = "dec:instr:third:im16"},
			["GSA"] = 	{opcode = "00100", form = "dec:instr:third:second:first"},
		}
	},

	["PRG"] = {
		opcode = "111",

		instr = {
			["NOP"] = 		{opcode = "00000", form = "dec:instr:nop:nop:nop"},
			["JMP"] = 		{opcode = "00001", form = "dec:instr:third:second:first"},
			["JMPi"] = 		{opcode = "01001", form = "dec:instr:im24"},
			["JMPfi"] = 	{opcode = "01010", form = "dec:instr:im24"},
			["JMPbi"] = 	{opcode = "01011", form = "dec:instr:im24"},

			["JMPC"] = 		{opcode = "00101", form = "dec:instr:third:second:first"},
			["JMPCi"] = 	{opcode = "01101", form = "dec:instr:third:im16"},
			["JMPCfi"] = 	{opcode = "01110", form = "dec:instr:third:im16"},
			["JMPCbi"] = 	{opcode = "01111", form = "dec:instr:third:im16"},

			["CALL"] = 		{opcode = "10000", form = "dec:instr:nop:nop:first"},
			["CALLi"] = 	{opcode = "10001", form = "dec:instr:nop:im16"},
			["RET"] = 		{opcode = "10010", form = "dec:instr:none"},
			["WAIT"] = 		{opcode = "10011", form = "dec:instr:none"},
			["HALT"] = 		{opcode = "11000", form = "dec:instr:none"},

			["PUSH"] =		{opcode = "11001", form = "dec:instr:nop:nop:first"},
			["POP"] = 		{opcode = "11010", form = "dec:instr:third:nop:nop"},
			["GSA"] = 		{opcode = "11011", form = "dec:instr:third:nop:nop"},
			["SWITCH"] = 	{opcode = "11100", form = "dec:instr:nop:nop:first"},
			["SWITCHi"] = 	{opcode = "11101", form = "dec:instr:nop:im16"},
			["SYSCALL"] = 	{opcode = "11110", form = "dec:instr:nop:nop:nop"},
			["GTP"] = 		{opcode = "01000", form = "dec:instr:nop:nop:first"},
		}
	},

	["SCHED"] = {
		opcode = "101",

		instr = {
			["NOP"] = 			{opcode = "00000", form = "dec:instr:nop:nop:nop"},
			["SYSCalli"] = 		{opcode = "10001", form = "dec:instr:nop:im16"},
			["Timeri"] = 		{opcode = "10010", form = "dec:instr:nop:im16"},
			["DMAInti"] = 		{opcode = "10011", form = "dec:instr:nop:im16"},
			["Start"] = 		{opcode = "00001", form = "dec:instr:nop:nop:nop"},
			["RTimer"] = 		{opcode = "00010", form = "dec:instr:nop:nop:nop"},
			["RETR"] = 			{opcode = "00011", form = "dec:instr:third:nop:nop"},
			["PC"] = 			{opcode = "00100", form = "dec:instr:third:nop:nop"},
		}
	
	},
}

dofile("lib.lua")

local labels = {}
local alabels = {}

local debug = true
local outfile_name = nil
local initial_value = nil
local resolve_mem_addr = nil
local inst_amount = 0

function onStart()
	local insts = {}

	if not arg[1] or arg[1] ~= "auto" then
		debug = false
	end

	if not arg[2] then
		outfile_name = "output.mif"
	else
		outfile_name = arg[2]
	end

	if not arg[3] then
		initial_value = 0
	else
		initial_value = tonumber(arg[3])
	end

	if not arg[4] then
		resolve_mem_addr = 0
	else
		if arg[4] == "true" then
			resolve_mem_addr = true
		end
	end

	if not debug then print("> Enter the JophAssembly file to be read: ") end

	local fileName = io.read()

	if fileName == "" then
	  fileName = "example.ja"
	end

	if not debug then print(">> Trying to read "..fileName.." ...") end

	local file, error_msg = io.open(fileName, 'rb')
	if not file then
	  onError(error_msg)
	  return
	end

	local buffer = file:read('*all')
	if not debug then print(">> File was read successfully !") end
	file:close()

	labels = {}
	alabels = {}
	local lines = string.explode(buffer, "\n")

	local line = 1
	for index, inst in pairs(lines) do
		if string.len(inst) > 4 then
			onGetLabels(line, inst)
			line = line + 1
		end
	end

	inst_amount = line-1

	local line = 1
	for index, inst in pairs(lines) do
		if string.len(inst) > 4 then
			local final_instr = onPrepareInstruction(line, inst)

			if not final_instr then
				return false
			end

			table.insert(insts, final_instr)
			line = line + 1
		end
	end

	onFinish(insts)

end

function onError(msg, line)
	error(">> ["..line.."] Fatal Error Detected -> "..msg)
end

function onGetLabels(line, inst)
	local inst_info = {}
	local label = nil

	inst = string.gsub(inst, "\r", "")

	if string.find(inst, "::") then
		local t = string.explode(inst, "::")
		label = t[2]
		inst = string.gsub(inst, "::"..label.."::", "")
		if labels[label] then
			onError("Label "..label.." already declared !", line)
			return false
		end

		--print("Found the label "..label.." at line "..line..".")

		if label == "@" then
			table.insert(alabels, line)
		else
			labels[label] = line
		end
	end

end

function onPrepareInstruction(line, inst)
	local inst_info = {}
	local label = nil

	inst = string.gsub(inst, "\r", "")

	if string.find(inst, "//") then
		local t = string.explode(inst, "//")
		inst = t[1]
	end

	inst = string.gsub(inst, ",", " ")
	inst = string.gsub(inst, "   ", " ")
	inst = string.gsub(inst, "  ", " ")

	local t = string.explode(inst, "_")
	local concat = ""

	for i = 2, #t do
		if i == 2 then
			concat = concat .. t[i]
		else
			concat = concat .. "_" .. t[i]
		end
	end
	
	local f = string.explode(concat, " ")

	--for ind, inf in pairs(t) do print("["..ind.."] "..inf) end
	--for ind, inf in pairs(f) do print("["..ind.."] "..inf) end

	inst_info.label = label
	inst_info.line = line

	inst_info[1] = t[1]
	for i = 1, #f do
		inst_info[i+1] = f[i]
	end

	--for ind, inf in pairs(inst_info) do print("["..ind.."] "..inf) end

	return onCompileInstruction(inst_info)

end

function convertRegister(reg, line)

	if not regs[reg] then
		onError("Unable to find register.", line)
		return false
	end

	return regs[reg].opcode

end

function convertLabel(lb, line)

	local nline = nil

	if string.find(lb, "@b") then
		local dist = 999999

		for index, aline in ipairs(alabels) do
			if aline < line and dist > line-aline then
				dist = line-aline
				nline = aline
			end
		end
	elseif string.find(lb, "@f") then
		local final_pos = nil
		local dist = 999999

		for index, aline in ipairs(alabels) do
			if aline > line and dist > aline-line then
				dist = aline-line
				nline = aline
			end
		end
	else
		nline = labels[lb]
	end

	-- Verify if the first character is m from memory addr
	if not nline then
		if string.sub(lb, 1, 1) == "m" then
			number = tonumber(string.sub(lb, 2, string.len(lb)))

			if number and resolve_mem_addr then
				number = number + (inst_amount + 1)
			end

			nline = number
		end
	end

	return nline
end

local translations = {
	["8BITS"] = 128,
	["16BITS"] = 129,

	["DISPLAY1"] = 129,
	["DISPLAY2"] = 130,
	["DISPLAY3"] = 131,
}

function translate(lb, line)
	return translations[lb]
end

function onCompileInstruction(info)
	local dec = info[1]
	local inst = info[2]

	local decInfo = arch[dec]
	local instInfo = decInfo.instr[inst]

	local final = ""

	if not decInfo then
		onError("There isn't any specialized decoder for "..dec, info.line)
		return false
	end

	if not instInfo then
		onError("There isn't any instruction decoder for "..inst, info.line)
		return false
	end

	local form = string.explode(instInfo.form, ":")

	local v = 0

	for index, formInfo in pairs(form) do
		if formInfo == "dec" then
			v = v + 1
			final = final .. decInfo.opcode
		elseif formInfo == "instr" then
			v = v + 1
			final = final .. instInfo.opcode
		elseif formInfo == "third" then
			v = v + 1
			local third = info[v]

			final = final .. convertRegister(third, info.line)
		elseif formInfo == "second" then
			v = v + 1
			local second = info[v]

			final = final .. convertRegister(second, info.line)
		elseif formInfo == "first" then
			v = v + 1
			local first = info[v]

			final = final .. convertRegister(first, info.line)
		elseif formInfo == "nop" then

			final = final .. bits(0, 8)
		elseif formInfo == "im8" then
			v = v + 1
			local im8 = tonumber(info[v])

			if not im8 then
				im8 = translate(info[v], info.line)
			end

			if not im8 then
				onError("Unable to find a value at instruction !", info.line)
				return false
			end

			final = final .. bits(im8, 8)
		elseif formInfo == "im16" then
			v = v + 1
			local im16 = tonumber(info[v])

			if not im16 then
				im16 = convertLabel(info[v], info.line)
			end

			if not im16 then
				im16 = translate(info[v], info.line)
			end

			if not im16 then
				onError("Unable to find a value at instruction !", info.line)
				return false
			end

			final = final .. bits(im16, 16)
		elseif formInfo == "im24" then
			v = v + 1
			local im24 = tonumber(info[v])

			if not im24 then
				im24 = convertLabel(info[v], info.line)
			end

			if not im24 then
				im24 = translate(info[v], info.line)
			end

			if not im24 then
				onError("Unable to find a value at instruction !", info.line)
				return false
			end

			final = final .. bits(im24, 24)
		elseif formInfo == "none" then
			final = final .. bits(0, 24)
		end
	end

	return final

end

function onFinish(insts)
	local data = "WIDTH=32;\nDEPTH=1024;\nADDRESS_RADIX=UNS;\nDATA_RADIX=BIN;\n\nCONTENT BEGIN\n"

	data = data ..tonumber(initial_value).." : 0000000000000000"..bits(inst_amount, 16)..";\n"

	for index, inst in pairs(insts) do
		data = data .. (index + initial_value) .." : "..inst..";\n"
	end

	data = data .. "END;\n"

	local file = io.open(outfile_name, 'w+')
    file:write(data)
    file:close()

	print(">> Everything went just fine.")
	print(">> MIF file created successfully !")
end

onStart()
