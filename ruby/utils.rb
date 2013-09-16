# ruby.rb: utility functions written in Ruby
# copyright (c) 2013 by Vincent Fourmond
  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
  
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details (in the COPYING file).

module Utils

  # This function calls the target function with the target receiver,
  # returning an array [retval] if everything went fine, or [nil,
  # exception] if an exception was caught
  def self.safe_call(target, symbol, *args)
    p [target, symbol, *args]
    begin
      return [target.send(symbol, *args)]
    rescue Exception => e
      return [nil, e]
    end
  end
end
