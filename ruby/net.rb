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

module Net

  # This class wraps the fetch call into a Fiber
  def self.fetch(tg, fetcher, creds, ary)
    return Fiber.new do 
      tg.fetch(fetcher, creds, ary)
    end
  end

  # This class is defined by the C code first
  class Fetcher

    private :private_get, :private_post

    # Returns the result of the get operation
    def get(url)
      private_get(url)
      Fiber.yield
    end

    def post(url, params)
      private_post(url, params)
      Fiber.yield
    end
  end

  # This class is defined by the C code first too.
  class Result

    # Returns a couple:
    # * form target
    # * all hidden input as a hash
    #
    # @todo Type checking
    def form_data(id = 0)
      form = css('form')[id]
      tg = normalize_url(form['action'])
      frm = {}
      for input in form.css("input")
        if input['type'] == 'hidden'
          i = input['name']
          v = input['value']
          if i && v
            frm[i] = v
          end
        end
      end
      return [tg, frm]
    end


    # Returns the hrefs of all links matching the given css attribute
    # (don't forget the a selector !)
    def linked_hrefs(css_selector, strip_spaces = true)
      lnks = css(css_selector)
      ret = []
      for l in lnks
        if l['href']
          url = l['href']
          if strip_spaces
            url.gsub!(/\s/,'')
          end
          ret << normalize_url(url)
        end
      end
      return ret
    end
  end
  
end
