# collectiondefinition.rb: base class for all collection definitions
# copyright (c) 2013 by Vincent Fourmond
  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
  
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details (in the COPYING file).

class CollectionDefinition

  @collections = {}

  # Gives a few information about the collection
  def self.collection(name, pubname, desc)
    @name = name
    @pubname = pubname
    @description = desc

    CollectionDefinition.register_definition(self)
  end

  def self.collection_name
    return @name
  end

  def self.register_definition(cls)
    @collections[cls.collection_name] = cls
  end


  def self.all_collections
    return @collections
  end

end

