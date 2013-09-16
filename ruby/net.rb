# net.rb: other sides of the Net implementation
# copyright (c) 2010, 2013 by Vincent Fourmond
  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
  
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details (in the COPYING file).


# We definitely need continuations
p $:
begin
  # require 'continuation'
  require '/usr/lib/ruby/1.9.1/x86_64-linux/continuation.so'
rescue Exception => e
  p e
end

p :bidule

module Net

  # This exception is raised to give control back to the calling C
  # code when a network transfer is under way
  class OngoingException < Exception
  end

  def self.fetch(mod, fetcher, creds, ary)
    mod.fetch(fetcher, creds, ary)
  end

  # This class is defined by the C code first
  class Fetcher

    private :private_get, :private_post

    # Returns the result of the get operation
    def get(url)
      return callcc {|cc| 
        private_get(url, cc)
        raise OngoingException.new
      }
    end

    def post(url, params)
      return callcc {|cc| 
        private_post(url, params, cc)
        raise OngoingException.new
      }
    end
  end

  # This class is defined by the C code first too.
  class Result

    # Returns the links contained in the #contents of the object.
    def links
      lnks = []
      contents.scan(/<a\s*([^>]*?)\s*href=['"]([^'"]+)['"]\s*([^>]*?)\s*>(.*?)<\/a>/) do |ary|
        link = {}
        link[:target] = $2
        link[:name] = $4
        link[:pre] = $1
        link[:post] = $3
        if link[:target] =~ /^\w+:\/\// # Absolute
          link[:absolute_target] = link[:target]
        else
          link[:absolute_target] = url + "/" + link[:target]
        end
        # TODO: rough parsing of other fields ?
        lnks << link
      end
      return lnks
    end
  end
  
end
