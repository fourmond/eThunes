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


# Score for a given transaction, a hash containing:
# * amount
# * date
# * memo
# * name
class TransactionMatcher

  # Crate
  def initialize(date_key, amount_key, tol_after, tol_before = 3)
    @date_key = date_key
    @amount_key = amount_key
    @tol_after = tol_after
    @tol_before = tol_before
  end

  def matches(transaction, document)
    date = document[@date_key]
    amount = document[@amount_key]
    if ! date or ! amount
      return -10000
    end
    t_date = transaction["date"]
    t_amount = transaction["amount"]
    if t_amount == amount or t_amount == -amount
      diff = (t_date - date)/86400
      if (diff <= @tol_after) && (diff >= - @tol_before)
        return 10000
      end
    end
    return -10000
  end

  def relevant_date_range(document)
    date = document[@date_key]
    if ! date
      return nil
    end
    return { 'start' => date - @tol_before * 86400,
      'end' => date + @tol_after * 86400
    }
  end

end


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

  def description(n = nil)
    @description = (n || @description)
  end

  # Definition of the list of compulsory attributes
  def attributes(*args)
    @attributes = []
    for a in args
      @attributes << a.to_s
    end
  end

  def matches(transaction, document)
    if @matcher
      return @matcher.matches(transaction, document)
    else
      return -10000
    end
  end

  def relevant_date_range(document)
    if @matcher
      return @matcher.relevant_date_range(document)
    else
      return nil
    end
  end

  def matcher(date_key, amount_key, tolerance)
    @matcher = TransactionMatcher.new(date_key, amount_key, tolerance)
  end
end

class CollectionDefinition

  @collections = {}

  @last_registered = nil

  def initialize(name)
    @name = name
    @public_name = nil
    @description = nil
    @documents = {}
  end

  # Gives a few information about the collection
  def self.collection(name, &blk)
    nc = CollectionDefinition.new(name)
    
    nc.instance_exec(&blk)
    
    CollectionDefinition.register_definition(nc)
    
  end

  def name
    @name
  end

  def public_name(n = nil)
    @public_name = (n || @public_name)
  end

  def description(n = nil)
    @description = (n || @description)
  end

  def self.register_definition(cls)
    @collections[cls.name] = cls
    @last_registered = cls
  end


  def self.collections
    return @collections
  end

  def document(name, &blk)
    doc = DocumentDefinition.new(name)

    # This block enables one to 
    doc.instance_exec(&blk)

    if !doc.respond_to? :parse
      warn "Document type #{name} does not have a parse method"
    else
      @documents[name] = doc
    end
  end

end

# Use:
# CollectionDefinition.collection "name" do
#
#   display_name 'A beautiful collection'
# 
#   document 'biniou' do
#     public_name "bidule"
#     display "machin"
#     def parse(doc)
#     end
#   end
# end
