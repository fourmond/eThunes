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


# All documents are instances of this class. 
class DocumentDefinition

  attr_accessor :name

  def initialize(n)
    @name = n
  end

  def public_name(n = nil)
    @public_name = (n || @public_name)
  end

  def format(n = nil)
    @format = (n || @format)
  end

  def display(n = nil)
    @display = (n || @display)
  end
end

class CollectionDefinition

  @collections = {}

  @last_registered = nil

  # Gives a few information about the collection
  def self.collection(name, pubname, desc)
    @name = name
    @public_name = pubname
    @description = desc

    CollectionDefinition.register_definition(self)
  end

  def self.name
    return @name
  end

  def self.public_name
    return @public_name
  end

  def self.description
    return @description
  end

  def self.register_definition(cls)
    @collections[cls.name] = cls
    @last_registered = cls
  end


  def self.collections
    return @collections
  end

  def self.document(name, &blk)
    @documents ||= {}
    doc = DocumentDefinition.new(name)

    # This block enables one to 
    doc.instance_exec(&blk)

    if !doc.respond_to? :parse
      warn "Document type #{name} does not have a parse method"
    else
      @documents[name] = doc
    end
  end

  def self.documents
    return @documents
  end
end

# Use:
# class Def < CollectionDefinition
#
#
# 
#   document 'biniou' do
#     public_name "bidule"
#     display "machin"
#     def parse(doc)
#     end
#   end
# end
