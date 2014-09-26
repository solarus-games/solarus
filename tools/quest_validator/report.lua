-- Provides functions to reports errors and warnings during the validation.

local report = {}

local log_count = {}

-- Returns the current file and line of a stack frame (depth 0 means you)
function report.get_file_line(depth)

  local info = debug.getinfo(depth + 1, "Sl")
  local file = info.source:sub(2)
  local line = info.currentline
  return file, line
end

local function log(level, message, file, line)

  if log_count[level] == nil then
    log_count[level] = 0
  end
  log_count[level] = log_count[level] + 1

  io.stderr:write("[" .. level .. "]")
  if file then
    io.stderr:write(" " .. file .. ":")
    if line then
      io.stderr:write(line .. ":")
    end
  end
  io.stderr:write(" " .. message .. "\n")
end

-- Prints a warning message with optional file name and line number.
function report.warning(message, file, line)
  log("WARNING", message, file, line)
end

-- Prints an error message with optional file name and line number.
function report.error(message, file, line)
  log("ERROR", message, file, line)
end

-- Like report.error, but terminates the program.
function report.fatal(message, file, line)
  log("FATAL", message, file, line)
  os.exit(1)
end

function report.get_num_warnings()
  return log_count["WARNING"] or 0
end

function report.get_num_errors()
  return log_count["ERROR"] or 0
end

return report

