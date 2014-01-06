# -*- coding: utf-8 -*-
CollectionDefinition.collection('bleu-ciel') do

  public_name "EDF Bleu Ciel"
  description <<EOD
Factures EDF Bleu Ciel
EOD
  
  document 'facture' do
    public_name "Facture"
    display "Facture du %{date%date:d/MM/yyyy}: %{montant%A} (%{ref-client} -- %{facture})"
    format "Facture-%{date%date:yyyy-MM}.pdf"

    matcher 'prelevement', 'montant', 9

    def parse(doc)
      res = {}
      if doc['text'] =~ /Facture.*?\s+du\s+(#{Dates::FrenchFullDateRE})/i
        res['date'] = Dates::parse_french_date($1)
      elsif doc['text-layout'] =~ /Facture.*?\s+du\s+(#{Dates::DateRE})/i
        res['date'] = Dates::parse_french_date($1)
      end
      if doc['text'] =~ /N(.|..)\s*facture\s*:\s*(\S+)/i
        res['facture'] = $2.chomp
      end
      if doc['text'] =~ /N(.|..)\s*client\s*:\s*([\d\s]+)/i
        res['ref-client'] = $2.chomp
      end
      if doc['text-layout'] =~ /montant\s*\s*(?:non\s*)?\S+\s*(le\s*(\d+)\/(\d+)\/(\d+))?\s*([-\d]+)[.,](\d+).*?TTC/i
        if $1
          res['prelevement'] = Time.local($4.to_i, $3.to_i, $2.to_i)
        else
          res['prelevement'] = res['date'] + 7 * 24 * 3600
        end
        units = $5.to_i * 100
        if units > 0
          units += $6.to_i
        else
          units -= $6.to_i
        end
        res['montant'] = units
      end
      return res
    end
  end
end
