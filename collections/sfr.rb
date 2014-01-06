# -*- coding: utf-8 -*-
CollectionDefinition.collection('sfr') do

  public_name "SFR"
  description <<EOD
Factures SFR
EOD

  document 'facture' do
    public_name 'Facture'
    display "Facture du %{date%date:d/MM/yyyy}: %{montant%A} (%{ref-client} -- %{facture})"
    format "Facture-%{date%date:yyyy-MM}.pdf"

    matcher 'prelevement', 'montant', 4

    def parse(doc)
      res = {}
      if doc['text'] =~ /Facture\s+du\s+(\d+)\/(\d+)\/(\d+)/i
        res['date'] = Time.local($3.to_i, $2.to_i, $1.to_i)
      end
      if doc['text'] =~ /de\s*Facture\s*:\s*(\S+)/i
        res['facture'] = $1
      end
      if doc['text'] =~ /client\s*:\s*(\S+)/i
        res['ref-client'] = $1
      end
      if doc['text'] =~ /MONTANT\s*TOTAL\s*\S+\s*le\s*(\d+)\/(\d+)\/(\d+)\s*(\d+)[.,](\d+)/
        res['prelevement'] = Time.local($3.to_i, $2.to_i, $1.to_i)
        res['montant'] = $4.to_i * 100 + $5.to_i
      end
      return res
    end
  end
end
