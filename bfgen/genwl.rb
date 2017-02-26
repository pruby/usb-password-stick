#!/usr/bin/env ruby

fd = File.read('wordlist.txt')
words = fd.split(/\r?\n/)

words = words.uniq

array = words.join("\0").bytes.to_a
array.push(0) unless array.last == 0

File.open('wordlist.h', 'w') do |out|
  out.puts "#include <stdint.h>"
  out.puts
  out.puts "#ifndef WORDLIST_H"
  out.puts "#define WORDLIST_H"
  out.puts
  out.puts "#define WORDLIST_LENGTH #{array.length}"
  out.puts
  out.puts "const uint8_t wordlist[WORDLIST_LENGTH] = {"
  msg = "  " + array.shift.to_s
  array.each do |v|
    if (msg.length + 2 + v.to_s.length < 79)
      msg += ", " + v.to_s
    else
      msg += ","
      out.puts msg
      msg = "  " + v.to_s
    end
  end
  out.puts msg
  out.puts "};"
  out.puts
  out.puts "#endif"
  out.puts
end
