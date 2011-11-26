# dates.rb: A few utility functions to handle dates and the like
# copyright (c) 2010 by Vincent Fourmond
  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
  
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details (in the COPYING file).

module Dates
  
  eacute = /e|.|../             # Basic but enough (two is for UTF-8)
  
  FrenchMonthsRE = 
    [
     /janvier/i,
     /f#{eacute}vrier/i,
     /mars/i,
     /avril/i,
     /mai/i,
     /juin/i,
     /juillet/i,
     /ao(?:u|.|..)t/i,
     /septembre/i,
     /octobre/i,
     /novembre/i,
     /d#{eacute}cembre/i
    ]
  
  FrenchMonthRE = Regexp::union(FrenchMonthsRE)

  # Returns the number of the french month corresponding to str.
  def self.parse_french_month(str)
    i = 1
    for re in FrenchMonthsRE
      if str =~ re
        return i
      end
      i += 1
    end
    return nil
  end

  FrenchFullDateRE = /(\d+)\s+(#{FrenchMonthRE})\s+(\d+)/ # for emacs

  DateRE = /(\d+)\/(\d+)\/(\d+)/ # for emacs
    
  # Parses a French date
  def self.parse_french_date(date)
    if date =~ FrenchFullDateRE
      return Time.local($3.to_i, Dates::parse_french_month($2), $1.to_i)
    elsif date =~ DateRE
      return Time.local($3.to_i, $2.to_i, $1.to_i)
    else
      return nil
    end
  end

end
