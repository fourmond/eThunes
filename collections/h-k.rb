# -*- coding: utf-8 -*-
CollectionDefinition.collection('h-k') do

  public_name "H&K"
  description <<EOD
???
EOD
  
  document "DA" do 
    display "Droits d'auteur %{year}: %{amount%A} (date: %{date%date:d/MM/yyyy})"
    format "DA-%{year}.pdf"
    public_name "Droits d'auteur"

    def parse(doc)
      res = {}
      if doc['text-layout'] =~ /(\d+)(?:er)?\s*(#{Dates::FrenchMonthRE})\s*(\d+)/
        year = $3
        res['date'] = Time.local(year.to_i, Dates::parse_french_month($2),$1.to_i)
        res['year'] = year.to_i - 1
      end
      if doc['text-layout'] =~ /Droit\s+net.*#{year}(?:\s*=)?\s+(\d+)\s*[.,]\s*(\d+)\s*e/
        res['amount'] = $1.to_i * 100 + $2.to_i
      end
      return res
    end
  end
end
